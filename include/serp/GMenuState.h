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
//    The state that handles the main menu.

#if 0
// Extend the basic state
#include <app/SerpState.h>

#include "GEvent.h"
#include "GBaseEngine.h"

// Standard Ogre includes
#include <OIS/OIS.h>
#include <Ogre.h>

#include "model/ScoringTable.h"

#include <guslib/system/thread.h>

namespace Serpents
{
	class GMenuState 
		: public app::SerpState
		//, public guslib::GMenuListener
	{
	private:
		app::SerpEngine *engine;
	protected:
#ifdef G_USE_MULTITHREAD
		guslib::GMutex menuEventProcessMutex;
		guslib::GMutex logicEventProcessMutex;
#endif
		GBase::GBaseEventQueue menuEventQueue;
		GBase::GBaseEventQueue logicEventQueue;
		//GBase::GBaseMenu * menu;

		yasper::ptr<Serpents::GScoringTable> scoringTable;

		GScoringTable * createScoringTable ();
	public:

		// Ctor.
		GMenuState (const std::string& name, app::SerpEngine *enginePtr, yasper::ptr<Serpents::GScoringTable> scoringTablePtr);

		//
		// The overrides for the state handling
		//

		// Handle state initialization.
		virtual void enter (const app::SerpStateParams& param) override;

		virtual void exit();

		virtual void pause() ;
		virtual void resume();

		//
		// The overrides to handle input
		//
		virtual void reactToKeyPress(OIS::KeyCode keycode);
		virtual void reactToKeyRelease(OIS::KeyCode keycode);

		virtual void reactToMouseMoved (int x, int y) override;
		virtual void reactToMousePressed (const OIS::MouseButtonID& btn, int x, int y) override;
		virtual void reactToMouseReleased (const OIS::MouseButtonID& btn, int x, int y) override;

		virtual bool handleLogicThreadLoop();
		virtual bool renderScene();
		virtual bool renderGUI();
		virtual void notifyOfShutdown();

		virtual yasper::ptr<Serpents::GScoringTable> getScoringTablePtr(){ return scoringTable; }

		// GMenuListener overloads
		//virtual void OnMenuAction( guslib::GMenuItem * item );
		//virtual bool OnMenuBack( guslib::GMenuItem * item );
		//virtual void OnMenuShouldRedraw( guslib::GMenu * menu );
		//virtual void OnMenuSelectionChange( guslib::GMenuItem *item );
		//virtual void OnMenuDepthChange( guslib::GMenu * menu, bool fromTop );
		//virtual void OnMenuOptionStartEdit( guslib::GMenuItem *optionGroup, guslib::GMenuItem *option );
		//virtual void OnMenuOptionStopEdit( guslib::GMenuItem *optionGroup, guslib::GMenuItem *option, bool bCanceled );
		//virtual void OnMenuOptionValueChange( guslib::GMenuItem *optionGroup, guslib::GMenuItem *option );

		/// Creates the game menu.
		//virtual GBase::GBaseMenu * createGameMenu();

		/// loads the overlays
		//virtual void loadOverlays();
		virtual void loadSounds();
		//virtual void loadMenuContents();
		//virtual void addResolutionOptions();
		virtual const std::string addResolutionOptionsAndReturnSelected();
		virtual void hideAllMenuPages();

		//virtual void showOverlays();
		virtual void hideOverlays();

	};
}
#endif