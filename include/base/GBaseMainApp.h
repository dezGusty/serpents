#pragma once

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

#include <Ogre.h>

#include "SharedStrings.h"

#include "GBaseFrameListener.h"
#include "GBaseEngine.h"

namespace GBase
{
	///	This will be the main application.
	/// The main components are grouped in this class as static objects.
	/// The application will be split into several threads, each handling a different aspect of the game.
	///		The first thread will handle the rendering, calling the ogre 2d and 3d rendering in its loop. 
	///		The second thread will handle the game logic. The rendering thread will require a copy of the state
	/// of this thread to render all the stuff. Having a copy at its disposal means that this second thread
	///	can go on with the game processing while the rendering is being done.
	///		The third thread will handle the user input. The input handling is separated into another thread
	/// so that it would not affect the control at low fps and to provide a smoother input to users with 
	/// weaker hardware. This also means that the input will be buffered.
	///		The fourth thread will handle the networking aspects of the game. It will basically handle the
	///	communication between the current application's logic thread and a remote's app.
	/// The application's main thread will wait for the game logic thread to finish.
	class GBaseMainApp
	{
	public:
		/// Returns the window title
		static const std::string getTitle(){return GUS_APP_TITLE;}

		///	Getter for the render window.
		static Ogre::RenderWindow *getRenderWindow() { return renderWindow; }

		static void runApp();

		static void stopAndCleanupApp ();

		/// @param bOnlyWidthHigher Retrieve only the resolutions where the width is higher than the height.
		/// @param widthLimiter limit the width to this (ignored if zer0).
		/// @param heightLimiter limit the height to this value (ignored if zer0).
		static std::vector<std::string> getResolutionOptions( Ogre::String engName="", bool bOnlyWidthHigher=true
			, int widthLimiter=0, int heightLimiter=0 );

		/// get the current window settings and render engine.
		static Ogre::String getCurrentRenderOptions( int &width, int &height );

		/// Get the width and height from a resolution string (e.g. "800 x 600 @ 32bpp") => 800,600
		static void getResolutionFromString( Ogre::String resString, int &width, int &height );

		///	Utility function to retrieve all the resolution options.
		/// Normally you shouldn't really need all the information here.
		//static void getAllResolutionOptions( std::vector<std::string> *engineList,
		//									std::string * currentEngine, 
		//									int * currentEngineIdx,
		//									std::vector<std::vector<std::string>> * resList, 
		//									std::vector<std::string> * currentResSelections,
		//									std::vector<int> * selectedIndices
		//									);

		static void getResolutionOptionsForEngine( std::string engine,
													std::vector<std::string> *resList, 
													int * currentSelectionIndex,
													std::string * currentSelection,
													bool bOnlyWidthHigher=true );

		static bool isRenderEngineSupported( std::string renderEngine );

		/// Add a new list of resource locations to a certain group.
		static void addResourceLocationList( std::vector<std::string> locList, std::string groupToAddTo );

	protected:
		/// Flag that controls the entire's application running/closing state. 
		/// Basically any thread has read+write access to this flag.
		static bool bExitReq;

		/// Flag that says wether the initialization is finished. This is written by the rendering thread.
		static bool bInitReady;

		static bool bThreadLogicFinished;
		static bool bThreadLogicStarted;
		static bool bThreadInputFinished;
		static bool bThreadNetworkFinished;
		static bool bThreadRenderFinished;
		static bool bThreadLogicAllowed;

		static bool isInitializationReady();
		static void setInitializationReady( bool value );

	protected:
		///	Constructor.
		GBaseMainApp(void){};

		///	Destructor. Deletes the frame listener and the root element.
		virtual ~GBaseMainApp(void){};

		/// Getter for the exit requirement flag.
		static bool isExitRequired();

		/// Setter for the exit requirement flag.
		static void setExitRequired(bool bInput = true);

		/// Do the initialization. Should be called from the main thread.
		static bool initialize();

		/// Do the cleanup. Basically, the equivalent of a static destructor.
		static void cleanup();

		/// After changes in the graphics hardware/software, some of the previously stored settings may
		/// no longer be available. This ensures that the application will try to use some valid settings.
		/// The function is called in the initialization, done in the rendering thread.
		static void validateWindowSettings( int *width, int *height, int *depth, std::string * modeString );

		static void threadLogicHandlingMain();
		static void threadRenderMain();
		static void threadInputMain();
		static void threadNetworkingMain();

		static void thread1main();
		static void thread2main();

		static void translateOISKeysToSerpents( OIS::Keyboard* mKeyboard );
		static void waitForSecondaryThreadsToFinish();

		static bool isThreadLogicStarted();
		static void setThreadLogicStarted(bool bInput = true);
		static bool isThreadLogicFinished();
		static void setThreadLogicFinished(bool bInput = true);
		static bool isThreadRenderFinished();
		static void setThreadRenderFinished(bool bInput = true);
		static bool isThreadInputFinished();
		static void setThreadInputFinished(bool bInput = true);
		static bool isThreadNetworkFinished();
		static void setThreadNetworkFinished(bool bInput = true);

		static void allowLogicThreadToStart(bool bInput = true);

		static guslib::GMutex threadStateMutex;
		static guslib::GMutex initializationMutex;
		static guslib::GMutex t2Mutex_1;
		static guslib::GMutex t2Mutex_2;
		static guslib::GMutex t2Mutex_3;

		static Ogre::Root *ogreRoot;
		static Ogre::RenderWindow *renderWindow;
		static Ogre::SceneManager *sceneManager;

		static GBaseEngine * engine;
		static GBaseFrameListener *frameListener;
		static Ogre::RenderSystem * ptrToRenderSys;

	};
} // namespace GBase