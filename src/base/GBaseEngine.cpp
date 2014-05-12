//    This file is part of Gusty's Serpents
//    Copyright (C) 2009  Augustin Preda (thegusty999@gmail.com)
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
//    Basic implementation of an engine.

// Header
#include "GBaseEngine.h"
#include <guslib\trace\Trace.h>
#if 0
#include "GUtils.h"

#include "GBaseFrameListener.h"
#include "GBaseMainApp.h"
#include "GBaseEngineDefinitions.h"
#include "GBaseSetting.h"
//Ogre includes
#include <ogreoverlay.h>
#include <OgrePanelOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>

#include <gaudiere.h>
#include "SerpConstants.h"
#endif

//using namespace GAudiere;
//using namespace Ogre;
//using namespace GUtils;

#ifndef LOAD_RESOURCES_IN_LOGIC_THREAD
#define LOAD_RESOURCES_IN_LOGIC_THREAD 0
#endif

namespace GBase
{

	GBaseEngine::GBaseEngine( Ogre::SceneManager * aManager, Ogre::Root * rootElement )
		//: root( rootElement )
		//, sceneManager( aManager )
		//, exitCmdReceived( false )
		//, bBootResourcesWereLoaded( false )
		//, frameListener( NULL )
		//, windowIsActive_( true )
		//, loadingRes_ (false)
		//, rendering_ (false)

	{
		GTRACE(3, "GBaseEngine constructor entered.");
#if 0
		loaderOverlay = NULL;
		overlay = NULL;

		// Create the state manager

		GTRACE(3, "GBaseEngine is now listening to state changes.");
		GBaseStateManager::get().setListener(this);
#endif
	}



	GBaseEngine::~GBaseEngine()
	{
		GTRACE(3, "GBaseEngine destructor entered.");
#if 0
		if( frameListener )
		{
			GTRACE(4, "GBaseEngine releasing self from framelistener");
			frameListener->releaseEngine();
			// The frame listener is not owned by the engine
			GTRACE(4, "GBaseEngine released connection to frameListener");
			releaseFrameListener();
		}
#endif
		GTRACE(3, "GBaseEngine destructor exiting.");
	}


#if 0
	bool GBaseEngine::loadBootResources()
	{
		if( bBootResourcesWereLoaded )
		{
			GTRACE(1, "Warning: received command to load boot resources again! (Ignoring)");
			return false;
		}
		ResourceGroupManager::getSingleton().initialiseResourceGroup("Bootstrap");
		ResourceGroupManager::getSingleton().loadResourceGroup("Bootstrap");
		Ogre::OverlayManager * overlayManager = OverlayManager::getSingletonPtr();
		if( ! overlayManager )
		{
			GTRACE(3, "ERROR! OverlayManager is NULL!");
			return false;
		}
		
		loaderOverlay = overlayManager->getByName("GusApp/Loader4_3");

		bool result = (loaderOverlay != NULL);
		bBootResourcesWereLoaded = result;
		return result;
	}



	void GBaseEngine::loadResources()
	{
#if LOAD_RESOURCES_IN_LOGIC_THREAD
#else
		for (int i = 1; i < 5000 && rendering_; i += 25)
		{
			guslib::sleep (25);
		}
#endif
		loadingRes_ = true;
		std::vector<std::string> resList = GBaseAppSettings::getPtr ()->setting.res.resources;

		try
		{
			GTRACE(3, "..:: Adding General resource location list ::..");
			GBaseMainApp::addResourceLocationList( resList, "General" );
			GTRACE(3, "..:: Creating General resource folder list ::..");
			ResourceGroupManager::getSingleton().initialiseResourceGroup("General");
			ResourceGroupManager::getSingleton().loadResourceGroup("General");
			GTRACE(3, "Initialized and loaded General res group");
		}
		catch( Exception & e )
		{
			GTRACE (1, "EXCEPTION: " << e.what ());
		}
		
		loadingRes_ = false;
		// TODO: add a listener and a progress bar for the resources.
		// Repeat for the level load as well, and make sure each mesh is displayed once (behind a fully opaque 2D Gui)
		//ResourceGroupListener * myListener;
	}



	void GBaseEngine::loadOverlays()
	{
		GTRACE(4, "GBaseEngine loading Overlays...");
		GTRACE(5, "GBaseEngine: loadOverlays getting lock (loadAndRenderMutex)");
		guslib::GScopedLock myLock( loadAndRenderMutex );
		GTRACE(5, "GBaseEngine: loadOverlays got mutex (loadAndRenderMutex)");
		overlay = OverlayManager::getSingletonPtr()->getByName(c_mainMenuOverlay);
		//Post init - hide templates and copy them.

		//overlay->show();
		//std::string gsCopyrightNotice = std::string( GUS_COPYRIGHT_NOTICE ) + std::string( GUS_APP_VERSION );
		//OverlayManager::getSingleton().getOverlayElement("GS/TAMenuCredits")->
		//	setCaption(gsCopyrightNotice);
		//overlay->hide();
	}



	void GBaseEngine::loadSounds()
	{
#ifdef GUS_ENABLE_SOUND
		GTRACE(3, "Loading sounds...");
		// This function is supposed to handle all sound loading.

		GSOUND_ADD_SOUND(GUS_SOUND_MENU_SELECTION_CHANGED, GUS_SOUND_MENU_SELECTION_CHANGED, GSC_Effect, true);
		GTRACE(3, "Loaded "<<GUS_SOUND_MENU_SELECTION_CHANGED);

		GSOUND_ADD_SOUND(GUS_SOUND_MENU_SELECTION_CLICKED, GUS_SOUND_MENU_SELECTION_CLICKED, GSC_Effect, true);
		GTRACE(3, "Loaded "<<GUS_SOUND_MENU_SELECTION_CLICKED);

		GSOUND_ADD_SOUND(GUS_SOUND_MENU_OPTION_CHANGED, GUS_SOUND_MENU_OPTION_CHANGED, GSC_Effect, true);
		GTRACE(3, "Loaded "<<GUS_SOUND_MENU_OPTION_CHANGED);

		//TODO: consider moving these to LUA!!!
		GSOUND_ADD_SOUND(GUS_SOUND_BONUS_PICKED_UP, GUS_SOUND_BONUS_PICKED_UP, GSC_Effect, true);
		GTRACE(3, "Loaded "<<GUS_SOUND_BONUS_PICKED_UP);

		GSOUND_ADD_SOUND(GUS_SOUND_BONUS_POPPED_UP, GUS_SOUND_BONUS_POPPED_UP, GSC_Effect, true);
		GTRACE(3, "Loaded "<<GUS_SOUND_BONUS_POPPED_UP);

		GSOUND_ADD_SOUND(GUS_SOUND_BONUS_CLICK, GUS_SOUND_BONUS_CLICK, GSC_Effect, true);
		GTRACE(3, "Loaded "<<GUS_SOUND_BONUS_CLICK);
		
		// ---------------- beginning of playlist setup ------------
		
		// register the 2 used playlists;
		GSOUND_ADD_PLAYLIST(GUS_MENU_PLAYLIST);
		GSOUND_ADD_PLAYLIST(GUS_GAME_PLAYLIST);

		GTRACE(4, "Adding tracks to playlists...");
		std::vector<std::string> playlist = GBaseAppSettings::getPtr ()->music.game.playlist;
		for( std::vector<std::string>::iterator it = playlist.begin(); it!= playlist.end(); ++it )
		{
			GSOUND_ADD_TRACK_TO_PLAYLIST(GUS_GAME_PLAYLIST, *it);
			GTRACE(4, "Loaded :"<< *it);
		}

		playlist = GBaseAppSettings::getPtr ()->music.menu.playlist;
		for( std::vector<std::string>::iterator it = playlist.begin(); it!= playlist.end(); ++it )
		{
			GSOUND_ADD_TRACK_TO_PLAYLIST(GUS_MENU_PLAYLIST, *it);
			GTRACE(4, "Loaded :"<< *it);
		}

		GTRACE(2, "Setting music player to shuffle and repeat...");
		GSOUND_SET_MUSIC_REPEAT(true);
		GSOUND_SET_MUSIC_SHUFFLE(true);

		// ---------------- end of playlist setup ------------

		float fMasterVolume = 1.0f;
		GTRACE(2, "setting master audio volume to "<<fMasterVolume);
		GSOUND_SET_VOLUME_FOR_CATEGORY( GSC_Master,
			(gussound::SoundVolume) GBaseAppSettings::getPtr ()->setting.sound.masterVolume);
		GSOUND_SET_VOLUME_FOR_CATEGORY( GSC_Music,
			(gussound::SoundVolume) GBaseAppSettings::getPtr ()->setting.sound.musicVolume);
		GSOUND_SET_VOLUME_FOR_CATEGORY( GSC_Voice,
			(gussound::SoundVolume) GBaseAppSettings::getPtr ()->setting.sound.voiceVolume);
		GSOUND_SET_VOLUME_FOR_CATEGORY( GSC_Effect,
			(gussound::SoundVolume) GBaseAppSettings::getPtr ()->setting.sound.fxVolume);
#endif //GUS_ENABLE_SOUND
	}



	bool GBaseEngine::logicThreadLoop()
	{
		GTRACE(5, "GBaseEngine: logicThreadLoop.");

		// currently, this will hold the app. logic.
		// it's projected to be executed from a separated thread.

		// run thru the events.
		// maybe some events will allow other to be treated as well, but some will require that after 
		// they are treated, the function exits, a frame is rendered and a followup event is processed.

		bool bResult = GBaseStateManager::getPtr()->handleLogicThreadLoop();
		
		GSOUND_ELAPSE_TIME();
		return bResult;
	}


#endif
	// This will be called from renderCompleteScene (via fireframe events)
	bool GBaseEngine::renderScene()
	{
		if (loadingRes_)
		{
			GTRACE(5, "GBaseEngine: render scene skipping because loading res.");
			return true;
		}

		rendering_ = true;
		GTRACE(5, "GBaseEngine: render scene entered");
		//GBaseStateManager::getPtr()->renderScene();
		rendering_ = false;
		return true;
	}


#if 0
	// This will be called from renderCompleteScene (via fireframe events)
	bool GBaseEngine::renderOverlay()
	{
		if (loadingRes_)
		{
			GTRACE(5, "GBaseEngine: render Overlay skipping because loading res.");
			return true;
		}
		rendering_ = true;
		GTRACE(5, "GBaseEngine: render overlay entered");
		if( frameListener == NULL )
		{
			rendering_ = false;
			return false;
		}

		GTRACE(5, "GBaseEngine: calling state gui render.");
		GBaseStateManager::getPtr()->renderGUI();

		rendering_ = false;
		return true;
	}



	RenderWindow *GBaseEngine::getRenderWindow()
	{ 
		if( ! frameListener )
		{
			return NULL;
		}
		return frameListener->getRenderWindow();
	}



	SceneManager *GBaseEngine::getSceneManager()
	{
		return sceneManager;
	}



	bool GBaseEngine::renderCompleteScene()
	{
		// This is basically a copy of the standard OGRE root function "renderOneFrame".
		// What it does in addition to the aforementioned function is to add a mutex lock for 
		// loading and rendering operations.
		// The function should be called from the rendering thread!

		if( ! frameListener )
		{
			return false;
		}

		guslib::Timer myTimer;
		myTimer.reset();

		GTRACE(5, "Engine: render complete scene() entered.");
		GTRACE(6, "GBaseEngine::renderCompleteScene() getting mutex(loadAndRenderMutex)");
		guslib::GScopedLock myLock( loadAndRenderMutex );
		GTRACE(6, "GBaseEngine::renderCompleteScene() got mutex(loadAndRenderMutex)");

		if( isExiting() )
			return false;

		// Trigger the FrameListener's "frameStarted" function to render the 3D scene.
		if(!root->_fireFrameStarted())
		{
			GTRACE(1, "GBaseEngine::renderCompleteScene received error from _fireFrameStarted");
            return false;
		}

		if( isExiting() )
			return false;

		try
		{
			if( ! root->_updateAllRenderTargets() )
			{
				GTRACE(1, "GBaseEngine::renderCompleteScene received error from _updateAllRenderTargets");
				return false;
			}
		}
		catch( Exception & e )
		{
			GTRACE(1, "Exception caught! GBaseEngine::renderCompleteScene(): "<<e.getFullDescription());
		}

		if( isExiting() )
			return false;

		// Trigger the FrameListener's "frameEnded" function.
        bool returnValue = root->_fireFrameEnded();

		GTRACE(5, "Engine: render complete scene() exiting. Render took: "<< myTimer.elapsed()<< " time units.");
		GTRACE(6, "GBaseEngine::renderCompleteScene() releasing mutex(loadAndRenderMutex)");
		return returnValue;
	}



	void GBaseEngine::setFrameListener( const GBaseFrameListener * gfListener )
	{ 
		if( frameListener )
		{
			// there is a listener already; replacement required
			root->removeFrameListener( frameListener );
		}
		frameListener = (GBaseFrameListener *)gfListener; 
		root->addFrameListener( frameListener );
	}


	void GBaseEngine::showLoadingScreen( bool loading )
	{
		guslib::GScopedLock myLock( loadingCmdWriteMutex );
		GTRACE(3, "Engine::" << (loading? "showing": "hiding")<<" loading screen.");

		try
		{
			if( loaderOverlay )
			{
				if( loading )
				{
					loaderOverlay->show();
				}
				else
				{
					loaderOverlay->hide();
				}
			}
			else
			{
				GTRACE(1, "NULL Overlay!");
			}
		}
		catch( ... )
		{
			GTRACE(1, "ERROR showing/hiding loading screen!");
		}
	}



	bool GBaseEngine::isExiting()
	{
		guslib::GScopedLock myLock( exitCmdWriteMutex );
		return exitCmdReceived;
	}



	void GBaseEngine::shutDown()
	{
		guslib::GScopedLock myLock( exitCmdWriteMutex );
		GTRACE(3, "engine received cmd to shutdown");
		GTRACE(4, "forwarding cmd to state mgr");
		GBaseStateManager::getPtr()->setShutdown();
		exitCmdReceived = true;
		//TODO:XXX: consider the possibility of having to remove the resource groups.
		//Note that if you try to remove the groups during rendering, the application may freeze.
		//(this side effect occured randomly while deleting the General group)
	}



	void GBaseEngine::releaseFrameListener()
	{
		frameListener = NULL;
	}


	void GBaseEngine::setWindowIsActive( bool value )
	{
		GTRACE(3, "Window active state change. Is active now:" <<value);
		windowIsActive_=value;
		if(windowIsActive_)
		{
			GSOUND_START_MUSIC();
		}
		else
		{
			GSOUND_PAUSE_MUSIC();
		}
	}

#endif
	//overloads + interf.-------------------------------

	bool GBaseEngine::keyPressed( const OIS::KeyEvent &arg )
	{
		//if( isExiting() ) return true;

		//GTRACE(4, "keyPressed, "<<arg.key);
		//GBaseStateManager::getPtr()->reactToKeyPress( arg.key );
		return true;
	}



	bool GBaseEngine::keyReleased( const OIS::KeyEvent &arg )
	{
		//if( isExiting() ) return true;
		//GTRACE(4, "keyReleased, "<<arg.key);
		//GBaseStateManager::getPtr()->reactToKeyRelease( arg.key );
		return true;
	}

#if 0

	void GBaseEngine::OnStateStackEmpty()
	{
		GTRACE(1, "the state stack is empty... shutting down...");
		shutDown();
	}
#endif
}