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
//    Main app. class def.


#include "GBaseMainApp.h"
#include "GUtils.h"
#include "GBAseSetting.h"
#include "../../res/resource.h"
#include "GBaseEngineDefinitions.h"

#include <guslib/system/random.h>

// the states are required because some of them are called, but all should be released if they are singletons.
#include "GMenuState.h"
#include "GHiscoresState.h"
#include "GLoaderState.h"
#include "GGameState.h"
#include "GIngameMenuState.h"

#include <ogreoverlay.h>
#include <OgrePanelOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>
#include <guslib/system/thread.h>

#include <sstream>
#include "GResourceManager.h"
#include "GAudiere.h"
#include "GBaseEngine.h"
#include "GStencilRenderer.h"

using namespace Serpents;
using namespace Ogre;
using namespace GAudiere;

namespace GBase
{
	bool GBaseMainApp::bExitReq = false;
	bool GBaseMainApp::bInitReady = false; 
	bool GBaseMainApp::bThreadLogicStarted = false;
	bool GBaseMainApp::bThreadLogicFinished = false;
	bool GBaseMainApp::bThreadInputFinished = false;
	bool GBaseMainApp::bThreadNetworkFinished = false;
	bool GBaseMainApp::bThreadRenderFinished = false;
	bool GBaseMainApp::bThreadLogicAllowed = false;

	Ogre::Root * GBaseMainApp::ogreRoot = NULL;
	RenderWindow * GBaseMainApp::renderWindow = NULL;
	Ogre::SceneManager * GBaseMainApp::sceneManager = NULL;
	Ogre::RenderSystem * GBaseMainApp::ptrToRenderSys = NULL;

	GBaseEngine * GBaseMainApp::engine = NULL;
	GBaseFrameListener * GBaseMainApp::frameListener = NULL;
	guslib::GMutex GBaseMainApp::threadStateMutex;
	guslib::GMutex GBaseMainApp::initializationMutex;

	const int waitingForInitSleepDuration = 20;
	const int networkingThreadSleepDuration = 10;
	const int inputThreadSleepDuration = 20;


	// TODO: these functions are duplicates! find a nice place for them.
	// utility functions.
	std::string getShortRenderDeviceNameFromLongName( std::string input )
	{
		if( input == G_RENDER_ENGINE_D3D_LONG_NAME )
			return G_RENDER_ENGINE_D3D_SHORT_NAME;
		if( input == G_RENDER_ENGINE_OGL_LONG_NAME )
			return G_RENDER_ENGINE_OGL_SHORT_NAME;
		return "";	
	}

	std::string getLongRenderDeviceNameFromShortName( std::string input )
	{
		if( input == G_RENDER_ENGINE_D3D_SHORT_NAME )
			return G_RENDER_ENGINE_D3D_LONG_NAME;
		if( input == G_RENDER_ENGINE_OGL_SHORT_NAME )
			return G_RENDER_ENGINE_OGL_LONG_NAME;
		return "";		
	}

	bool GBaseMainApp::isInitializationReady()
	{
		guslib::GScopedLock myLock(initializationMutex);
		return bInitReady;
	}
	void GBaseMainApp::setInitializationReady( bool value )
	{
		guslib::GScopedLock myLock(initializationMutex);
		bInitReady = value;
	}


	void GBaseMainApp::allowLogicThreadToStart(bool bInput)
	{
		guslib::GScopedLock myLock( threadStateMutex );
		bThreadLogicAllowed = bInput;
	}

	bool GBaseMainApp::isThreadRenderFinished()
	{
		return bThreadRenderFinished;
	}
	void GBaseMainApp::setThreadRenderFinished(bool bInput)
	{
		guslib::GScopedLock myLock( threadStateMutex );
		bThreadRenderFinished = bInput;
	}
	bool GBaseMainApp::isThreadLogicFinished()
	{
		return bThreadLogicFinished;
	}
	void GBaseMainApp::setThreadLogicFinished(bool bInput)
	{
		guslib::GScopedLock myLock( threadStateMutex );
		bThreadLogicFinished = bInput;
	}
	bool GBaseMainApp::isThreadLogicStarted()
	{
		return bThreadLogicStarted;
	}
	void GBaseMainApp::setThreadLogicStarted(bool bInput)
	{
		guslib::GScopedLock myLock( threadStateMutex );
		bThreadLogicStarted = bInput;
	}
	bool GBaseMainApp::isThreadInputFinished()
	{
		return bThreadInputFinished;
	}
	void GBaseMainApp::setThreadInputFinished(bool bInput)
	{
		guslib::GScopedLock myLock( threadStateMutex );
		bThreadInputFinished = bInput;
	}
	bool GBaseMainApp::isThreadNetworkFinished()
	{
		return bThreadNetworkFinished;
	}
	void GBaseMainApp::setThreadNetworkFinished(bool bInput)
	{
		guslib::GScopedLock myLock( threadStateMutex );
		bThreadNetworkFinished = bInput;
	}


	bool GBaseMainApp::isExitRequired()
	{
		return bExitReq;
	}

	void GBaseMainApp::setExitRequired(bool bInput )
	{
		guslib::GScopedLock myLock( threadStateMutex );
		bExitReq = bInput;
	}



	bool GBaseMainApp::initialize()
	{
		// load the config.
		try
		{
			if( ! GBaseAppSettings::getPtr ()->isLoaded() )
			{
		        MessageBoxA( NULL, "yo", "no settings loaded man!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
				return false;
			}
		}
		catch(std::exception &e)
		{ 
			MessageBoxA( NULL, (LPCSTR)e.what(), (LPCSTR)"some error for settings!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
			return false; 
		}

		//  start the tracing if required.
		if(GBaseAppSettings::getPtr ()->setting.log.traceFile != "" )
		{
			GSTARTTRACING( GBaseAppSettings::getPtr ()->setting.log.traceFile
						, GBaseAppSettings::getPtr ()->setting.log.traceLevel );
		}

		GTRACE(1, "Trace file for " << GUS_APP_NAME << " v. "<< GUS_APP_VERSION);
		GTRACE(1, "-------------------------------------------------------------");

		// ------------ Start the Ogre app configuring ------------

		GTRACE(3, "Creating Ogre root...");

		ogreRoot = new Ogre::Root("", "", GBaseAppSettings::getPtr ()->setting.log.ogreLog );

		// Note: it is possible to load only one plugin instead of both D3D and OpenGL, but while
		// this will provide a speed-up at the application startup it will make the resolution changes more difficult,
		// since the DLL would still require loading, then and reading the supported settings.
		// So for the sake of simplicity it's better to load the data in a single place.

		// The loading of additional plugins is done in the same loop also for simplicity.

		// make sure the plugin dir has a trailing slash. Allthough it would be nicer to separate between
		// linux slash and windows double backslash... why bother? windows can interpret slash as well.
		std::string pluginFolder = GBaseAppSettings::getPtr ()->setting.res.pluginDir;
		GTRACE(3, "The plugins will be loaded from ["<<pluginFolder<<"]");
		if( pluginFolder.length() > 0 )
		{
			char last_char = pluginFolder[pluginFolder.length()-1];
			if (last_char != '/' && last_char != '\\')
			{
				pluginFolder += "/";
			}
		}
		std::vector<std::string> plugList = GBaseAppSettings::getPtr ()->setting.res.plugins;
		GTRACE(3, "..:: Loading plugin list ("<< plugList.size()<<"  items) ::..");
		for( std::vector<std::string>::iterator it=plugList.begin() ;it!= plugList.end(); ++it )
		{
			std::string myPlugin = pluginFolder + std::string(*it);
			GTRACE(5, "Loading plugin ["<<myPlugin<<"]");
			try
			{
				ogreRoot->loadPlugin( myPlugin );
			}
			catch( Ogre::Exception &e )
			{
				GTRACE(1, "Exception encountered! "<<e.getFullDescription());
				std::string message = std::string("Unable to load (some) plugins.\nAre you sure there are plugins stored at the location [")
					+ pluginFolder + std::string("] ?");
				MessageBoxA( NULL, (LPCSTR)message.c_str(), (LPCSTR)"some error for settings!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
				throw e;
			}
			GTRACE(3, "Loaded plugin ["<<myPlugin<<"]");
		}

		// Load the bootstrap resources.

		std::vector<std::string> resList = GBaseAppSettings::getPtr ()->setting.res.preload;
		addResourceLocationList( resList, "Bootstrap" );

		// Destroy the "General" resource group, since it will be initialized in the engine, and a "loading..." will
		// be displayed while the actual resource loading is done.
		
		ResourceGroupManager::getSingleton().destroyResourceGroup("General");

		// Get the available render systems. We need to select the currently desired one.
		// If you choose to only load one engine at startup, there should be only one entry here.

		GTRACE(4, "Setting rendering engine ...");
		Ogre::RenderSystem *rsys = NULL;
		Ogre::RenderSystemList renderers = ogreRoot->getAvailableRenderers();
		GTRACE(4, "got renderers...");
		for( Ogre::RenderSystemList::iterator rslIt = renderers.begin(); rslIt != renderers.end(); rslIt++ )
		{
			GTRACE(4, "renderer "<<(*rslIt)->getName());
			if( (*rslIt)->getName() == GBaseAppSettings::getPtr ()->setting.video.renderEngineFullName )
			{
				rsys = *rslIt;
				GTRACE(2, "Setting rendering engine to "<<
					GBaseAppSettings::getPtr ()->setting.video.renderEngineFullName);
				break;
			}
		}
		// If we did not find a preferred engine, try to get any available engine.
		if( rsys == NULL )
		{
			if( renderers.empty() )
			{
				GTRACE(1, "ERROR: Unable to get any renderer! (renderers list empty).");
				return false;
			}
			rsys = *(renderers.begin());
		}
		// If we still don't have an engine, it means nothing was loaded, so there's something wrong with the paths.
		if( rsys == NULL )
		{
			GTRACE(1, "ERROR: Unable to get any renderer!");
			return false;
		}
		ogreRoot->setRenderSystem( rsys );
		ptrToRenderSys = rsys;

		GTRACE(3, "Render system set.");
		// also place the setting for this new device.

		GBaseAppSettings::getPtr ()->setting.videoTemp = GBaseAppSettings::getPtr ()->setting.video;
		// The root is initialized with false, in order to allow window customization.
		// Note: this will also initialize registered resource groups.

		ogreRoot->initialise(false);
		GTRACE(3, "Ogre root initialized");

		// -------------------- Create the window to be used ------------------------------

		// Get the general parameters required for window initialization:
		// fullscreen antialiasing (default: 0), window width and height, fullscreen mode and vertical sync.

		int fsaa = GBaseAppSettings::getPtr ()->setting.video.fsaa;
		int width = GBaseAppSettings::getPtr ()->setting.video.windowWidth;
		int height = GBaseAppSettings::getPtr ()->setting.video.windowHeight;
		bool fullscreen = GBaseAppSettings::getPtr ()->setting.video.fullscreen;
		int colourDepth = GBaseAppSettings::getPtr ()->setting.video.colourDepth;

		// Ensure that whatever resolution settings are chosen, they are supported by the system.
		// In case they are not, they will be changed to some acceptable values.

		std::string chosenMode;
		validateWindowSettings( &width, &height, &colourDepth, &chosenMode );

		// after the validation, save the results back in the main settings.
		GBaseAppSettings::getPtr ()->setting.video.renderEngineFullName = rsys->getName();
		GTRACE(3, "Full name of render engine is:"<<GBaseAppSettings::getPtr ()->setting.video.renderEngineFullName);
		GBaseAppSettings::getPtr ()->setting.video.renderEngine = ( getShortRenderDeviceNameFromLongName(
			GBaseAppSettings::getPtr ()->setting.video.renderEngineFullName ) =="Direct3D") ? Direct3D : OpenGL ;
		GTRACE(3, "Short name of render engine is:"<<GBaseAppSettings::getPtr ()->setting.video.renderEngine);
		GBaseAppSettings::getPtr ()->setting.video.windowWidth = width;
		GBaseAppSettings::getPtr ()->setting.video.windowHeight = height;


		GBaseAppSettings::getPtr ()->setting.video.renderModeString = chosenMode;
		GBaseAppSettings::getPtr ()->setting.video.colourDepth = colourDepth;

		Ogre::NameValuePairList miscParams;
		bool vsync = GBaseAppSettings::getPtr ()->setting.video.vsync;
		miscParams["vsync"] = StringConverter::toString(vsync);
		ogreRoot->getRenderSystem()->setWaitForVerticalBlank(vsync);
		miscParams["FSAA"] = StringConverter::toString(fsaa);
		
		//miscParams["Display Frequency"]= StringConverter::toString(0);
		//miscParams["Colour Depth"]=StringConverter::toString(32);

		miscParams["displayFrequency"]= StringConverter::toString(0);
		miscParams["colourDepth"]=StringConverter::toString(32);
		

		renderWindow = ogreRoot->createRenderWindow( GUS_APP_NAME, width, height,
											fullscreen, &miscParams );
		GTRACE(3, "Created app window, size="<<renderWindow->getWidth()<<","<<renderWindow->getHeight()
			<<", fullscr="<<renderWindow->isFullScreen()
			<<", depth="<<renderWindow->getColourDepth());

		// Set the window icon, this should only be visible in windowed mode.
	#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		HWND hwnd;
		renderWindow->getCustomAttribute("WINDOW", &hwnd);
		HINSTANCE hinstance;
		hinstance = GetModuleHandle(NULL); 
		HICON icon = LoadIcon(hinstance, MAKEINTRESOURCE( IDI_GUS_APP_ICON ));
		SendMessage(hwnd, WM_SETICON, ICON_BIG, LPARAM(icon));
		SendMessage(hwnd, WM_SETICON, ICON_SMALL, LPARAM(icon)); 
		GTRACE(3, "Set window icon");

		// Also hide the cursor. This may make the debugging sometimes more difficult.
		//ShowCursor(false);
	#endif

		// Change the default texture filtering mode.
		Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering( Ogre::TFO_ANISOTROPIC );

		// Make the window active and autoupdated when rendered.

		renderWindow->setActive(true);
		renderWindow->setAutoUpdated(true);
		GTRACE(3, "Set window to active + autoupdate");

		// Choose a scene manager. In this case, exterior, from up close.

		sceneManager = ogreRoot->createSceneManager(ST_EXTERIOR_CLOSE, "SceneMgr");

		GTRACE(3, "Creating camera...");

		Ogre::Camera * myCam = sceneManager->createCamera("PlayerCam");
		myCam->setNearClipDistance(5);

		Viewport *vp = renderWindow->addViewport(myCam);
		vp->setBackgroundColour(ColourValue(0,0,0));
		myCam->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));

		// Set default mipmap level (NB some APIs ignore this)

		TextureManager::getSingleton().setDefaultNumMipmaps( 5 );

		// Create the game engine and frame listener.

		engine = new GBaseEngine( sceneManager, ogreRoot );
		engine->setCamera( myCam );
		frameListener = new GBaseFrameListener( engine, renderWindow );
		engine->setFrameListener( frameListener );

		StencilOpQueueListener *stencilListener = new StencilOpQueueListener();
		int minGroup = GBaseAppSettings::getPtr ()->setting.renderg.firstID;
		int maxGroup = GBaseAppSettings::getPtr ()->setting.renderg.lastID;

		if( minGroup != 0 && maxGroup != 0 )
		{
			stencilListener->setFirstStencilOpRenderQueue( minGroup );
			stencilListener->setLastStencilOpRenderQueue( maxGroup );
			for( unsigned i = 0; i<GBaseAppSettings::getPtr ()->setting.renderg.groups.size(); ++i )
			{
				stencilListener->addRenderQueue(
					GBaseAppSettings::getPtr ()->setting.renderg.groups[i].id,
					GBaseAppSettings::getPtr ()->setting.renderg.groups[i].compareFunc,
					GBaseAppSettings::getPtr ()->setting.renderg.groups[i].stencilFailOp,
					GBaseAppSettings::getPtr ()->setting.renderg.groups[i].depthFailOp,
					GBaseAppSettings::getPtr ()->setting.renderg.groups[i].passOp,
					GBaseAppSettings::getPtr ()->setting.renderg.groups[i].value);
			}
			sceneManager->addRenderQueueListener(stencilListener);
		}

		if (ogreRoot->getRenderSystem()->getCapabilities()->hasCapability(RSC_HWRENDER_TO_TEXTURE))
        {
            // In D3D, use a 1024x1024 shadow texture
            sceneManager->setShadowTextureSettings(1024, 2);
        }
        else
        {
            // Use 512x512 texture in GL since we can't go higher than the window res
            sceneManager->setShadowTextureSettings(512, 2);
        }

		setInitializationReady(true);
		return true;
	}



	void GBaseMainApp::cleanup()
	{
		GBaseAppSettings::getPtr ()->save();
		setExitRequired(true);
		GTRACE(3, "Main app cleanup");
		if(engine)
		{
			delete engine;
			engine = NULL;
		}
		if (frameListener)
		{
			delete frameListener;
			frameListener = NULL;
		}

		// Get rid of the used singletons;

		GGameState::deleteInstance();

		GBaseStateManager::destroy();

		// get rid of the random number generator.
		guslib::Random::destroy();

		// kill the game's resource manager.
		GResourceManager::destroy();

		
		// Finally kill the tracing singleton
		GSTOPTRACING();
	}



	void GBaseMainApp::runApp()
	{
		// Start the working threads

		guslib::GThread thread1( &threadRenderMain );
		Sleep(50);
		if( isExitRequired() )
		{
			cleanup();
			return;
		}

		guslib::GThread thread2( &threadLogicHandlingMain );
		Sleep(50);
		if( isExitRequired() )
		{
			cleanup();
			return;
		}

		guslib::GThread thread3( &threadInputMain );
		Sleep(50);
		if( isExitRequired() )
		{
			cleanup();
			return;
		}

		//guslib::GThread thread4( &threadNetworkingMain );
		//if( isExitRequired() )
		//{
		//	cleanup();
		//	return;
		//}

		// Join the main thread. This means, we wait for this thread to finish.
		// It also means that we must ensure that the other threads will be finished prior to this main thread.
		thread1.join();

		cleanup();
	}


	void GBaseMainApp::stopAndCleanupApp ()
	{
		setExitRequired(true);
		cleanup ();
	}



	// -----------------------------------------------------------------------------------------------
	// Game Logic thread.
	// -----------------------------------------------------------------------------------------------

	void GBaseMainApp::threadLogicHandlingMain()
	{
		GTRACE(3, "GBaseMainApp::threadLogicHandlingMain start; waiting for boot resources to be loaded.");

		// Wait for the initialization to finish.
		while( !isExitRequired() && !isInitializationReady() )
		{
			guslib::yield();
			guslib::sleep( waitingForInitSleepDuration );
		}
		if( isExitRequired() ) return;

		if( ptrToRenderSys )
		{
			GTRACE(3, "GBaseMainApp::threadLogicHandlingMain registering to rendersys.");
			ptrToRenderSys->registerThread();
			setThreadLogicStarted();
		}
		else
		{
			setExitRequired(true);
			return;
		}

		GTRACE(3, "GBaseMainApp::threadLogicHandlingMain done waiting for init. starting translation.");
		// do the loop
		bool bKeepLoop = true;
		while( bKeepLoop )
		{
			if( engine->isExiting() )
			{
				break;
			}
			engine->logicThreadLoop();

			if( bKeepLoop && engine->isExiting() )
			{
				GTRACE(3, "Main App thread: engine says it is exiting");
				bKeepLoop = false;
				setExitRequired(true);
			}


			//	Add some cpu idle time.
			int iSleep = GBaseAppSettings::getPtr ()->setting.perf.cpuSleep;

			// If setting is smaller than 1, this means no pause for the thread.
			// (for the value 0, the thread will cede cpu control to other threads with similar priority)
			if( iSleep >= 0 )
			{
                guslib::yield();
                guslib::sleep(iSleep);
			}
		}
		setThreadLogicFinished();
		GTRACE(3, "GBaseMainApp::threadLogicHandlingMain unregistering from rendersys.");
		if( ptrToRenderSys )
		{
			ptrToRenderSys->unregisterThread();
		}
		GTRACE(3, "GBaseMainApp::threadLogicHandlingMain end.");
	}



	// -----------------------------------------------------------------------------------------------
	// Main Render thread.
	// -----------------------------------------------------------------------------------------------

	void GBaseMainApp::threadRenderMain()
	{
		if( ! initialize() )
		{
			setExitRequired(true);
			cleanup();
			return;
		}

		GTRACE(3, "GBaseMainApp::threadRenderMain start.");
		if( engine == NULL || ptrToRenderSys == NULL)
		{
			setExitRequired(true);
			return;
		}

		ptrToRenderSys->preExtraThreadsStarted();

		GTRACE (5, "Creating game states");
		GBaseStateManager::getPtr()->addState( GLoaderState::getInstancePtr(engine) );
		GBaseStateManager::getPtr()->addState( GMenuState::getInstancePtr(engine) );
		Serpents::GScoringTable * scoringTable = GMenuState::getInstancePtr(engine)->getScoringTablePtr();
		GBaseStateManager::getPtr()->addState( GHiscoresState::getInstancePtr(engine, scoringTable, false) );
		int itemNo = (int)guslib::Random::getPtr()->getValueUpTo
			((long)GBase::GBaseAppSettings::getPtr ()->level.defs.size() );
		std::string fileOfLevel = std::string("data/levels/") + GBase::GBaseAppSettings::getPtr ()->level.defs[itemNo].fileOfLevel;
		GBaseStateManager::getPtr()->addState( GGameState::getInstancePtr(engine, scoringTable, fileOfLevel) );
		GBaseStateManager::getPtr()->addState( GInGameMenuState::getInstancePtr(engine) );

		// signal that the logic handling thread may start.
		allowLogicThreadToStart();

		int nMax = 5000; // 5 seconds;

		GTRACE (4, "Waiting for the logic handling thread to start");
		// wait for the logic thread to start and register itself.
		while( ! isThreadLogicStarted() && nMax > 0)
		{
			guslib::yield();
			guslib::sleep(10);
			nMax -= 10;
		}

		if( nMax<0)
		{
			setExitRequired(true);
			return;
		}
		ptrToRenderSys->postExtraThreadsStarted();

		// Make the next state to become "loader"
		GBaseStateManager::getPtr()->pushStateNextTick( "loader" );
		GTRACE(3, "Changing state to loader...");

		GTRACE(3, "Beginning loop");
		bool bKeepLoop = true;
		while( bKeepLoop )
		{
			if( engine->isExiting() )
			{
				break;
			}

			if( engine->isExiting() )
			{
				break;
			}

			//	allow the message pumping to take place. Without it, application switching (alt-tab) will not work ok.
			Ogre::WindowEventUtilities::messagePump();

			if( engine->isExiting() )
			{
				break;
			}

			if( ! ogreRoot )
			{
				GTRACE(1, "ERROR!!! ogre root element is NULL");
				break;
			}

			//	Render a frame
			bKeepLoop = engine->renderCompleteScene();

			if( bKeepLoop && engine->isExiting() )
			{
				GTRACE(3, "Main App thread: engine says it is exiting");
				bKeepLoop = false;
				setExitRequired(true);
			}

			//	Add some cpu idle time.
			int iSleep = GBaseAppSettings::getPtr ()->setting.perf.delayBetweenRenders;
			if( iSleep > 10 ) iSleep = 10;

			// If setting is smaller than 1, this means no pause for the thread.
			// (for the value 0, the thread will cede cpu control to other threads with similar priority)
			if( iSleep >= 0 )
			{
				guslib::yield();
                guslib::sleep(iSleep);
			}
		}

		// This is the main thread; it was joined with the application's main thread, so before leaving this
		// one, make sure all the other threads have the chance to finish nicely, otherise their new state
		// will be a total mistery => errors/crashes as game exit.
		setExitRequired();
		waitForSecondaryThreadsToFinish();

		if (ogreRoot)
		{
			delete ogreRoot;
			ogreRoot = NULL;
		}

		setThreadRenderFinished();
		GTRACE(3, "GBaseMainApp::thread1 clean end.");
	}



	// -----------------------------------------------------------------------------------------------
	// Input reading thread.
	// -----------------------------------------------------------------------------------------------
	void GBaseMainApp::threadInputMain()
	{
		GTRACE(3, "GBaseMainApp::threadInputMain start.");
		GTRACE(3, "GBaseMainApp::threadInputMain waiting for init.");

		// Wait for the initialization to finish.
		while( !isExitRequired() && !isInitializationReady() )
		{
			guslib::yield();
			guslib::sleep (waitingForInitSleepDuration);
		}

		GTRACE(3, "GBaseMainApp::threadInputMain done waiting for init. starting translation.");

		// Start the processing.
		while( !isExitRequired() )
		{
			if( frameListener != NULL )
			{
				guslib::GScopedLock myLock( frameListener->getInputObjectsAccessMutex() );
				if( isExitRequired() || frameListener == NULL || frameListener->getOISKeyboard() == NULL )
					break;
				frameListener->getOISKeyboard()->capture();
			}
			guslib::yield();
			guslib::sleep(inputThreadSleepDuration);
		}
		setThreadInputFinished(true);
		GTRACE(3, "GBaseMainApp::threadInputMain end.");
	}



	void GBaseMainApp::threadNetworkingMain()
	{
		GTRACE(3, "GBaseMainApp::networking thread start.");

		// Wait for the initialization to finish.
		while( !isExitRequired() && !isInitializationReady() )
		{
			guslib::yield();
			guslib::sleep(waitingForInitSleepDuration);
		}

		GTRACE(3, "GBaseMainApp::threadNetworkingMain done waiting for init. starting translation.");

		// Start the processing.
		while( !isExitRequired() )
		{
			guslib::yield();
			guslib::sleep(inputThreadSleepDuration);
		}
		setThreadNetworkFinished(true);
		GTRACE(3, "GBaseMainApp::threadNetworkingMain end.");
	}



	void GBaseMainApp::waitForSecondaryThreadsToFinish()
	{
		GTRACE(4, "Waiting for secondary threads to finish... iexitreq="<<isExitRequired());
		// giving a max of 5 seconds
		int nMaxCount = 5000;
		int nDelay = 10;

		while( ! isThreadInputFinished() && !isThreadNetworkFinished() && !isThreadLogicFinished() && nMaxCount > 0)
		{
			GTRACE(5, "timeleft... " << nMaxCount );
			guslib::yield();
			guslib::sleep(nDelay);
			nMaxCount -= nDelay;
		}
		GTRACE(4, "Waited for secondary threads to finish... time left was: " << nMaxCount );
	}


	void GBaseMainApp::addResourceLocationList( std::vector<std::string> locList, std::string groupToAddTo )
	{		
		GTRACE(3, "..:: Adding resource locations ("<<locList.size()<<" items)...");
		for( std::vector<std::string>::iterator it=locList.begin(); it!= locList.end(); ++it )
		{
			std::string secName, typeName, archName;
			archName = *it;
			secName = groupToAddTo;
			typeName = "FileSystem";
			if( archName.length()>4 )
			{
				if(archName.substr(archName.length()-4) == ".zip" )
				{
					typeName = "Zip";
				}
			}
			ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
			GTRACE(3, "Adding location to resource list:["<<archName<<"], group:["<<secName<<"]");
		}
	}




	// --------------- render engine and video settings support functions --------------

	bool GBaseMainApp::isRenderEngineSupported( std::string renderEngine )
	{
		if( renderEngine == G_RENDER_ENGINE_D3D_LONG_NAME
			|| renderEngine == G_RENDER_ENGINE_OGL_LONG_NAME )
		{
			return true;
		}
		return false;
	}


	// get the resolutions for a given render engine.
	void GBaseMainApp::getResolutionOptionsForEngine( std::string engine,
													std::vector<std::string> *resList, 
													int * currentSelectionIndex,
													std::string * currentSelection,
													bool bOnlyWidthHigher)
	{
		Ogre::RenderSystem *rsys=ogreRoot->getRenderSystemByName(engine);
		if( rsys )
		{
			ConfigOptionMap options = rsys->getConfigOptions();
			for( ConfigOptionMap::iterator it = options.begin(); it != options.end(); it++ )
			{
				if( it->second.name == "Video Mode" )
				{
					// Direct3D settings are displayed as 640 x 400 @ 16-bit colour
					// OpenGL settings are displayed as 640 x 400
					
					for (StringVector::iterator opt_it = it->second.possibleValues.begin();opt_it != it->second.possibleValues.end(); ++opt_it) 
					{
						std::string sFullEntry = *opt_it;
						int width=0;
						int height=0;
						GBaseMainApp::getResolutionFromString( sFullEntry, width, height );

						// If only the landscape mode is allowed, filter the results.
						if( width && height && ((!bOnlyWidthHigher) || (bOnlyWidthHigher && width>height) ))
						{
							resList->push_back( sFullEntry );
						}
					}
				}
			}
		}
		* currentSelection = "";
		* currentSelectionIndex = 0;

		int myWidth, myHeight;
		std::string engineStr = GBaseMainApp::getCurrentRenderOptions(myWidth, myHeight);

		if( engine == engineStr )
		{
			for( int i = 0; i<(int)resList->size(); i++ )
			{
				if( GBaseAppSettings::getPtr ()->setting.video.renderModeString == 
					(*resList)[i] )
				{
					* currentSelectionIndex = i;
					* currentSelection = (*resList)[i];

				}
			}
		}


	}



	Ogre::String GBaseMainApp::getCurrentRenderOptions( int &width, int &height )
	{
		width = getRenderWindow()->getWidth();
		height = getRenderWindow()->getHeight();
		return ogreRoot->getRenderSystem()->getName();
	}


	void GBaseMainApp::getResolutionFromString( Ogre::String resString, int &width, int &height )
	{
		unsigned nPos = (int)resString.find(" x ");
		if( nPos == std::string::npos )
		{
			// some error...
			GTRACE(1, "getResolutionFromString: unexpected string:"<<resString);
			return;
		}

		std::string sLeft, sRight, sTempLeft;
		sTempLeft = resString.substr(0, nPos);
		sRight= resString.substr(nPos+3);// + 3 for " x "
		nPos = (int)sTempLeft.find_last_of(" ");
		sLeft = sTempLeft.substr(nPos+1);
		nPos = (int)sRight.find(" ");
		if( nPos == std::string::npos )
		{
			// if it's not found, we most likely have an opengl mode.
			// currently, no additional handling of this case.
		}
		std::string sAux = sRight.substr(0, nPos);
		width = atoi( sLeft.c_str() );
		height = atoi( sAux.c_str() );
	}



	std::vector<std::string> GBaseMainApp::getResolutionOptions(Ogre::String engName, bool bOnlyWidthHigher
													 , int widthLimiter, int heightLimiter)
	{
		std::vector<std::string> output;
		int width=0;
		int height=0;
		ConfigOptionMap options;
		if( engName=="" ) // if no engine was given, use the current one.
		{
			options = ogreRoot->getRenderSystem()->getConfigOptions();
		}
		else // custom search, engine based.
		{
			Ogre::RenderSystem *rsys=ogreRoot->getRenderSystemByName(engName);
			if( !rsys )
				return output;
			options = rsys->getConfigOptions();
		}
		for( ConfigOptionMap::iterator it = options.begin(); it != options.end(); it++ )
		{
			if( it->second.name == "Video Mode" )
			{
				std::stringstream gssTemp;
				std::string sTemp;
				StringVector::iterator opt_it;

				// Direct3D settings are displayed as 640 x 400 @ 16-bit colour
				// OpenGL settings are displayed as 640 x 400
				
				gssTemp<<width<<" x "<<height;
				sTemp = gssTemp.str();

				for (opt_it = it->second.possibleValues.begin();opt_it != it->second.possibleValues.end(); ++opt_it) 
				{
					std::string sFullEntry = *opt_it;
					GTRACE(5, "getResolutionOptions, str="<<sFullEntry);
					
					getResolutionFromString( sFullEntry, width, height );

					if( (!bOnlyWidthHigher) || (bOnlyWidthHigher && width>height) )
					{
						if( width && height && ( (widthLimiter != 0 && width==widthLimiter )|| widthLimiter == 0)
							&& ((heightLimiter!=0 && height==heightLimiter) || heightLimiter ==0 ) )
						{
							output.push_back( sFullEntry );
						}
					}
				}
			}
		}
		return output;
	}


	// This function is called once after startup to provide a mechanism ensuring that the used resolution
	// is possible, with a fallback case of 800x600 or anything found.

	void GBaseMainApp::validateWindowSettings( int *width, int *height, int *depth, std::string * modeString )
	{
		ConfigOptionMap options = ogreRoot->getRenderSystem()->getConfigOptions();
		for( ConfigOptionMap::iterator it = options.begin(); it != options.end(); it++ )
		{
			if( it->second.name == "Video Mode" )
			{
				std::stringstream gssTemp;
				std::string sTemp;
				size_t nPos;
				StringVector::iterator opt_it;
				std::vector<std::string> possibleResolutions;

				// Direct3D settings are displayed as 640 x 400 @ 16-bit colour
				// OpenGL settings are displayed as 640 x 400
				
				gssTemp<<*width<<" x "<<*height;
				sTemp = gssTemp.str();

				// first, try to find the width x height resolution, opengl style

				for (opt_it = it->second.possibleValues.begin();
							opt_it != it->second.possibleValues.end(); opt_it++) 
				{
					nPos = (*opt_it).find(sTemp);
					if(nPos != std::string::npos )
					{
						possibleResolutions.push_back( *opt_it );
					}
				}

				// If the requested resolution was found in some instances, try to set the desired depth.

				if( possibleResolutions.size() > 0 )
				{
					gssTemp.str("");
					gssTemp<<*width<<" x "<<*height<<" @ "<<*depth;
					sTemp = gssTemp.str();
					for( std::vector<std::string>::iterator myIt = possibleResolutions.begin(); 
						myIt != possibleResolutions.end(); ++myIt )
					{
						nPos = (*myIt).find(sTemp);
						if(nPos != std::string::npos )
						{
							// The resolution is found, use it as is.
							GTRACE(4, "GBaseMainApp::validateWindowSettings usage of resolution ["<<sTemp<<"] approved");
							return;
						}
					}

					// If this point is reached, the desired depth was not found. Therefore, choose any other
					// depth, but keep the same screen resolution.

					* depth = 0; // indicate that the requested colour depth wasn't set.
					* modeString = possibleResolutions[0];
					return;
				}

				// The resolution is not available, try to pick a new one.
				GTRACE(1, "GBaseMainApp::validateWindowSettings usage of resolution "<<*width<<" x "<<*height<<" rejected.");

				// Try a 640x480 resolution, any depth. If that fails, use whatever is found.

				sTemp = "640 x 480";
				for (opt_it = it->second.possibleValues.begin();
							opt_it != it->second.possibleValues.end(); opt_it++) 
				{
					nPos = (*opt_it).find(sTemp);
					if( nPos != std::string::npos )
					{
						*width = 640;
						*height = 480;
						// The resolution is found, use it as is.
						GTRACE(4, "GBaseMainApp::validateWindowSettings changed resolution to "<< *width<<" x "<< *height<<".");
						return;
					}
				}

				GTRACE(1, "GBaseMainApp::validateWindowSettings failed to change to "<<sTemp);

				// Failed default, try anything.
				
				for (opt_it = it->second.possibleValues.begin();
							opt_it != it->second.possibleValues.end(); opt_it++) 
				{
					std::string sFullEntry = *opt_it;
					std::string sLeft, sRight;
					nPos = sFullEntry.find(" ");
					if( nPos == std::string::npos )
					{
						// some error...
						GTRACE(3, "Unable to find space in ["<<sFullEntry<<"]");
						return;
					}
					sLeft = sFullEntry.substr(0, nPos);
					sRight= sFullEntry.substr(nPos+3);// + 3 for " x "
					nPos = sRight.find(" ");
					std::string sAux;
					if( nPos == std::string::npos )
					{
						// we don't have a colour depth.
						sAux = sRight;
					}
					else
						sAux = sRight.substr(0, nPos);
					*width = atoi( sLeft.c_str() );
					*height = atoi( sAux.c_str() );
					GTRACE(1, "GBaseMainApp::validateWindowSettings changed resolution to "<< *width<<" x "<< *height<<".");
					return;
				}

				GTRACE(1, "GBaseMainApp::validateWindowSettings failed to find a display resolution it would agree to.");
			}
		}
	}
}