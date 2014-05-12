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
// Own header: the audiere bound implementation.
//
#include "gaudiere.h"

//
// C++ system headers
//
#include <string>

//
// Other libraries' headers. This library's headers.
//

// The abstract sound interface
#include "gussound.h"

// Trace support.
#ifdef GUSSOUND_STANDALONE
// Tracing util
#include "gussoundutil/trace.h"
// A generic exception.
#include "gussoundutil/simpleexception.h"
#else
// Tracing util
#include "guslib/trace/trace.h"
// A generic exception.
#include "guslib/common/simpleexception.h"
#endif


#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
// Link to the audiere.lib file.
#pragma comment(lib, "audiere.lib")
#else
#pragma message("You should link to the audiere runtime lib.")
#endif  // GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS

#define ATLVL 4

namespace GAudiere
{
  GaudiereDevice::GaudiereDevice()
    : gussound::AudioDevice()
  {
    audiereDevice_ = audiere::OpenDevice();
    if (audiereDevice_ == NULL)
    {
      throw guslib::SimpleException("Unable to open audiere device!");
    }
    GTRACE(2, "Using audiere, version: " << audiere::GetVersion());
  }


  GaudiereDevice::~GaudiereDevice()
  {
    GTRACE(2, "Destroying audiere device, clearing callbacks");
    if (audiereDevice_)
    {
      audiereDevice_->clearCallbacks();
    }
    GTRACE(2, "Destroyed audiere device");
  }


  // ------------------------------------------ GaudiereSound -----------------------------------------


  GaudiereSound::GaudiereSound(const std::string & id)
    : EffectableSound(id),
      sampleRate_(0),
      repeating_(false),
      elapsedFromStart_(0),
      outstreamptr_(0),
      sndEffectPtr_(0)
  {
  }


  GaudiereSound::~GaudiereSound()
  {
    if (getState() != GPLS_Stopped)
    {
      stop();
    }
    GTRACE(8, "Low lvl sound destruction");
  }


  SoundDuration GaudiereSound::getSeekDuration()
  {
    // No support for sound effects (audiere); only streams

    if (outstreamptr_ && sampleRate_)
    {
      if (outstreamptr_->getLength() == 0)
      {
        return 0;
      }
      SoundDuration rValue = getDuration() * outstreamptr_->getPosition() / outstreamptr_->getLength();
      return rValue;
    }
    return 0;
  }


  SoundDuration GaudiereSound::getDuration()
  {
    // No support for sound effects (audiere); only streams

    if (sampleRate_)
    {
      if (outstreamptr_)
      {
        SoundDuration rValue = 1000 * (SoundDuration)outstreamptr_->getLength() / (SoundDuration)sampleRate_;
        return rValue;
      }
    }
    return 0;
  }


  bool GaudiereSound::loadFrom(
      const gussound::AudioDeviceSmartPtr & devicePtr,
      const std::string &fileName,
      bool loadInMem,
      bool onlyOneInstance)
  {
    onlyOneInstance_ = onlyOneInstance;

    audiere::SampleSource *src = audiere::OpenSampleSource(fileName.c_str());
    if (src)
    {
      GTRACE(3, "opened sample: " << fileName);
      tags_.clear();

      for (int i = 0; i < src->getTagCount(); ++i)
      {
        tags_.insert(std::make_pair(std::string(src->getTagKey(i)), std::string(src->getTagValue(i))));
      }

      audiere::SampleSourcePtr srcptr(src);
      audiere::SampleFormat sampleFormat;
      int nChanCount;  // unused beyond the following call.
      src->getFormat(nChanCount, sampleRate_, sampleFormat);

      // the audiere flag is "streamable", the flag in this class (loadInMem) is the exact opposite.
      streamed_ = !loadInMem;

      GaudiereDeviceSmartPtr myDeviceCast = static_cast <const GaudiereDeviceSmartPtr> (devicePtr);

      if (onlyOneInstance)
      {
        outstreamptr_ = audiere::OpenSound(myDeviceCast->getAudiereDevice(), srcptr, !loadInMem);
      }
      else
      {
        sndEffectPtr_ = audiere::OpenSoundEffect(myDeviceCast->getAudiereDevice(), srcptr, audiere::MULTIPLE);
      }
    }
    else
    {
      GTRACE(2, "Unable to open sound for : " << fileName);
    }

    if (false == onlyOneInstance)
    {
      return (sndEffectPtr_ != NULL);
    }
    return (outstreamptr_ != NULL);
  }


  void GaudiereSound::pause()
  {
    // No support for sound effects (audiere); only streams

    if (getState() == GPLS_Playing)
    {
      // store the location of the pause in case a resume is desired.
      elapsedFromStart_ = getSeekDuration();
      if (outstreamptr_)
      {
        outstreamptr_->stop();
        state_ = GPLS_Paused;
        GTRACE(ATLVL, "Paused low level sound." << id_);
      }
    }
    else
    {
      GTRACE(ATLVL, id_ << "; Warning: received command to pause while state is: " << getState());
    }
  }

  void GaudiereSound::doPlay(bool repeatContinuously)
  {
    SoundVolume volumeToUse = getCombinedVolumeModifiers();
    GTRACE(6, id_ << "; received command to play from stop, vol=" << volumeToUse
        << " only one instance? " << onlyOneInstance_);

    if (onlyOneInstance_)
    {
      repeating_ = repeatContinuously;
      outstreamptr_->setVolume(volumeToUse);
      outstreamptr_->setRepeat(repeating_);
      outstreamptr_->reset();
      outstreamptr_->play();
    }
    else
    {
      sndEffectPtr_->setVolume(volumeToUse);
      sndEffectPtr_->play();
    }
  }

  void GaudiereSound::doResume(bool repeatContinuously)
  {
    // No support for sound effects (audiere); only streams

    GTRACE(6, id_ << "; received command to resume");
    if (elapsedFromStart_ > 0)
    {
      repeating_ = repeatContinuously;
      SoundVolume volumeToUse = getCombinedVolumeModifiers();
      outstreamptr_->setVolume(volumeToUse);
      outstreamptr_->setRepeat(repeating_);
      seekToTime(elapsedFromStart_);
      outstreamptr_->play();
    }
  }

  void GaudiereSound::play2D(ActionIfPlayingEnum actionIfPlaying, bool repeatContinuously)
  {
    GTRACE(ATLVL, id_ << "; low level sound play... currentstate=" << getState());

    if (onlyOneInstance_)
    {
      if (outstreamptr_)
      {
        switch (getState())
        {
        case GPLS_Playing:
          if (outstreamptr_->isPlaying())
          {
            switch (actionIfPlaying)
            {
            case ActionRestart:
              doPlay(repeatContinuously);
              state_ = GPLS_Playing;
              break;
            case ActionParallelPlay:
              // Not supported at this level
              break;
            case ActionEnqueue:
              // Not supported at this level
              break;
            case ActionDoNothing:
            default:
              break;
            }
          }
          else
          {
            GTRACE(2, "WARNING: Sound internal state corruption (obj state=playing; stream not playing)!");
            // Normally this shouldn't happen, we have some internal state problems.
            doPlay(repeatContinuously);
            state_ = GPLS_Playing;
          }
          return;  // Already playing.
        case GPLS_Stopped:
          {
            doPlay(repeatContinuously);
            state_ = GPLS_Playing;
            return;
          }
        case GPLS_Paused:
          {
            doResume();
            state_ = GPLS_Playing;
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
      if (sndEffectPtr_)
      {
        doPlay();
      }
    }
  }


  void GaudiereSound::setOutputVolume(const SoundVolume & volume)
  {
    SoundVolume sentVolume(volume);  // the volume to send to the device.
    if (sentVolume < gussound::min_volume)
    {
      sentVolume = gussound::min_volume;
    }

    if (sentVolume > gussound::max_volume)
    {
      sentVolume = gussound::max_volume;
    }

    if (onlyOneInstance_)
    {
      if (outstreamptr_)
      {
        GTRACE(8, "Setting low level output volume to: " << sentVolume);
        outstreamptr_->setVolume(sentVolume);
      }
    }
    else
    {
      if (sndEffectPtr_)
      {
        sndEffectPtr_->setVolume(sentVolume);
      }
    }
  }

  void GaudiereSound::seekToTime(const SoundDuration & duration)
  {
    // No support for sound effects (audiere); only streams

    if (outstreamptr_)
    {
      if (outstreamptr_->isSeekable() && sampleRate_ != 0)
      {
        if (getDuration() == 0)
        {
          return;
        }

        SoundDuration ftemp = outstreamptr_->getLength() * duration / getDuration();
        int nPosition = static_cast<int>(ftemp);
        GTRACE(6, "inner calc: seek to pos=" << nPosition << ", from len=" << outstreamptr_->getLength());
        outstreamptr_->setPosition(nPosition);
      }
    }
  }

  void GaudiereSound::stop()
  {
    GTRACE(6, "GaudiereSound::stop()");

    if (onlyOneInstance_)
    {
      if (outstreamptr_)
      {
        outstreamptr_->stop();
        outstreamptr_->reset();
        elapsedFromStart_ = 0;
        state_ = GPLS_Stopped;
      }
    }
    else
    {
      if (sndEffectPtr_)
      {
        sndEffectPtr_->stop();
        state_ = GPLS_Stopped;
      }
    }
  }


  // -------------------------------------------- AudiereSoundRepository ----------------------------------------------

  AudiereSoundRepository::AudiereSoundRepository()
    : SoundRepository()
  {
    setAudioDevice(new GaudiereDevice());
  }

  AudiereSoundRepository::~AudiereSoundRepository()
  {
    GTRACE(3, "destroying audiere sound repository");
  }


  EffectableSound * AudiereSoundRepository::createSound(const std::string & id, const std::string& fileExtension)
  {
    return new GaudiereSound(id);
  }

  AudiereManagerUtil::AudiereManagerUtil()
    : SoundManagerUtil(new AudiereSoundRepository())
  {
    GTRACE(3, "Audiere Manager construction");
  }

  SoundManagerUtil * AudiereManagerUtil::createSoundManager()
  {
    return new AudiereManagerUtil();
  }


  AudiereManagerUtil::~AudiereManagerUtil()
  {
    GTRACE(1, "Destroying AudiereManagerUtil");
  }
}
