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
//    Class to handle the hi-score read and display state

// Extend the basic state
#include <app/SerpState.h>

#include <base/GEvent.h>

#include <OIS.h>
#include <Ogre.h>

#include "model/ScoringTable.h"

// Include the smart pointer.
#include <guslib/thirdparty/yasper.h>
#include "app/SerpGui.h"


namespace Serpents
{
	class HiscoresState 
		: public app::SerpState
	{
	private:
		bool initialized_;
		bool showedScoreNames_;
		bool showingNameInputPanel_;

		bool initialize ();

		void showScoreNames ();
		void clearContent ();

		void showNameInputPanel ();

		//
		// If the player placed a valid name in the input field, hide the window and store the name
		// in the list.
		//
		void hideNameInputPanelAndStoreNameIfValid ();

	protected:
		GBase::GBaseEventQueue logicEventQueue;

		Ogre::Camera *mCamera;

		bool guiWasRenderedOnce;
		bool loadingWasDone;
		bool requestNameRead_;

		// gameplay stuff

		yasper::ptr<Serpents::GScoringTable> scoringTable;
	public:

		HiscoresState(const std::string& name
			, app::SerpEngine *enginePtr
			, yasper::ptr<Serpents::GScoringTable> scoringTablePtr
			);

		//
		// The overrides for the state handling
		//

		// Handle state initialization.
		virtual void enter (const app::SerpStateParams& param) override;
		
		// Handle state control release.
		virtual void exit () override;

		// Suspend the current state. Should most likely be resumed later on.
		virtual void pause () override;
		
		// Resume a suspended state.
		virtual void resume () override;

		//
		// The overrides to handle input
		//
		virtual void reactToKeyPress(OIS::KeyCode keycode) override;
		virtual void reactToKeyRelease(OIS::KeyCode keycode) override;

		virtual void reactToMouseMoved (int x, int y) override;
		virtual void reactToMousePressed (const OIS::MouseButtonID& btn, int x, int y) override;
		virtual void reactToMouseReleased (const OIS::MouseButtonID& btn, int x, int y) override;

		virtual bool handleLogicThreadLoop();
		virtual bool renderScene();
		virtual bool renderGUI();
		virtual void notifyOfShutdown();

		// The overrides to handle the logic callbacks

		// other funcs
		bool onButtonOkClicked (const CEGUI::EventArgs& evt);
		bool onButtonBackClicked (const CEGUI::EventArgs& evt);

		void setScoringTable( GScoringTable *scoringTablePtr ){ scoringTable = scoringTablePtr; }
	};

}