//   This file is part of the gussound library, licensed under the terms of the MIT License.
//
//   The MIT License
//   Copyright (C) 2010-2014  Augustin Preda (thegusty999@gmail.com)
//
//   Permission is hereby granted, free of charge, to any person obtaining a copy
//   of this software and associated documentation files (the "Software"), to deal
//   in the Software without restriction, including without limitation the rights
//   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//   copies of the Software, and to permit persons to whom the Software is
//   furnished to do so, subject to the following conditions:
//
//   The above copyright notice and this permission notice shall be included in
//   all copies or substantial portions of the Software.
//
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//   THE SOFTWARE.
//
//    Audio utility class.

//
// Includes
//

//
// Own header: The audiere bound implementation.
//
#include "gopenal.h"

//
// C++ system headers.
//

#include <string>
#include <vector>

//
// This / other libraries' headers.
//

#include "alut.h"

// The abstract sound interface
#include "gussound.h"

// add ogg stream support.
#include "goggstreamedsound.h"


//
// Add guslib inclusions... unless we're dealing with the standalone version
//

#ifdef GUSSOUND_STANDALONE

// Tracing util
#include "gussoundutil/trace.h"

// String parsing utility.
#include "gussoundutil/stringutil.h"

#else

// Tracing util
#include "guslib/trace/trace.h"

// String parsing utility.
#include "guslib/util/stringutil.h"

#endif


#define GOPENAL_MAJOR_VERSION 1
#define GOPENAL_MINOR_VERSION 0
#define GOPENAL_PATCH_VERSION 1


#define ATLVL 4

namespace GOpenAl
{
  // ------------------------------------------ SoundException -----------------------------------------

  SoundException::SoundException(const char* text)
  {
    this->exception_message_ = text;
  }

  SoundException::~SoundException() throw()
  {
  }

  const char* SoundException::what() const throw()
  {
    return this->exception_message_.c_str();
  }

  // ------------------------------------------ GOpenAlDevice -----------------------------------------

  GOpenAlDevice::GOpenAlDevice()
    : gussound::AudioDevice(),
      alcContext_(0),
      alcDevice_(0)
  {
    ALenum myErr;

    // calling alGetError here already returns an error
    // so checking the boolean only, which indicates that all is good by returning true
    ALboolean b = alutInitWithoutContext(NULL, NULL);
    if (false == b)
    {
      throw SoundException("Unable to initialize alut!");
    }

    alcDevice_ = alcOpenDevice(NULL);

    // alGetError still returns the same error, but the device is still created
    // according to the docs, alcDevice_ does return a 0 pointer if something is wrong
    if (NULL == alcDevice_)
    {
      throw SoundException("Unable to open OpenAL device!");
    }

    // same here
    alcContext_ = alcCreateContext(alcDevice_, NULL);
    if (NULL == alcContext_)
    {
      alcCloseDevice(alcDevice_);
      throw SoundException("Unable to create OpenAL context!");
    }

    b = alcMakeContextCurrent(alcContext_);  // returns true -> ok
    myErr = alGetError();  // this actually return 0 here
    if (false == b)
    {
      throw SoundException("Unable to make the current context current!");
    }

    GTRACE(2, "Using OpenAL, version: " << alGetString(AL_VERSION)
      << "; vendor: " << alGetString(AL_VENDOR)
      << "; renderer: " << alGetString(AL_RENDERER));

    alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
    myErr = alGetError();
    if (myErr)
    {
      throw SoundException("Unable to set the distance model to: AL_LINEAR_DISTANCE_CLAMPED");
    }
  }


  GOpenAlDevice::~GOpenAlDevice()
  {
    GTRACE(2, "Destroying OpenAL device, clearing callbacks, etc.");

    if (alcContext_)
    {
      alcDestroyContext(alcContext_);
    }

    if (alcDevice_)
    {
      alcCloseDevice(alcDevice_);
    }

    alutExit();
    GTRACE(2, "Destroyed OpenAL device");
  }


  // ------------------------------------------ GOpenAlSound -----------------------------------------


  GOpenAlSound::GOpenAlSound(OpenAlSoundRepository* repoPtr, const std::string & id)
    : EffectableSound(id),
      sampleRate_(0),
      repeating_(false),
      elapsedFromStart_(0),
      source_(0),
      buffer_(0),
      cachedBufferLength_(0),
      repositoryPtr_(repoPtr)
  {
  }


  GOpenAlSound::~GOpenAlSound()
  {
    if (getState() != gussound::GPLS_Stopped)
    {
      stop();
    }

    // Delete the sound buffer
    if (NULL != buffer_)
    {
      alDeleteBuffers(1, &buffer_);
    }

    // Note: the sound sources are managed and deleted by the repository.
    GTRACE(8, "Low lvl sound destruction");
  }

  //
  // Retrieve a source that can be used for playing. In case no available sources can be found
  // a new source is created.
  //
  ALuint GOpenAlSound::getAvailableSourceForPlay()
  {
    if (NULL == repositoryPtr_)
    {
      return 0;
    }
    try
    {
      // Get a free source (an exception is thrown if none is available).
      ALuint availableSource = repositoryPtr_->getAvailableSourceForPlay();

      // The source is not guaranteed to have the current buffer attahced to it.
      alSourcei(availableSource, AL_BUFFER, buffer_);

      // Return it.
      return availableSource;
    }
    catch (const std::exception &e)
    {
      GTRACE(2, "Could not get a source to play sound!" << e.what());
      return 0;
    }
  }


  /// Utility function to retrieve the time that is currently elapsed from the stream.
  gussound::SoundDuration GOpenAlSound::getStreamElapsedTime()
  {
    if (0 == source_)
    {
      return 0;
    }

    float elapsed;
    alGetSourcef(source_, AL_SEC_OFFSET, &elapsed);
    return elapsed;
  }


  /// Utility function to retrieve the length of the stream.
  gussound::SoundDuration GOpenAlSound::getStreamLength()
  {
    return cachedBufferLength_;
  }


  /// Utility function to calculate and retrieve the length of the stream.
  gussound::SoundDuration GOpenAlSound::calculateAndRetrieveStreamLength()
  {
    ALint bufferSize, frequency, bitsPerSample, channels;
    ALenum myErr;

    alGetBufferi(buffer_, AL_SIZE, &bufferSize);
    myErr = alGetError();
    if (myErr)
    {
      GTRACE(2, "Warning: AL error encountered while trying to get buffer size: " << alGetString(myErr));
      std::string errorMessage("Failed to calculate stream data for: ");
      errorMessage.append(getId());
      throw SoundException(errorMessage.c_str());
    }

    alGetBufferi(buffer_, AL_FREQUENCY, &frequency);
    myErr = alGetError();
    if (myErr)
    {
      GTRACE(2, "Warning: AL error encountered while trying to get frequency: " << alGetString(myErr));
      std::string errorMessage("Failed to calculate stream data for: ");
      errorMessage.append(getId());
      throw SoundException(errorMessage.c_str());
    }

    alGetBufferi(buffer_, AL_CHANNELS, &channels);
    myErr = alGetError();
    if (myErr)
    {
      GTRACE(2, "Warning: AL error encountered while trying to get channels: " << alGetString(myErr));
      std::string errorMessage("Failed to calculate stream data for: ");
      errorMessage.append(getId());
      throw SoundException(errorMessage.c_str());
    }

    alGetBufferi(buffer_, AL_BITS, &bitsPerSample);
    myErr = alGetError();
    if (myErr)
    {
      GTRACE(2, "Warning: AL error encountered while trying to get bits per sample: " << alGetString(myErr));
      std::string errorMessage("Failed to calculate stream data for: ");
      errorMessage.append(getId());
      throw SoundException(errorMessage.c_str());
    }

    gussound::SoundDuration duration = ((gussound::SoundDuration)bufferSize)/(frequency*channels*(bitsPerSample/8));
    cachedBufferLength_ = duration;
    sampleRate_ = bitsPerSample;

    GTRACE(4, "Calculating stream/buffer information for sound [" << getId() << "]. Channels = ["
      << channels << "] frequency = [" << frequency << "] duration = ["
      << duration << "] samplerate = [" << sampleRate_ << "]");

    return duration;
  }


  gussound::SoundDuration GOpenAlSound::getSeekDuration()
  {
    if (getStreamLength() == 0)
    {
      return 0;
    }

    gussound::SoundDuration rValue = getStreamElapsedTime();
    return rValue;
  }


  gussound::SoundDuration GOpenAlSound::getDuration()
  {
    return getStreamLength();
  }


  bool GOpenAlSound::loadFrom(
      const gussound::AudioDeviceSmartPtr & devicePtr,
      const std::string &fileName,
      bool loadInMem,
      bool onlyOneInstance)
  {
    onlyOneInstance_ = onlyOneInstance;

    GTRACE(2, "Loading from file [" << fileName << "]. only one instance? : " << onlyOneInstance_);

    // Get the extension from the file name
    std::string extension = guslib::stringutil::GetExtensionFromFileName(fileName);
    GTRACE(3, "Extension: " << extension);

    ALenum myErr;

    streamed_ = !loadInMem;
    buffer_ = alutCreateBufferFromFile(fileName.c_str());

    myErr = alGetError();
    if (myErr)
    {
      GTRACE(2, "Error encountered while trying to load AL sound from [" << fileName << "]: " << alGetString(myErr));
    }

    source_ = 0;

    calculateAndRetrieveStreamLength();

    return (buffer_ != 0);
  }


  void GOpenAlSound::pause()
  {
    if (getState() == gussound::GPLS_Playing)
    {
      // store the location of the pause in case a resume is desired.

      elapsedFromStart_ = getStreamElapsedTime();

      alSourcePause(source_);
      state_ = gussound::GPLS_Paused;
      GTRACE(ATLVL, "Paused low level sound." << id_);
    }
    else
    {
      GTRACE(ATLVL, id_ << "; Warning: received command to pause while state is: " << getState());
    }
  }

  bool GOpenAlSound::doPlay3D(double posX, double posY, double posZ, bool repeatContinuously)
  {
    GTRACE(8, "doPlay3D entered for [" << posX << ", " << posY << ", " << posZ << "]");
    source_ = getAvailableSourceForPlay();
    if (0 == source_)
    {
      return false;
    }

    gussound::SoundVolume volumeToUse = getCombinedVolumeModifiers();

    if (posX && posY && posZ)
    {
      ALfloat spos[3] = {0.0, 0.0, 0.0};
      spos[0] = static_cast<ALfloat>(posX);
      spos[1] = static_cast<ALfloat>(posY);
      spos[2] = static_cast<ALfloat>(posZ);
      alSourcefv(source_, AL_POSITION, spos);
      alSourcei(source_, AL_SOURCE_RELATIVE, AL_FALSE);

      alSourcef(source_, AL_REFERENCE_DISTANCE, 5);
      alSourcef(source_, AL_ROLLOFF_FACTOR, 0.5);
      alSourcef(source_, AL_MAX_DISTANCE, 20);
    }

    alSourcei(source_, AL_BUFFER, buffer_);
    alSourcef(source_, AL_GAIN, volumeToUse);
    alSourceRewind(source_);
    alSourcePlay(source_);
    return true;
  }

  void GOpenAlSound::doResume(bool repeatContinuously)
  {
    GTRACE(6, id_ << "; received command to resume");
    if (elapsedFromStart_ > 0)
    {
      gussound::SoundVolume volumeToUse = getCombinedVolumeModifiers();
      alSourcei(source_, AL_BUFFER, buffer_);
      alSourcef(source_, AL_GAIN, volumeToUse);
      alSourcePlay(source_);
    }
  }

  void GOpenAlSound::play2D(gussound::ActionIfPlayingEnum actionIfPlaying, bool repeatContinuously)
  {
    play3D(0, 0, 0, actionIfPlaying, repeatContinuously);
  }


  void GOpenAlSound::play3D(
      double posX, double posY, double posZ, 
      gussound::ActionIfPlayingEnum actionIfPlaying, bool repeatContinuously)
  {
    GTRACE(ATLVL, id_ << "; GOpenAlSound::play3D ... currentstate=" << getState());

    if (onlyOneInstance_)
    {
      if (buffer_)
      {
        switch (getState())
        {
        case gussound::GPLS_Playing:
          switch (actionIfPlaying)
          {
          case gussound::ActionRestart:
            stop();
            if (doPlay3D(posX, posY, posZ, repeatContinuously))
            {
              state_ = gussound::GPLS_Playing;
            }
            break;
          case gussound::ActionParallelPlay:
            if (doPlay3D(posX, posY, posZ, repeatContinuously))
            {
              state_ = gussound::GPLS_Playing;
            }
            break;
          case gussound::ActionEnqueue:
            // not supported at this level
            break;
          case gussound::ActionDoNothing:
          default:
            break;
          }
          return;   // already playing.
        case gussound::GPLS_Stopped:
          {
            if (doPlay3D(posX, posY, posZ, repeatContinuously))
            {
              state_ = gussound::GPLS_Playing;
            }
            return;
          }
        case gussound::GPLS_Paused:
          {
            doResume();
            state_ = gussound::GPLS_Playing;
            return;
          }
        default:
          return;
        }
      }
    }
    else
    {
      // effect
      if (buffer_)
      {
        if (doPlay3D(posX, posY, posZ))
        {
          state_ = gussound::GPLS_Playing;
        }
      }
    }
  }


  ///
  /// Commence the low-level sound play with 3D positioning. Will default to 2D play unless overridden.
  /// This allows you to update the source of the sound after the play command is launched.
  ///
  int GOpenAlSound::play3Dnamed(
      double posX, double posY, double posZ,
      gussound::ActionIfPlayingEnum actionIfPlaying, bool repeatContinuously)
  {
    play3D(posX, posY, posZ, actionIfPlaying, repeatContinuously);

    return source_;
  }


  void GOpenAlSound::setOutputVolume(const gussound::SoundVolume & volume)
  {
    gussound::SoundVolume sentVolume(volume);   // the volume to send to the device.
    if (sentVolume < gussound::min_volume)
    {
      sentVolume = gussound::min_volume;
    }

    if (sentVolume > gussound::max_volume)
    {
      sentVolume = gussound::max_volume;
    }

    if (buffer_ && source_)
    {
      GTRACE(8, "Setting low level output volume to: " << sentVolume);
      alSourcef(source_, AL_GAIN, sentVolume);
    }
  }

  void GOpenAlSound::seekToTime(const gussound::SoundDuration & duration)
  {
    GTRACE(6, "GOpenAlSound::seekToTime () - NOT SUPPORTED");
  }

  void GOpenAlSound::stop()
  {
    GTRACE(6, "GOpenAlSound::stop()");

    alSourceStop(source_);
    source_ = 0;
    state_ = gussound::GPLS_Stopped;
    elapsedFromStart_ = 0;
  }


  // -------------------------------------------- OpenAlSoundRepository ----------------------------------------------

  OpenAlSoundRepository::OpenAlSoundRepository()
    : SoundRepository(),
      maximumConcurrentSounds_(28),
      reservedMusicSources_(3)
  {
    setAudioDevice(new GOpenAlDevice());
  }

  OpenAlSoundRepository::~OpenAlSoundRepository()
  {
    GTRACE(3, "destroying audiere sound repository");

    // Destroy the sources
    for (std::vector <ALuint>::iterator it = soundSources_.begin();
      it != soundSources_.end(); ++it)
    {
      alDeleteSources(1, &(*it));
    }

    for (std::vector <ALuint>::iterator it = musicSources_.begin();
      it != musicSources_.end(); ++it)
    {
      alDeleteSources(1, &(*it));
    }
  }


  gussound::EffectableSound * OpenAlSoundRepository::createSound(const std::string & id, const std::string& fileExtension)
  {
    GTRACE(4, "OpenAL repository: creating sound with id [" << id << "] and extension [" << fileExtension << "]");

    // TODO(Augustin Preda, 2014.04.20): add factory. For the time being, a hard-coded version is in place.
    // TODO(Augustin Preda, 2014.04.20): make lowercase only!!!
    std::string localFileExt(fileExtension);

    if (localFileExt == "ogg")
    {
      return new GOggStreamedSound(this, id);
    }

    return new GOpenAlSound(this, id);
  }


  ///
  /// Internal function used to get an available (non-playing and non-paused) sound source.
  /// @see getAvailableSourceForPlay
  ///
  ALuint OpenAlSoundRepository::getAvailableSoundSourceForPlay()
  {
    ALenum state;
    // Create new source or use available source in the vector?
    for (std::vector <ALuint>::iterator it = soundSources_.begin();
      it != soundSources_.end(); ++it)
    {
      alGetSourcei(*it, AL_SOURCE_STATE, &state);
      if (state != AL_PLAYING && state != AL_PAUSED)
      {
        return *it;
      }
    }

    // Ran through all sources and all are playing. Check whether we can add a new sound source.
    if (soundSources_.size() >= maximumConcurrentSounds_)
    {
      throw SoundException("Maximum amount of sound sources reached!");
    }

    // Create a new source
    GTRACE(4, "Creating new sound source for the repository. Current count: " << (soundSources_.size() + 1));

    ALuint newSource;
    alGenSources(1, &newSource);
    soundSources_.push_back(newSource);

    return newSource;
  }


  ///
  /// Internal function used to get an available (non-playing and non-paused) music source.
  /// @see getAvailableSourceForPlay
  ///
  ALuint OpenAlSoundRepository::getAvailableMusicSourceForPlay()
  {
    ALenum state;
    // Create new source or use available source in the vector?
    for (std::vector <ALuint>::iterator it = musicSources_.begin();
      it != musicSources_.end(); ++it)
    {
      alGetSourcei(*it, AL_SOURCE_STATE, &state);
      if (state != AL_PLAYING)
      {
        return *it;
      }
    }

    // Ran through all sources and all are playing. Check whether we can add a new sound source.
    if (musicSources_.size() >= reservedMusicSources_)
    {
      std::stringstream ss;
      ss << "Maximum amount of music sources reached! ";
      ss << "Reserved sources: " << reservedMusicSources_ << "; current music sources: " << musicSources_.size();
      throw SoundException(ss.str().c_str());
    }

    // Create a new source
    GTRACE(4, "Creating new music source for the repository. Current count: " << (musicSources_.size() + 1));

    ALuint newSource;
    alGenSources(1, &newSource);
    musicSources_.push_back(newSource);
    return newSource;
  }


  ///
  /// Get a source that can be used for a play command. In case no available sources can be found:
  /// - if the maximum amount of sound sources has not been reached yet, a new source is created.
  /// - if the maximum amount of sound sources has been reached, an exception is thrown.
  ///
  ALuint OpenAlSoundRepository::getAvailableSourceForPlay(gussound::SoundCategory categ)
  {
    if (categ == gussound::GSC_Music)
    {
      return getAvailableMusicSourceForPlay();
    }

    return getAvailableSoundSourceForPlay();
  }

  // ----------------------------------------------- OpenAlManagerUtil -------------------------------------------------

  OpenAlManagerUtil::OpenAlManagerUtil()
    : SoundManagerUtil(new OpenAlSoundRepository())
  {
    GTRACE(3, "OpenAL (sound) Manager Utility construction");
  }

  gussound::SoundManagerUtil * OpenAlManagerUtil::createSoundManager()
  {
    return new OpenAlManagerUtil();
  }


  OpenAlManagerUtil::~OpenAlManagerUtil()
  {
    GTRACE(1, "Destroying OpenAlManagerUtil");
  }


  /// Create and return a string containing the version of the sound library
  std::string OpenAlManagerUtil::getVersion() const
  {
    std::stringstream ss;
    ss << "OpenAlManagerUtil-" << GOPENAL_MAJOR_VERSION << "." << GOPENAL_MINOR_VERSION << "." << GOPENAL_PATCH_VERSION;
    ss << " (" << gussound::SoundManagerUtil::getVersion() << ")";
    return ss.str();
  }


  /// Set the listener position
  void OpenAlManagerUtil::setListenerPosition(double posX, double posY, double posZ)
  {
    ALfloat listenerPos[3] = {0.0, 0.0, 0.0};
    listenerPos[0] = static_cast<ALfloat>(posX);
    listenerPos[1] = static_cast<ALfloat>(posY);
    listenerPos[2] = static_cast<ALfloat>(posZ);
    alListenerfv(AL_POSITION, listenerPos);
  }
}
