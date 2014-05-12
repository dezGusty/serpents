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

#include "GBaseFrameListener.h"
#include "GBaseMainApp.h"
#include <guslib/trace/trace.h>

using namespace Ogre;

namespace GBase
{

	GBaseFrameListener::GBaseFrameListener(	GBaseEngine *engine
										, Ogre::RenderWindow *win
										, bool bufferedKeys
										, bool bufferedMouse
										)
										:activeWindow_(true)
	{
		this->engine = engine;
		this->renderWindow = win;

		inputManager = NULL;
		mouse = NULL;
		keyboard = NULL;
		this->mFiltering = TFO_BILINEAR; // TODO: change filtering
		this->mAniso = 1;

		using namespace OIS;

		ParamList pl;
		size_t windowHnd = 0;
		std::ostringstream windowHndStr;

		win->getCustomAttribute("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

		// Insert additional properties to have the mouse as NON-exclusive (meaning one can move the mouse freely outside the window).
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
		pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
		pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE"))); 

		inputManager = InputManager::createInputSystem( pl );

		// Create all devices
		keyboard = static_cast<Keyboard*>(inputManager->createInputObject( OISKeyboard, bufferedKeys ));
		mouse = static_cast<Mouse*>(inputManager->createInputObject( OISMouse, bufferedMouse ));

		//	Set initial mouse clipping size
		windowResized( renderWindow );
		keyboard->setBuffered(true);
		keyboard->setTextTranslation(OIS::Keyboard::Ascii);
		keyboard->setEventCallback( engine );

		// Register as a Window listener
		WindowEventUtilities::addWindowEventListener( renderWindow, this);
	}



	GBaseFrameListener::~GBaseFrameListener(void)
	{
		GTRACE(3, "GBaseFrameListener destructor called...");
		if( engine != NULL ) engine->releaseFrameListener();
		if( engine != NULL ) engine->shutDown();
		if( engine != NULL ) releaseEngine();
		//Remove the current object as a Window listener
		if( renderWindow )
		{
			WindowEventUtilities::removeWindowEventListener(this->renderWindow, this);
			windowClosed(this->renderWindow);
		}
		GTRACE(3, "GBaseFrameListener destructor exiting...");
	}


	void GBaseFrameListener::releaseEngine()
	{ 
		GTRACE(4, "GBaseFrameListener received command to release engine.");
		engine = NULL;
	}




	void GBaseFrameListener::windowResized(RenderWindow* rw)
	{
		if( rw == this->renderWindow )
		{
			if( engine == NULL )
			{
				guslib::GScopedLock myLock(windowOperationsMutex);
				unsigned int width, height, depth;
				int left, top;
				rw->getMetrics(width, height, depth, left, top);
				const OIS::MouseState &ms = mouse->getMouseState();
				ms.width = width;
				ms.height = height;
			}
			else
			{
				guslib::GScopedLock myLock(engine->getLoadAndRenderMutex());
				unsigned int width, height, depth;
				int left, top;
				rw->getMetrics(width, height, depth, left, top);
				const OIS::MouseState &ms = mouse->getMouseState();
				ms.width = width;
				ms.height = height;
			}
		}
	}


	void GBaseFrameListener::windowClosed(RenderWindow* rw)
	{
		GTRACE(3, "GBaseFrameListener windowClosed called...");
		//Only close for renderwindow
		if( rw && rw == this->renderWindow )
		{
			guslib::GScopedLock myLock(windowOperationsMutex);
			if( engine != NULL )
			{
				engine->shutDown();
				GTRACE(3, "GBaseFrameListener sent shutdown cmd to engine...");
			}
			guslib::sleep(100);
			// The input devices should probably wait for a loop of the input reading thread to finish.

			guslib::GScopedLock secondLock(inputObjectsAccessMutex);
			GTRACE(4, "destroying input manager and objects");
			if( inputManager )
			{
				inputManager->destroyInputObject( mouse );
				inputManager->destroyInputObject( keyboard );

				OIS::InputManager::destroyInputSystem(inputManager);
				inputManager = NULL;
				keyboard = NULL;
				mouse = NULL;
			}
			GTRACE(4, "finished destroying input manager and objects");
			renderWindow = NULL;
		}
	}

	void GBaseFrameListener::windowFocusChange(RenderWindow* rw)
	{
		activeWindow_ = !activeWindow_;
		if( rw == this->renderWindow )
		{
			guslib::GScopedLock myLock(windowOperationsMutex);
			engine->setWindowIsActive(activeWindow_);
		}
	}


	bool GBaseFrameListener::frameStarted(const FrameEvent &evt)
	{
		GTRACE(5, "GBaseFrameListener::frameStarted");
		if( this->renderWindow == NULL )
			return false;
		if( this->renderWindow->isClosed())	
			return false;
		guslib::GScopedLock myLock(windowOperationsMutex);

		if( !renderWindow ) return false;
		bool result=true;
		// Call the scene rendering here.
		if( engine != NULL )
		{
			result = engine->renderScene();
		}
		GTRACE(5, "GBaseFrameListener::frameStarted exit");

		return result;
	}



	bool GBaseFrameListener::frameEnded(const FrameEvent &evt)
	{
		GTRACE(5, "GBaseFrameListener::frameEnded");
		guslib::GScopedLock myLock(windowOperationsMutex);
		if( this->renderWindow == NULL )
			return false;
		if( this->renderWindow->isClosed())	
			return false;
		bool result=true;
		if( engine != NULL )
		{
			result = engine->renderOverlay();
		}
		GTRACE(5, "GBaseFrameListener::frameEnded exit");
		return result;
	}

} // namespace GBase