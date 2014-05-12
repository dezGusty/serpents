#ifndef GAUDIERE_MAIN_HEADER_H
#define GAUDIERE_MAIN_HEADER_H

//   This file is part of the gussound library, licensed under the terms of the MIT License.
//
//   The MIT License
//   Copyright (C) 2010-2012  Augustin Preda (thegusty999@gmail.com)
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

// The API abstraction layer.
#include <gussound.h>

// The implementation is based on audiere.
#include <audiere.h>


//
// Add guslib inclusions... unless we're dealing with the standalone version
//

#ifdef GUSSOUND_STANDALONE

#include <gussoundutil/singleton.hpp>

#else

#include <guslib/common/singleton.hpp>

#endif // GUSSOUND_STANDALONE

// definition for the float type expected to be used by the underlying sound lib.

using namespace gussound;
namespace GAudiere
{
  class GaudiereDevice
    : public gussound::AudioDevice
  {
  protected:
    audiere::AudioDevicePtr audiereDevice_;

  public:
    GaudiereDevice ();
    virtual ~GaudiereDevice ();

    virtual const audiere::AudioDevicePtr & getAudiereDevice () { return audiereDevice_; }
  };

  typedef GSmartPtr <GaudiereDevice> GaudiereDeviceSmartPtr;

  /**
    A low level sound class. This one is tied to the audiere engine.
  */
  class GaudiereSound
    : public EffectableSound
  {
  protected:
    /// The duration in milliseconds that passed since this sound began playing.
    SoundDuration elapsedFromStart_;

    /// This is a smart pointer to the audiere streaming object. Note that it shouldn't be contained in a vector.
    audiere::OutputStreamPtr outstreamptr_;

    /// This is a smart pointer to the audiere streaming object. Note that it shouldn't be contained in a vector.
    audiere::SoundEffectPtr sndEffectPtr_;

    /// Flag that says wether the sound was started with a repeat.
    /// This might be useful for short sounds that can be combined via a repeat.
    bool repeating_;

    /// The sample rate for the loaded sound.
    SampleRate sampleRate_; 

    /// The sound can also store a list of tags
    TagList tags_;

    virtual void doPlay( bool repeatContinuously = false );
    virtual void doResume( bool repeatContinuously = false );

    virtual bool loadFrom (const gussound::AudioDeviceSmartPtr & devicePtr
          , const std::string &fileName, bool loadInMemory, bool onlyOneInstance = true);
  public:
    GaudiereSound (const std::string & id = "");
    virtual ~GaudiereSound();

    virtual SoundVolume getOutputVolume(){ return volume_; }

    virtual SoundDuration getSeekDuration();

    virtual bool isRepeating() const
    {
      return repeating_;
    }

    virtual void pause();

    virtual void play2D (ActionIfPlayingEnum actionIfPlaying = ActionRestart, bool repeatContinuously = false);

    virtual SoundDuration getDuration();

    virtual SampleRate getSampleRate(){ return sampleRate_; }


    virtual const TagList & getTags() const { return tags_;}

    virtual void seekToTime (const SoundDuration & duration);

    virtual void setOutputVolume (const SoundVolume & volume);

    virtual void stop();
  };




  // BIG issues:
  // how to make the automatic fade-out effect when 2 tracks change automatically?
  // sounds elapse time automatically from the repository.
  // what happens if there are 1000 sounds? do all receive the elapse time cmd?
  // should only send to the ones actually playing?
  

  class AudiereSoundRepository
    : public SoundRepository
  {
  public:
    virtual EffectableSound * createSound (const std::string& id = "", const std::string& fileExtension = "");
    AudiereSoundRepository ();
    virtual ~AudiereSoundRepository ();
  };


  class AudiereManagerUtil
    : public SoundManagerUtil
  {
  protected:
    AudiereManagerUtil ();
  public:
    virtual ~AudiereManagerUtil ();
    static SoundManagerUtil * createSoundManager ();
    friend class guslib::Singleton<AudiereManagerUtil>;
  };





  //typedef guslib::Singleton<GAudiere::AudiereManagerUtil> AudiereSoundManager;
}


// ------------- LUA bindings begin -------------------
#ifdef GAUDIERE_LUA_FUNCS
#include <lua.hpp>

  static int l_soundPlay( lua_State *L )
  {
    std::string s( luaL_checkstring(L, 1) );
    GSOUND_PLAY(s);
    return 0;
  }
#endif// GAUDIERE_LUA_FUNCS
// ------------- LUA bindings end -------------------

#endif
