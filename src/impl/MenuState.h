#ifndef _APP_MENU_STATE_H
#define _APP_MENU_STATE_H

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

// Extend the basic state
#include "app/serpentsstate.h"

// Standard Ogre includes
#include <OIS.h>

#include "app/serpentsgui.h"

namespace Serpents
{
	class MenuState 
		: public app::SerpState
	{
	private:
		bool initialized_;
		bool initialize ();

		void quitApp ();
		void newGame ();
		void showOptions (bool onOffFlag);

		CEGUI::FrameWindow *optionsWindow;
		void loadResolutionOptionsForSelectedRenderer ();

		app::SerpStateParams enteringParams;

	public:
		MenuState (const std::string& name
			, app::SerpEngine *enginePtr
			);


		//
		// The overrides for the state handling
		//

		// Handle state initialization.
		virtual void enter (const app::SerpStateParams& params) override;
		virtual void exit ();

		virtual void pause () ;
		virtual void resume ();

		//
		// The overrides to handle input
		//
		virtual void reactToKeyPress(OIS::KeyCode keycode);
		virtual void reactToKeyRelease(OIS::KeyCode keycode);

		virtual void reactToMouseMoved (int x, int y) override;
		virtual void reactToMousePressed (const OIS::MouseButtonID& btn, int x, int y) override;
		virtual void reactToMouseReleased (const OIS::MouseButtonID& btn, int x, int y) override;

		virtual bool handleLogicThreadLoop ();
		virtual bool renderScene ();
		virtual bool renderGUI ();
		virtual void notifyOfShutdown ();

		bool onButtonQuitClicked (const CEGUI::EventArgs& evt);
		bool onButtonOptOkClicked (const CEGUI::EventArgs& evt);
		bool onButtonOptCancelClicked (const CEGUI::EventArgs& evt);
		bool onButtonNewGameClicked (const CEGUI::EventArgs& evt);
		bool onButtonOptionsClicked (const CEGUI::EventArgs& evt);

		bool onComboValueChange (const CEGUI::EventArgs& evt);

	};
}


#endif // _APP_MENU_STATE_H