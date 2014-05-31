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

#include "impl/MenuState.h"
#include <guslib/trace/Trace.h>

#include "app/SerpEngine.h"

#include "utils/CEGUIUtils.h"
#include "app/SerpentsGameHelper.h"

namespace Serpents
{
	MenuState::MenuState (const std::string& name
		, app::SerpEngine *enginePtr
		//, yasper::ptr<Serpents::GScoringTable> scoringTablePtr
		)
		: SerpState (name, enginePtr)
		, initialized_ (false)
	{
	}

	// The overrides for the state handling
	void MenuState::enter (const app::SerpStateParams& params)
	{
		GTRACE(4, "Entering main menu state...");

		enteringParams = params;
	}

	void MenuState::exit ()
	{
		enginePtr_->shutDown ();
		enteringParams.getConfig ().clear ();
	}

	void MenuState::pause ()
	{
	}


	void MenuState::resume ()
	{
	}

	void MenuState::reactToKeyPress(OIS::KeyCode keycode)
	{
	}

	void MenuState::reactToKeyRelease(OIS::KeyCode keycode)
	{
	}


	void MenuState::reactToMouseMoved (int x, int y){}

	void MenuState::reactToMousePressed (const OIS::MouseButtonID& btn, int x, int y){}

	void MenuState::reactToMouseReleased (const OIS::MouseButtonID& btn, int x, int y){}




	bool MenuState::handleLogicThreadLoop ()
	{
		GTRACE(5, "MenuState::handleLogicThreadLoop");

		return true;
	}


	bool MenuState::initialize ()
	{
		GTRACE(5, "MenuState::initialize");

		// Load the resources.
		std::string menuGroup = "Menu"; // TODO: remove hardcoding.

		std::vector<std::string> resList;
		resList.push_back ("data/levels/Beach_1");
		app::SerpentsGameHelper::getPtr ()->addResourceLocationList (resList, menuGroup);
		Ogre::ResourceGroupManager::getSingleton ().initialiseResourceGroup (menuGroup);
		Ogre::ResourceGroupManager::getSingleton ().loadResourceGroup (menuGroup);

		// Use a skybox if required. Loaded from the level.
		std::string sTemp = "SBCloudySkies";//GBaseAppSettings::getPtr ()->level.lvl.skyBoxName;
		double dTemp = 500;//GBaseAppSettings::getPtr ()->level.lvl.skyBoxDist;
		GTRACE (4, "Loading skybox...:" << sTemp);

		if (sTemp != "")
		{
			enginePtr_->getSceneManagerPtr ()->setSkyBox (true, //enable
															sTemp, //material
															dTemp, //dist
															true, // draw first
															Ogre::Quaternion::IDENTITY, // orientation
															menuGroup); // resource group name.
		}

		GTRACE(4, "Setting shadow options...");
		// TODO: add code.

		// 2D stuff.

		Ogre::ResourceGroupManager::getSingleton ().initialiseResourceGroup ("General");
		Ogre::ResourceGroupManager::getSingleton ().loadResourceGroup ("General");

		CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton ();

		CEGUI::System* ceguiSysPtr_ = CEGUI::System::getSingletonPtr ();

#ifdef CEGUI_07
		CEGUI::FrameWindow* myFrame = static_cast <CEGUI::FrameWindow*> (win_mgr.loadWindowLayout("MainMenu.layout"));
		ceguiSysPtr_->setGUISheet (myFrame);

		optionsWindow = static_cast <CEGUI::FrameWindow*> (win_mgr.loadWindowLayout("Options.layout"));
		optionsWindow->setAlwaysOnTop(true);
		myFrame->addChildWindow (optionsWindow);

		CEGUI::Window* opt_video = win_mgr.loadWindowLayout("options_video.layout");
		CEGUI::TabControl* tabCtrl = static_cast <CEGUI::TabControl*> (utils::CEGUIHelperUtil::getWindow ("Tab_Video"));
		if (tabCtrl)
		{
			GTRACE (4, "Adding the video options to the specific tab");
			tabCtrl->addChildWindow (opt_video);
		}

		ceguiSysPtr_->setDefaultMouseCursor("GlossySerpentCursors", "MouseArrow");
	    ceguiSysPtr_->setDefaultTooltip ("GlossySerpent/Tooltip");
#else
		CEGUI::Window* mainMenuSheet  = win_mgr.loadLayoutFromFile ("MainMenu.layout");

		// install this as the root GUI sheet
		ceguiSysPtr_->getDefaultGUIContext ().setRootWindow (mainMenuSheet);
		optionsWindow = (CEGUI::FrameWindow*) win_mgr.loadLayoutFromFile ("Options.layout");
		optionsWindow->setAlwaysOnTop (true);
		mainMenuSheet->addChild (optionsWindow);

#endif
		CEGUI::PushButton* btn;
		btn = static_cast<CEGUI::PushButton*> (utils::CEGUIHelperUtil::getWindow ("ButtonExit"));
		if (btn)
		{
			btn->setWantsMultiClickEvents(false);
			btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onButtonQuitClicked, this));
		}

		btn = static_cast<CEGUI::PushButton*> (utils::CEGUIHelperUtil::getWindow ("ButtonOptions"));
		if (btn)
		{
			btn->setWantsMultiClickEvents(false);
			btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onButtonOptionsClicked, this));
		}

		btn = static_cast<CEGUI::PushButton*> (utils::CEGUIHelperUtil::getWindow ("ButtonQuickGame"));
		if (btn)
		{
			btn->setWantsMultiClickEvents(false);
			btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onButtonNewGameClicked, this));
		}

		btn = static_cast<CEGUI::PushButton*> (utils::CEGUIHelperUtil::getWindow ("BtnOptCancel"));
		if (btn)
		{
			btn->setWantsMultiClickEvents(false);
			btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onButtonOptCancelClicked, this));
		}

		btn = static_cast<CEGUI::PushButton*> (utils::CEGUIHelperUtil::getWindow ("BtnOptOk"));
		if (btn)
		{
			btn->setWantsMultiClickEvents(false);
			btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onButtonOptOkClicked, this));
		}

		CEGUI::Combobox* cbo;
		cbo = static_cast<CEGUI::Combobox*> (utils::CEGUIHelperUtil::getWindow ("VideoDriverBox"));
		if (cbo)
		{
			// Make the combo read-only, so it's only a drop-down list.
			cbo->setReadOnly (true);

			std::string prefRenderer = app::SerpentsGameHelper::getPtr ()->getPreferredRendererName ();
			GTRACE(4, "Will try to set the combo-box to the preferred renderer: " << prefRenderer);
			// Add the entries: the available drivers.
			std::vector <std::string> vidDrivers = app::SerpentsGameHelper::getPtr ()->getEditableAvailableVideoDrivers ();
			for (std::vector <std::string>::iterator it = vidDrivers.begin ();
				it != vidDrivers.end (); ++ it)
			{
				GTRACE (4, "Adding a video driver: " << *it);
				CEGUI::ListboxItem* vidItem = new utils::SimpleListItem ((CEGUI::utf8*) it->c_str ()
								, "GlossySerpent", "ComboboxSelectionBrush", "");
				cbo->addItem (vidItem);
				// Select the current driver.
				if (*it == prefRenderer)
				{
					GTRACE (4, "Found preferred renderer; setting selection to it");
					cbo->setItemSelectState (cbo->getItemCount () - 1, true);
				}
			}
			cbo->handleUpdatedListItemData();
			cbo->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&MenuState::onComboValueChange, this));
		}


		cbo = static_cast<CEGUI::Combobox*> (utils::CEGUIHelperUtil::getWindow ("DisplayModeBox"));
		if (cbo)
		{
			// Make the combo read-only, so it's only a drop-down list.
			cbo->setReadOnly (true);

			app::DisplayModes prefMode = app::SerpentsGameHelper::getPtr ()->getPreferredDisplayMode ();

			// Add the entries
			// For each entry, check to see if it's the active one and select it if so.
			cbo->addItem (new utils::SimpleListItem ((CEGUI::utf8*) app::SerpentsGameHelper::getPtr()->
				getDescriptionForDisplayMode (app::FULLSCREEN_EXCLUSIVE).c_str ()
				, "GlossySerpent", "ComboboxSelectionBrush", ""));
			if (prefMode == app::FULLSCREEN_EXCLUSIVE)
			{
				cbo->setItemSelectState (cbo->getItemCount () - 1, true);
			}

			cbo->addItem (new utils::SimpleListItem ((CEGUI::utf8*) app::SerpentsGameHelper::getPtr()->
				getDescriptionForDisplayMode (app::WINDOWED_WITH_BORDER).c_str ()
				, "GlossySerpent", "ComboboxSelectionBrush", ""));
			if (prefMode == app::WINDOWED_WITH_BORDER)
			{
				cbo->setItemSelectState (cbo->getItemCount () - 1, true);
			}

			if (app::SerpentsGameHelper::getPtr ()->hasFeature ("non-exclusive-fullscreen"))
			{
				cbo->addItem (new utils::SimpleListItem ((CEGUI::utf8*) app::SerpentsGameHelper::getPtr()->
					getDescriptionForDisplayMode (app::WINDOWED_BORDERLESS_FULL_SIZE).c_str ()
					, "GlossySerpent", "ComboboxSelectionBrush", ""));
				if (prefMode == app::WINDOWED_BORDERLESS_FULL_SIZE)
				{
					cbo->setItemSelectState (cbo->getItemCount () - 1, true);
				}
			}
		}

		cbo = static_cast<CEGUI::Combobox*> (utils::CEGUIHelperUtil::getWindow ("ResolutionBox"));
		if (cbo)
		{
			// Make the combo read-only, so it's only a drop-down list.
			cbo->setReadOnly (true);

			// load the list of options and select the current one (or nearest one).
			loadResolutionOptionsForSelectedRenderer ();
		}


		initialized_ = true;
		return true;
	}

	bool MenuState::renderScene ()
	{
		if (! initialized_)
			return initialize ();
		return true;
	}

	bool MenuState::renderGUI ()
	{
		return true;
	}

	void MenuState::notifyOfShutdown ()
	{
	}


	void MenuState::quitApp ()
	{
		exit ();
	}


	void MenuState::newGame ()
	{
	}


	void MenuState::loadResolutionOptionsForSelectedRenderer ()
	{
		CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
		CEGUI::Combobox* cbo;
		cbo = static_cast<CEGUI::Combobox*> (utils::CEGUIHelperUtil::getWindow ("VideoDriverBox"));
		if (cbo)
		{
			std::string currentEngine;
			// Get the items for the selected rendered.
			currentEngine = cbo->getSelectedItem ()->getText ().c_str ();

			cbo = static_cast<CEGUI::Combobox*> (utils::CEGUIHelperUtil::getWindow ("ResolutionBox"));
			if (cbo)
			{
				// clear existing items
				cbo->resetList ();

				// Get the list of resolutions permitted for the selected renderer.
				std::vector <std::string> resOptions =
					app::SerpentsGameHelper::getPtr ()->getEditableResolutionsMapping ()[currentEngine];

				// Get the pre-saved config.
				app::ResolutionConfig resCfg =
					app::SerpentsGameHelper::getPtr ()->getPreferredResolutionConfig ();

				// And calculate the preferred resolution based on the preferred config
				std::string prefResolution =
					app::SerpentsGameHelper::getPtr ()->getUpdatedResolutionString (*resOptions.begin(), resCfg.width, resCfg.height, resCfg.depth);
				GTRACE (4, "Expecting preferred resolution as: " << prefResolution);

				// Add the entries to the list.
				for (std::vector <std::string>::iterator it = resOptions.begin ();
					it != resOptions.end (); ++ it)
				{
					cbo->addItem (new utils::SimpleListItem ((CEGUI::utf8*) it->c_str ()
							, "GlossySerpent", "ComboboxSelectionBrush", ""));

					// Select the current driver.
					if (*it == prefResolution)
					{
						GTRACE (4, "Found preferred resolution; setting selection to it");
						cbo->setItemSelectState (cbo->getItemCount () - 1, true);
					}
				}

				cbo->handleUpdatedListItemData ();
			}
		}
	}

	void MenuState::showOptions (bool onOffFlag)
	{
		CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
		CEGUI::FrameWindow* options = static_cast<CEGUI::FrameWindow*> (utils::CEGUIHelperUtil::getWindow ("OptionsWindow"));

		if (0 == options)
		{
			return;
		}

		if (onOffFlag)
		{
			options->setVisible (true);
			options->setAlwaysOnTop (true);
		}
		else
		{
			options->setVisible (false);
			options->setAlwaysOnTop (false);
		}
	}


	bool MenuState::onButtonQuitClicked (const CEGUI::EventArgs& evt)
	{
		quitApp ();
		return true;
	}

	bool MenuState::onButtonOptCancelClicked (const CEGUI::EventArgs& evt)
	{
		// hide
		showOptions (false);
		return true;
	}

	bool MenuState::onButtonOptOkClicked  (const CEGUI::EventArgs& evt)
	{
		// hide
		showOptions (false);

		app::ResolutionConfig resCfg = app::SerpentsGameHelper::getPtr ()->getPreferredResolutionConfig ();

		// store the options into the helper and into the config.
		CEGUI::Combobox* cbo = static_cast<CEGUI::Combobox*> (utils::CEGUIHelperUtil::getWindow ("VideoDriverBox"));
		if (cbo)
		{
			std::string currentEngine = cbo->getSelectedItem ()->getText ().c_str ();
			std::string storageRenderer = app::SerpentsGameHelper::getPtr ()->getPseudoNameForDriverName (currentEngine);

			enginePtr_->getConfig () ["video"]["preferredEngine"].setAsString (storageRenderer);
			app::SerpentsGameHelper::getPtr ()->setPreferredRendererName (currentEngine);

		}

		cbo = static_cast<CEGUI::Combobox*> (utils::CEGUIHelperUtil::getWindow ("DisplayModeBox"));
		if (cbo)
		{
			int mySelectionIndex = cbo->getItemIndex (cbo->getSelectedItem ());
			app::DisplayModes prefMode = static_cast <app::DisplayModes> (mySelectionIndex);

			enginePtr_->getConfig () ["video"]["displayMode"].setAsInt (mySelectionIndex);
			app::SerpentsGameHelper::getPtr ()->setPreferredDisplayMode (prefMode);
		}

		cbo = static_cast<CEGUI::Combobox*> (utils::CEGUIHelperUtil::getWindow ("ResolutionBox"));
		if (cbo)
		{
			std::string currentRes = cbo->getSelectedItem ()->getText ().c_str ();

			app::ResolutionConfig subConfig = app::SerpentsGameHelper::getPtr ()->getPartialConfigFromResolutionString (currentRes);

			enginePtr_->getConfig () ["video"]["window.width"].setAsInt (subConfig.width);
			enginePtr_->getConfig () ["video"]["window.height"].setAsInt (subConfig.height);
			if (subConfig.depth != 0)
			{
				enginePtr_->getConfig () ["video"]["colourDepth"].setAsInt (subConfig.depth);
				resCfg.depth = subConfig.depth;
			}

			resCfg.width = subConfig.width;
			resCfg.height = subConfig.height;
		}

		app::SerpentsGameHelper::getPtr ()->setPreferredResolutionConfig (resCfg);

		// save.
		GTRACE (3, "Telling the engine to save the config...");
		enginePtr_->getConfig ().save ();

		return true;
	}

	bool MenuState::onButtonNewGameClicked (const CEGUI::EventArgs& evt)
	{
		return true;
	}

	bool MenuState::onButtonOptionsClicked (const CEGUI::EventArgs& evt)
	{
		showOptions (true);
		return true;
	}


	bool MenuState::onComboValueChange (const CEGUI::EventArgs& evt)
	{
		const CEGUI::MouseEventArgs& we =
				static_cast<const CEGUI::MouseEventArgs&>(evt);

		CEGUI::Combobox* cbo = static_cast<CEGUI::Combobox*>(we.window);

		if (cbo == 0)
		{
			return false;
		}

		CEGUI::ListboxItem* item = cbo->getSelectedItem();

		if (item == 0)
		{
			return false;
		}

		if (cbo->getName () == "VideoDriverBox")
		{
			loadResolutionOptionsForSelectedRenderer ();
		}


		return true;
	}

}
