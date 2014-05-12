#pragma once

//   This file is part of the gussound library, licensed under the terms of the MIT License.
//
//   The MIT License
//   Copyright (C) 2010-2013  Augustin Preda (thegusty999@gmail.com)
//
//    Gusty's Serpents is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    Audio utility class.

//
// Includes
//

//
// C++ system includes
//

#include <string>
#include <vector>

//
// Other library's headers
//

// The implementation is based on OpenAL.
#include "al/al.h"
#include "al/alc.h"

// Add support for ogg files.
#include "vorbis/vorbisfile.h"

//
// This library's headers
//

// The API abstraction layer.
#include "gussound.h"

namespace GOpenAl
{
  ///
  /// An exception class.
  ///
  class GUSSOUND_EXPORT_SYMBOL SoundException
    : public std::exception
  {
  private:
    std::string exception_message_;

  public:
    explicit SoundException(const char* text);
    virtual ~SoundException() throw();
    const char* what() const throw();
  };

  class GUSSOUND_EXPORT_SYMBOL GOpenAlDevice
    : public gussound::AudioDevice
  {
  protected:
    ALCcontext* alcContext_;

    ALCdevice * alcDevice_;

  public:
    GOpenAlDevice();
    virtual ~GOpenAlDevice();

    virtual const ALCcontext* getALCcontext()
    {
      return alcContext_;
    }

    virtual const ALCdevice* getALCdevice()
    {
      return alcDevice_;
    }
  };

  typedef GUSSOUND_EXPORT_SYMBOL GSmartPtr <GOpenAlDevice> GOpenAlDeviceSmartPtr;

  // fwd class defs.
  class GOpenAlSound;
  class OpenAlSoundRepository;


  /**
    A low level sound class. This one is tied to the audiere engine.
  */
  class GUSSOUND_EXPORT_SYMBOL GOpenAlSound
    : public gussound::EffectableSound
  {
  protected:
    /// The duration in milliseconds that passed since this sound began playing.
    gussound::SoundDuration elapsedFromStart_;

    /// The source of the sound.
    ALuint source_;

    // TODO(Augustin Preda, 2014.04.15): investigate whether this is needed or not.
    /// Additional sources to be used for the sound.
    // std::vector <ALuint> secondarySources_;

    /// The buffered content of the sound.
    ALuint buffer_;

    /// Flag that says wether the sound was started with a repeat.
    /// This might be useful for short sounds that can be combined via a repeat.
    bool repeating_;

    /// The sample rate for the loaded sound.
    gussound::SampleRate sampleRate_;

    /// The sound can also store a list of tags
    gussound::TagList tags_;

    /// Will store the length of the buffer.
    gussound::SoundDuration cachedBufferLength_;

    /// A pointer to the sound repository is stored here.
    /// Do not change this to a smart pointer, as the repository is likely to store
    /// the sounds as smart pointers and that would produce issues.
    OpenAlSoundRepository* repositoryPtr_;

    virtual ALuint getAvailableSourceForPlay();

    virtual bool doPlay3D(double posX = 0, double posY = 0, double posZ = 0, bool repeatContinuously = false);
    virtual void doResume(bool repeatContinuously = false);

    virtual bool loadFrom(
        const gussound::AudioDeviceSmartPtr & devicePtr,
        const std::string &fileName,
        bool loadInMemory,
        bool onlyOneInstance = true);

    /// Utility function to retrieve the time that is currently elapsed from the stream.
    virtual gussound::SoundDuration getStreamElapsedTime();

    /// Utility function to retrieve the length of the stream.
    virtual gussound::SoundDuration getStreamLength();

    /// Utility function to calculate and retrieve the length of the stream.
    virtual gussound::SoundDuration calculateAndRetrieveStreamLength();

  public:
    GOpenAlSound(OpenAlSoundRepository* repoPtr, const std::string & id = "");
    virtual ~GOpenAlSound();

    virtual gussound::SoundVolume getOutputVolume()
    {
      return volume_;
    }

    virtual gussound::SoundDuration getSeekDuration();

    virtual bool isRepeating() const
    {
      return repeating_;
    }

    virtual void pause();

    /// Commence the low-level sound play. Note that certain actions may be performed if the sound is already
    /// playing, but not all possible actions are supported at this low level.
    virtual void play2D(
        gussound::ActionIfPlayingEnum actionIfPlaying = gussound::ActionParallelPlay,
        bool repeatContinuously = false);

    /// Commence the low-level sound play with 3D positioning. Note that certain actions
    /// may be performed if the sound is already playing, but not all possible actions are
    /// supported at this low level. Will default to 2D play unless overridden.
    virtual void play3D(
        double posX,
        double posY,
        double posZ,
        gussound::ActionIfPlayingEnum actionIfPlaying = gussound::ActionRestart,
        bool repeatContinuously = false);

    /// Commence the low-level sound play with 3D positioning. Will default to 2D play unless overridden.
    /// This allows you to update the source of the sound after the play command is launched.
    virtual int play3Dnamed(
        double posX,
        double posY,
        double posZ,
        gussound::ActionIfPlayingEnum actionIfPlaying = gussound::ActionRestart,
        bool repeatContinuously = false);

    virtual gussound::SoundDuration getDuration();

    virtual gussound::SampleRate getSampleRate()
    {
      return sampleRate_;
    }

    virtual const gussound::TagList & getTags() const
    {
      return tags_;
    }

    virtual void seekToTime(const gussound::SoundDuration & duration);

    virtual void setOutputVolume(const gussound::SoundVolume & volume);

    virtual void stop();
  };


  ///
  /// An OpenAL specific sound repository.
  ///
  class GUSSOUND_EXPORT_SYMBOL OpenAlSoundRepository
    : public gussound::SoundRepository
  {
  protected:
    /// The sound sources available to the engine
    std::vector <ALuint> soundSources_;

    std::vector <ALuint> musicSources_;

    /// The maximum number of sources allowed to play at one time.
    /// Note: if you exceed the hardware/software limit, you may get no error, but still be unable to play any sounds
    unsigned int maximumConcurrentSounds_;

    /// The amount of sources reserved for music.
    unsigned int reservedMusicSources_;

    /// Internal function used to get an available (non-playing and non-paused) sound source.
    /// @see getAvailableSourceForPlay
    virtual ALuint getAvailableSoundSourceForPlay();

    /// Internal function used to get an available (non-playing and non-paused) music source.
    /// @see getAvailableSourceForPlay
    virtual ALuint getAvailableMusicSourceForPlay();

  public:
    /// Constructor.
    OpenAlSoundRepository();

    /// Destructor.
    virtual ~OpenAlSoundRepository();

    /// Sound creation function. It is designed to be placed as a function pointer in an abstract factory.
    virtual gussound::EffectableSound* createSound(
        const std::string& id = "",
        const std::string& fileExtension = "");

    /// Get a source that can be used for a play command. In case no available sources can be found:
    /// - if the maximum amount of sound sources has not been reached yet, a new source is created.
    /// - if the maximum amount of sound sources has been reached, an exception is thrown.
    virtual ALuint getAvailableSourceForPlay(gussound::SoundCategory categ = gussound::GSC_Effect);
  };


  ///
  /// A custom implementation (OpenAL specific) of the sound manager.
  ///
  class GUSSOUND_EXPORT_SYMBOL OpenAlManagerUtil
    : public gussound::SoundManagerUtil
  {
  protected:
    /// Constructor. A call from a singleton to the function createSoundManager is preferred.
    OpenAlManagerUtil();

  public:
    /// Destructor.
    virtual ~OpenAlManagerUtil();

    /// Object instantiation function.
    static gussound::SoundManagerUtil * createSoundManager();

    /// Create and return a string containing the version of the sound library
    virtual std::string getVersion() const;

    /// Set the listener position
    virtual void setListenerPosition(double posX, double posY, double posZ);

    /// Declare the singleton to be a friend for easy access..
    friend class guslib::Singleton <OpenAlManagerUtil>;
  };
}


// ------------- LUA bindings begin -------------------
#ifdef GOPENAL_LUA_FUNCS
#include "lua.hpp"

  static int l_soundPlay(lua_State *L)
  {
    std::string s(luaL_checkstring(L, 1));
    GSOUND_PLAY(s);
    return 0;
  }
#endif  // GAUDIERE_LUA_FUNCS
// ------------- LUA bindings end -------------------
