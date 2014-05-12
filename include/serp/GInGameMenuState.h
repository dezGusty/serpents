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
//    The state that controls the "in-game" menu.

#include "GBaseState.h"
#include "GBaseMenu.h"
#include "GEvent.h"
#include "GBaseEngine.h"
#include <OIS/OIS.h>
#include <Ogre.h>


#include <guslib/system/thread.h>

namespace Serpents
{
	class GInGameMenuState : public GBase::GBaseState
		, public guslib::GMenuListener
	{
	private:
		static GInGameMenuState * objectPtr;
		GBase::GBaseEngine *engine;
	protected:
		static guslib::GMutex creationMutex;
		guslib::GMutex menuEventProcessMutex;
		guslib::GMutex logicEventProcessMutex;
		GBase::GBaseEventQueue menuEventQueue;
		GBase::GBaseEventQueue logicEventQueue;
		GBase::GBaseMenu * menu;

		Ogre::Overlay * overlay;
		Ogre::Overlay * loaderOverlay;
		Ogre::OverlayManager * overlayManager;

		GInGameMenuState(GBase::GBaseEngine *enginePtr);

	public:

		static GInGameMenuState * getInstancePtr(GBase::GBaseEngine *enginePtr);
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

		// GMenuListener overloads
		virtual void OnMenuAction( guslib::GMenuItem * item );
		virtual bool OnMenuBack( guslib::GMenuItem * item );
		virtual void OnMenuShouldRedraw( guslib::GMenu * menu );
		virtual void OnMenuSelectionChange( guslib::GMenuItem *item );
		virtual void OnMenuDepthChange( guslib::GMenu * menu, bool fromTop );
		virtual void OnMenuOptionStartEdit( guslib::GMenuItem *optionGroup, guslib::GMenuItem *option );
		virtual void OnMenuOptionStopEdit( guslib::GMenuItem *optionGroup, guslib::GMenuItem *option, bool bCanceled );
		virtual void OnMenuOptionValueChange( guslib::GMenuItem *optionGroup, guslib::GMenuItem *option );

		/// Creates the game menu.
		virtual GBase::GBaseMenu * createGameMenu();

		/// Load overlays that are required solely for this state.
		virtual void loadStateOverlays();

		/// Show or hide the state specific overlays.
		virtual void showStateOverlays( bool show = true );

		/// Clear internal state data.
		virtual void clearStateLogic();
	};
}