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
//
//    The state that handles the initial resource loading.

#include "GBaseState.h"
#include "GBaseEngine.h"

#include <Ogre.h>

#include <guslib/system/thread.h>

namespace Serpents
{
	class GLoaderState 
		: public GBase::GBaseState
	{
	private:
		static GLoaderState * objectPtr;
		GBase::GBaseEngine * engine;
	protected:
		static guslib::GMutex creationMutex;
		Ogre::Overlay * overlay;
		Ogre::Overlay * loaderOverlay;
		Ogre::OverlayManager * overlayManager;

		GLoaderState(GBase::GBaseEngine * enginePtr);

		bool guiWasRenderedOnce;
		bool loadingWasDone;
	public:

		static GLoaderState * getInstancePtr(GBase::GBaseEngine * enginePtr);
		static void deleteInstance();

		// The overrides for the state handling
		virtual void enter(const GUtils::GAny & param = GUtils::GAny());
		virtual void exit();

		virtual void pause() ;
		virtual void resume();

		virtual void reactToKeyPress(OIS::KeyCode keycode);
		virtual void reactToKeyRelease(OIS::KeyCode keycode);
		virtual bool handleLogicThreadLoop();
		virtual bool renderScene();
		virtual bool renderGUI();
		virtual void notifyOfShutdown();

	};
}