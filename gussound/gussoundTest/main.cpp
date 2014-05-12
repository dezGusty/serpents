
// Tracing util
#ifndef GUSSOUND_STANDALONE
#include <guslib/trace/trace.h>
#include <guslib/system/apprestart.h>
#else
// Tracing util
#include <gussoundutil/trace.h>
#endif

#include <gussound.h>
#include <gaudiere.h>

#include <GOpenAL.h>

 #ifndef GUSSOUND_STANDALONE
#include "soundmenu.h"
 #endif

void main ()
{
#ifndef GUSSOUND_STANDALONE
  guslib::AppRestarter::getPtr ()->setRestartParameter ("--restart");

  if( guslib::AppRestarter::getPtr()->restartProcessStarted() )
  {
    guslib::AppRestarter::getPtr()->waitForPreviousProcessToFinish();
  }

  GSTARTTRACING ("zsndtrace.txt", 10);
#endif
  try
  {
#ifndef GUSSOUND_STANDALONE
    SoundManagerFactory::getPtr ()->Register ("audiere", GAudiere::AudiereManagerUtil::createSoundManager);
#endif
    SoundManagerFactory::getPtr ()->Register ("openal", GOpenAl::OpenAlManagerUtil::createSoundManager);

    SoundManager::setPtr (SoundManagerFactory::getPtr ()->CreateObject ("openal"));
    //SoundManager::setPtr (SoundManagerFactory::getPtr ()->CreateObject ("audiere"));

#ifndef GUSSOUND_STANDALONE
    GTRACE (3, "Starting Sound Manager version: " << SoundManager::getPtr ()->getVersion ());
#endif
    SoundManager::getPtr ()->getMusicPlayer ()->setFadeDuration (2500);
    SoundManager::getPtr ()->getMusicPlayer ()->setRepeat (true);

    // make sure the first call to elapse time won't return many seconds. 
    SoundManager::getPtr ()->elapseTime ();

    // Start adding sounds to the sound repository.
    SoundManager::getPtr ()->getRepository ()->addSound ("gus" // name
                            , "../Game/data/sounds/Mulchsound.wav" // path
                            , true // load entire sound into memory? don't do it for songs! only for small sounds.
                            , GSC_Effect); // a category for the sound; you can change the volume for all sounds in a category.

    // Play this sound immediately.
    SoundManager::getPtr ()->getRepository ()->getSound ("gus")->play2D ();

    SoundManager::getPtr ()->getRepository ()->setVolumeForCategory (GSC_Music, (SoundVolume)0.75);
    SoundManager::getPtr ()->getRepository ()->setVolumeForCategory (GSC_Voice, (SoundVolume)0.8);

    SoundManager::getPtr ()->addSoundGroup("empty group");
    try
    {
      std::string sTemp = SoundManager::getPtr ()->getSoundGroup ("empty group")->getRandomSound ();
      SoundManager::getPtr ()->getRepository ()->getSound (sTemp)->play2D ();
    }
    catch (std::exception& e)
    {
      GTRACE (2, "Caught exception: " << e.what ());
    }


    // Place some sounds together in the same group. This could be used to offer a varied response to the same action.
    // E.g. in a strategy game, select a swordsman. Sometimes he will reply with "hey", sometimes with "hello", sometimes with "hmm?"
    SoundManager::getPtr ()->addSoundGroup("swordsman selected");
    SoundManager::getPtr ()->getRepository ()->addSound ("srd_select1", "../Game/data/sounds/Mulchsound.wav", true, GSC_Voice, false);
    SoundManager::getPtr ()->getRepository ()->addSound ("srd_select2", "../Game/data/sounds/MenuSelectionClicked.wav", true, GSC_Voice, false);
    SoundManager::getPtr ()->getRepository ()->addSound ("srd_select3", "../Game/data/sounds/Huiitysound.wav", true, GSC_Voice, false);

    SoundManager::getPtr ()->getSoundGroup("swordsman selected")->addSound("srd_select1", 20); // % chance to play this: (20 * 100) / (20 + 15 + 10) = 44%
    SoundManager::getPtr ()->getSoundGroup("swordsman selected")->addSound("srd_select2", 15); // % chance to play this: (15 * 100) / (20 + 15 + 10) = 33%
    SoundManager::getPtr ()->getSoundGroup("swordsman selected")->addSound("srd_select3", 10); // % chance to play this. (10 * 100) / (20 + 15 + 10) = 22%

    SoundManager::getPtr ()->addSoundGroup("single sound selection");
    SoundManager::getPtr ()->getRepository ()->addSound ("srd_select1_single", "../Game/data/sounds/Mulchsound.wav", true, GSC_Voice, true);
    SoundManager::getPtr ()->getSoundGroup("single sound selection")->addSound("srd_select1_single", 20); // % chance to play this: (20 * 100) / (20 + 15 + 10) = 44%

    SoundManager::getPtr ()->getMusicPlayer ()->registerPlaylist ("menu");
    SoundManager::getPtr ()->getMusicPlayer ()->registerPlaylist ("game");
    
    SoundManager::getPtr ()->addPlaylistTrack ("menu", "../Game/data/music/menu/General Fuzz - Cliff Notes.ogg");

    SoundManager::getPtr ()->addPlaylistTrack ("game", "../Game/data/music/game/General Fuzz - Comfort Zone.ogg");
    SoundManager::getPtr ()->addPlaylistTrack ("game", "../Game/data/music/game/General Fuzz - Starry.ogg");
    SoundManager::getPtr ()->addPlaylistTrack ("game", "../Game/data/music/game/General Fuzz - Warm Steel.ogg");

    //SoundManager::getPtr ()->addPlaylistTrack ("game", "../Game/data/music/the_prophecy.ogg");
    //SoundManager::getPtr ()->addPlaylistTrack ("game", "../Game/data/music/Dark_Exploration_mini.ogg");
    //SoundManager::getPtr ()->addPlaylistTrack ("game", "../Game/data/music/tread_lightly_mini.ogg");
    SoundManager::getPtr ()->getMusicPlayer ()->setRepeat (true);
    //SoundManager::getPtr ()->getMusicPlayer ()->setShuffle (true);

    ((GOpenAl::OpenAlManagerUtil*)SoundManager::getPtr ())->setListenerPosition (0, 0, 0);
    SoundManager::getPtr()->getMusicPlayer()->switchToPlaylist("game");

#ifndef GUSSOUND_STANDALONE
    displayInstructions ();

    menutest ();
#endif
    SoundManager::destroy ();
  }
  catch (std::exception & e)
  {
#ifndef GUSSOUND_STANDALONE
    GTRACE (1, "ex: " << e.what ());
#endif
  }

#ifndef GUSSOUND_STANDALONE
  GTRACE (7, "ending");
  GSTOPTRACING ();

  guslib::AppRestarter::getPtr()->finishRestart();
#endif
}