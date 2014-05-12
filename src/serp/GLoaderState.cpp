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

#include "GLoaderState.h"

//#include "GMenuState.h"
#include <guslib/trace/trace.h>

using namespace GBase;
using namespace Ogre;
using namespace GUtils;

#ifndef LOAD_RESOURCES_IN_LOGIC_THREAD
#define LOAD_RESOURCES_IN_LOGIC_THREAD 0
#endif

namespace Serpents
{
	GLoaderState * GLoaderState::objectPtr=NULL;
	guslib::GMutex GLoaderState::creationMutex;

	GLoaderState * GLoaderState::getInstancePtr( GBaseEngine * enginePtr )
	{
		if( !objectPtr )
		{
			guslib::GScopedLock myLock( creationMutex );
			//recheck to see if the object was not constructed while trying to obtain the lock.
			if( !objectPtr )
			{
				volatile GLoaderState * tempPtr = new GLoaderState(enginePtr);
				objectPtr = (GLoaderState*)tempPtr;
			}
		}
		return objectPtr;
	}


	void GLoaderState::deleteInstance()
	{
		if( objectPtr )
		{
			guslib::GScopedLock myLock( creationMutex );
			//recheck to see if the object was not deleted while trying to obtain the lock.
			if( objectPtr )
			{
				delete objectPtr;
				objectPtr = NULL;
			}

		}
	}


	GLoaderState::GLoaderState(GBaseEngine * enginePtr)
		: GBaseState("loader")
		, engine(enginePtr)
	{
	}



	void GLoaderState::enter(const GAny & param)
	{
		GTRACE(4, "GLoaderState: Entered.");

		// load the boot resources... the absolute must-have resources

		guiWasRenderedOnce = false;
		loadingWasDone = false;

		GTRACE(4, "GLoaderState loading boot overlay...");
		if( !engine || ! engine->loadBootResources() )
		{
			GTRACE(2, "FAIL while loading boot overlay...");
			return;
		}
		
		// show the "Loading..." screen.

#if LOAD_RESOURCES_IN_LOGIC_THREAD
		// nothing left
#else
		// handle some pre-rendering actions
		if( guiWasRenderedOnce && !loadingWasDone )
		{
			GTRACE(4, "L\tLoader state (renderT) loading res.");
			if( engine ) engine->loadResources();
			GTRACE(4, "L\tLoader state (renderT) loaded res.");
			// Load the GUI

			if( engine ) engine->loadSounds();
			GTRACE(4, "L\tLoader state (renderT) loaded sounds.");
			if( engine ) engine->loadOverlays();
			GTRACE(4, "L\tLoader state (renderT) loaded overlays.");
			loadingWasDone = true;
		}

#endif

		GTRACE(4, "setting loading screen");
		if( engine ) engine->showLoadingScreen(true);
		
	}

	void GLoaderState::exit()
	{
		// Everything required for the next stage should be loaded;
		// clear the "Loading..." screen.
		if( engine ) engine->showLoadingScreen(false);
		GTRACE(4, "cleared loading screen.");
	}

	void GLoaderState::pause() 
	{
		// Can't be paused/resumed.
	}

	void GLoaderState::resume()
	{
		// Can't be paused/resumed.
	}

	void GLoaderState::reactToKeyPress(OIS::KeyCode keycode)
	{
		// State doesn't handle input.
	}

	void GLoaderState::reactToKeyRelease(OIS::KeyCode keycode)
	{
		// State doesn't handle input.
	}

	bool GLoaderState::handleLogicThreadLoop()
	{
		if( loadingWasDone )
		{
			GTRACE (4, "Loading was done; at next tick, the state will switch to menu");
			GBaseStateManager::getPtr()->setStateNextTick("menu");
			return true;
		}

#if LOAD_RESOURCES_IN_LOGIC_THREAD
		// handle some pre-rendering actions
		if( guiWasRenderedOnce && !loadingWasDone )
		{
			GTRACE(4, "L\tloading res.");
			if( engine ) engine->loadResources();
			GTRACE(4, "L\tloaded res.");
			// Load the GUI

			if( engine ) engine->loadSounds();
			GTRACE(4, "L\tloaded sounds.");
			if( engine ) engine->loadOverlays();
			GTRACE(4, "L\tloaded overlays.");
			loadingWasDone = true;
		}
#endif

		return true;
	}

	bool GLoaderState::renderScene()
	{
		return true;
	}

	bool GLoaderState::renderGUI()
	{
		guiWasRenderedOnce = true;
		return true;
	}

	void GLoaderState::notifyOfShutdown()
	{
		engine = NULL;
	}
}