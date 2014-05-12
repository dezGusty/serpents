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

//    Basic implementation of an engine.
//    This engine will be tightly related to the Ogre lib and it's way of working.

#include <OIS.h>
#include <Ogre.h>
#if 0
#include "BuildOpts.h"

#include "GBaseMenu.h"
#include "GBaseState.h"

#include <guslib/system/timer.h>
#include "GEvent.h"
#endif

namespace GBase
{
	// Fwd. class decls.

	class GBaseFrameListener;


	// all scene elements will be translated using this vector.
	static Ogre::Vector3 globalTranslationVector = Ogre::Vector3(0,0,0);


	/// This will be the game engine class. It is expected to grow to a considerable size.
	/// The engine class.
	class GBaseEngine
		: public OIS::KeyListener
		//, public GBaseStateManagerListener
	{
	private:
		bool windowIsActive_;
		bool loadingRes_;
		bool rendering_;
	protected:
#if 0

		// The application's scene manager. Used to instantiate new graphical objects.
		Ogre::SceneManager * sceneManager;

		Ogre::Root * root;

		bool exitCmdReceived; // flag for the exit command.
		bool bBootResourcesWereLoaded; // flag for the resource loading.

		Ogre::Overlay * overlay;
		Ogre::Overlay * loaderOverlay;

		GBaseFrameListener * frameListener;

		GBaseEventQueue eventQueue;

		guslib::GMutex loadingCmdWriteMutex; // mutex to control the setting of the loading flag.
		guslib::GMutex exitCmdWriteMutex; // mutex for the exit command.

		// mutex for rendering a scene and loading resources. This is needed also to sync with the 
		// user clicking the window close button, since that may happen during a scene render
		// and the closing of the window will attempt resource deallocation.

		guslib::GMutex loadAndRenderMutex; 

		// Timer to use for various operations.
		guslib::Timer timer;

		// The main camera used by the engine for viewing operations.
		Ogre::Camera *internalCamera;
#endif

	public:
		GBaseEngine( Ogre::SceneManager * aManager, Ogre::Root * rootElement );
		virtual ~GBaseEngine();
#if 0
		/// Load the initial resources. This is done in the resource loading thread, which should be
		/// the same thread as the one that creates the render window and handles the render loop.
		virtual bool loadBootResources();

		virtual guslib::GMutex & getLoadAndRenderMutex(){ return loadAndRenderMutex;}

		/// Tells wether the command was given to exit the application.
		virtual bool isExiting();

		/// Shuts the application down.
		virtual void shutDown();


		/// Says wether the app boot resources were loaded successfully.
		virtual bool bootResourcesWereLoaded(){ return bBootResourcesWereLoaded; }

		virtual void loadOverlays();

		/// Load the initial resources before going active.
		virtual void loadResources();

		/// Note should refactor the code to make clear distinction between sound resources and other graphical res.
		virtual void loadSounds();
#endif
		/// This is basically a copy of the standard OGRE root function "renderOneFrame".
		/// What it does in addition to the aforementioned function is to add a mutex lock for 
		/// loading and rendering operations.
		/// The function should be called from the rendering thread!
		virtual bool renderCompleteScene();

		virtual Ogre::RenderWindow *getRenderWindow();
		virtual Ogre::SceneManager * getSceneManager();

		/// The scene rendering.
		virtual bool renderScene();

#if 0
		// Getter and setter for the internal camera.
		virtual Ogre::Camera * getCamera(){ return internalCamera; }
		virtual void setCamera(Ogre::Camera * cam){ internalCamera = cam; }

		/// A main loop that will be handled before the rendering of the scene.
		virtual bool logicThreadLoop();

		/// The GUI rendering.
		virtual bool renderOverlay();


		virtual GBaseFrameListener* getFrameListener(){ return frameListener; }
		/// Sets the frame listener to use.
		virtual void setFrameListener( const GBaseFrameListener * gfListener );

		/// Shows or hides the loading screen.
		virtual void showLoadingScreen( bool bLoading );

		/// Release the connection to the frameListener. The frame listener is not owned by the engine,
		/// so it will not be destroyed.
		virtual void releaseFrameListener();

		virtual void setWindowIsActive( bool value );
		virtual bool isWindowActive() const { return windowIsActive_; }
#endif
		//overloads + interf.

		// OIS::KeyListener overloads
		virtual bool keyPressed( const OIS::KeyEvent &arg );
		virtual bool keyReleased( const OIS::KeyEvent &arg );		
#if 0
		// GBaseStateManagerListener overloads
		virtual void OnStateStackEmpty();
#endif
	};



}