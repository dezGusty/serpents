//    This file is part of Gusty's Serpents
//    Copyright (C) 2009-2012  Augustin Preda (thegusty999@gmail.com)
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
//    -------------------------------------------------------------------------
//
//    The state that handles the display of high-scores.
//
//	  Last change:	$LastChangedDate: 2014-05-31 11:26:56 +0300 (S, 31 mai. 2014) $
//	  Revision:		$Revision: 653 $


#include <impl/HiscoresState.h>
#include <guslib/trace/trace.h>

// String parse operations.
#include <guslib/util/StringUtil.h>

#include <app/SerpEngine.h>

#include "SerpConstants.h"

#include "utils/CEGUIUtils.h"


// TODO: add sound include?

using namespace GBase;
using namespace Ogre;

namespace Serpents
{
	const std::string prefixOfScoreTextElem = "Score_";
	const std::string suffixOfScoreTextElem = "_Label";
	const std::string prefixOfNameTextElem = "Name_";
	const std::string suffixOfNameTextElem = "_Label";

	const std::string nameInputPanel = "NameInputPanel";
	const std::string nameInputField = "NameInputField";

	const int numExpectedScoreTextElems = 10;
	const int maximumNameLength = 100;


	HiscoresState::HiscoresState(const std::string& name, app::SerpEngine *enginePtr, yasper::ptr<Serpents::GScoringTable> scoringTablePtr)
		: SerpState (name, enginePtr)
		, scoringTable (scoringTablePtr)
		, requestNameRead_ (false)
		, initialized_ (false)
		, showedScoreNames_ (false)
	{
	}



	bool HiscoresState::initialize ()
	{
		Ogre::ResourceGroupManager::getSingleton ().initialiseResourceGroup ("General");
		Ogre::ResourceGroupManager::getSingleton ().loadResourceGroup ("General");

		CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();

		CEGUI::System* ceguiSysPtr_ = CEGUI::System::getSingletonPtr();

#ifdef CEGUI_07
		CEGUI::FrameWindow* myFrame= (CEGUI::FrameWindow*) win_mgr.loadWindowLayout("Hiscores.layout");
		ceguiSysPtr_->setGUISheet (myFrame);
		ceguiSysPtr_->setDefaultMouseCursor("GlossySerpentCursors", "MouseArrow");
	    ceguiSysPtr_->setDefaultTooltip("GlossySerpent/Tooltip");
#else
		CEGUI::FrameWindow* myFrame= (CEGUI::FrameWindow*) win_mgr.loadLayoutFromFile("Hiscores.layout");
		ceguiSysPtr_->getDefaultGUIContext ().setRootWindow (myFrame);
		ceguiSysPtr_->getDefaultGUIContext ().getMouseCursor ().setDefaultImage ("GlossySerpentCursors/MouseArrow");
        ceguiSysPtr_->getDefaultGUIContext().setDefaultTooltipType("GlossySerpent/Tooltip");
#endif

		initialized_ = true;
		showingNameInputPanel_ = false;
		return true;
	}


	void HiscoresState::showNameInputPanel ()
	{
		CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
		CEGUI::Window* tempWindow = utils::CEGUIHelperUtil::getWindow (nameInputPanel);
		if (tempWindow)
		{
			tempWindow->show ();
			tempWindow->activate ();
			tempWindow->setEnabled (true);
			tempWindow->setAlwaysOnTop (true);
		}

		CEGUI::Editbox * textItem;
		textItem = static_cast <CEGUI::Editbox *> (utils::CEGUIHelperUtil::getWindow (nameInputField));
		// Prepare the name input panel.
		if (textItem)
		{
			// make sure the edit-box is editable.
			textItem->setReadOnly (false);
			// write the last used text to it and select all of it.
			textItem->setText (scoringTable->getCurrentEntry ()->getName ());
			textItem->setSelection (0, scoringTable->getCurrentEntry ()->getName ().length ());
			// put the text carat at the end.
#if CEGUI_07
			textItem->setCaratIndex (scoringTable->getCurrentEntry ()->getName ().length ());
#else
			textItem->setCaretIndex (scoringTable->getCurrentEntry ()->getName ().length ());
#endif
			// Set the focus on this item.
			textItem->activate ();
		}

		tempWindow = utils::CEGUIHelperUtil::getWindow ("HighscoreListPanel");
		if (tempWindow)
		{
			tempWindow->setEnabled (false);
			tempWindow->setAlpha (0.6f);
		}

		CEGUI::PushButton* btn = static_cast<CEGUI::PushButton*> (utils::CEGUIHelperUtil::getWindow ("ButtonTakeName"));
		if (btn)
		{
			btn->setWantsMultiClickEvents (false);
			btn->subscribeEvent (CEGUI::PushButton::EventClicked,
				CEGUI::Event::Subscriber (&HiscoresState::onButtonOkClicked, this));
		}

		showingNameInputPanel_ = true;
	}


	void HiscoresState::hideNameInputPanelAndStoreNameIfValid ()
	{
		CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
		CEGUI::Editbox * textItem = static_cast <CEGUI::Editbox *> (utils::CEGUIHelperUtil::getWindow  (nameInputField));

		if (textItem)
		{
			// TODO: also call a function to "fix" the string, removing special chars, such as "<", "\", etc.
			CEGUI::String currentlyUsedName (textItem->getText ());

			// First, make sure that a name is entered in the edit-box. If it's empty, refuse to accept the name.
			if (currentlyUsedName.length () > 0)
			{
				scoringTable->getCurrentEntry ()->setName (currentlyUsedName.c_str ());
				CEGUI::Window* tempWindow;

				// Hide the name input panel.
				tempWindow = utils::CEGUIHelperUtil::getWindow (nameInputPanel);
				if (tempWindow)
				{
					tempWindow->hide ();
				}

				tempWindow = utils::CEGUIHelperUtil::getWindow ("HighscoreListPanel");
				if (tempWindow)
				{
					tempWindow->setEnabled (true);
					tempWindow->setAlpha (1.0f);
				}

				CEGUI::PushButton* btn = static_cast<CEGUI::PushButton*> (utils::CEGUIHelperUtil::getWindow ("ButtonTakeName"));
				if (btn)
				{
					btn->removeAllEvents ();
				}
				// Set the focus on this item.
				textItem->activate ();

				showingNameInputPanel_ = false;
			}
		}
	}

	void HiscoresState::showScoreNames ()
	{
		// Load each score.
		try
		{
			std::vector<GScoreEntryImpl> myTable = scoringTable->getTemporaryTable();
			const int thePlayersLocation = scoringTable->getLatestPositionOfCurrent();
			int myNumberOfExpectedItems = numExpectedScoreTextElems;

			if( myNumberOfExpectedItems > (int) myTable.size() )
			{
				myNumberOfExpectedItems = (int) myTable.size();
			}

			GTRACE(4, "Setting the " <<  myNumberOfExpectedItems << "items...");
			CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();

			for( int i=0; i< myNumberOfExpectedItems; i++ )
			{
				std::stringstream ss;
				// The name of the elements will always have a prexif, a suffix and a middle part with a fixed length number
				// This means that we always use names such as "08", "09", "10", to keep the string length.
				ss << prefixOfScoreTextElem << ((i<9)? "0" : "") << (i+1) << suffixOfScoreTextElem;
				std::string itemName = ss.str ();

				CEGUI::Editbox* textItem;
				textItem = static_cast <CEGUI::Editbox *> (utils::CEGUIHelperUtil::getWindow (itemName));

				if (textItem)
				{
					textItem->setText (guslib::stringutil::DoubleToString (myTable[i].get ()->getValue ()));
				}

				ss.str("");
				ss << prefixOfNameTextElem << ((i<9)? "0" : "") << (i+1) << suffixOfNameTextElem;
				itemName = ss.str ();

				textItem = static_cast <CEGUI::Editbox *> (utils::CEGUIHelperUtil::getWindow (itemName));
				if (textItem)
				{
					textItem->setText (myTable[i].get ()->getName ());

					// If this is the player's location, highlight it a bit.
					if (i == thePlayersLocation)
					{
						GTRACE (3, "Updating the target item; found player location: " << thePlayersLocation);

						// If the user is inputing the name, allow editing and make an auto-selection of the name.
						if (requestNameRead_)
						{
							showNameInputPanel ();
						}
					}
				}
			} // end-for

			CEGUI::PushButton* btn = static_cast<CEGUI::PushButton*> (utils::CEGUIHelperUtil::getWindow (CEGUI::String("ButtonBack")));
			if (btn)
			{
				btn->setWantsMultiClickEvents (false);
				btn->subscribeEvent (CEGUI::PushButton::EventClicked,
					CEGUI::Event::Subscriber (&HiscoresState::onButtonBackClicked, this));
			}

			showedScoreNames_ = true;
		}
		catch (Exception& e)
		{
			GTRACE (1, "Exception: "<<e.getFullDescription ());
		}
	}


	void HiscoresState::clearContent ()
	{

	}


	// -------------- overloads

	void HiscoresState::enter(const app::SerpStateParams& param)
	{
		GTRACE(4, "Entering hi-score state...");

		// Set the required flags:
		// initialized_ = false; // TODO: reinit every time?
		guiWasRenderedOnce = false;
		loadingWasDone = false;
		guslib::Configuration stateCfg = param.getConstConfig ();
		requestNameRead_ = stateCfg["general"]["readName"].getAsBoolOrDefault (false);

		GTRACE(4, "Entered game state...");
	}

	void HiscoresState::exit()
	{
		GTRACE(4, "Exiting hi-score state...");
		//hideOverlays();
		//TODO: hide / unload the sheet.
		GTRACE(4, "Completing exit game state...");
	}

	void HiscoresState::pause()
	{
	}
	void HiscoresState::resume()
	{
	}

	void HiscoresState::reactToKeyPress(OIS::KeyCode keycode)
	{
		GTRACE(4, "HiscoresState:reactToKeyPress("<<keycode<<")");
		using namespace OIS;
		switch( keycode )
		{
		case KC_ESCAPE:
			{
				if (requestNameRead_ && showingNameInputPanel_)
				{
					// Player presses ESC, but has not yet entered a name. Don't allow exit.
					return;
				}
				logicEventQueue.push (new GBaseEvent (GET_Exit, ""));
			}
			break;
		case KC_RETURN:
			{
				if (requestNameRead_ && showingNameInputPanel_)
				{
					// Player pressed ENTER, while the window for typing the name is enabled.
					// Check to see if the name put in is not empty. And hide the input window.
					hideNameInputPanelAndStoreNameIfValid ();
				}
			}
			break;
		default:
			{
			}
			break;
		}
	}

	void HiscoresState::reactToKeyRelease(OIS::KeyCode keycode){}

	void HiscoresState::reactToMouseMoved (int x, int y){}

	void HiscoresState::reactToMousePressed (const OIS::MouseButtonID& btn, int x, int y){}

	void HiscoresState::reactToMouseReleased (const OIS::MouseButtonID& btn, int x, int y){}


	bool HiscoresState::handleLogicThreadLoop()
	{
		// handle some pre-rendering actions; this is done only once.
		if( guiWasRenderedOnce && !loadingWasDone )
		{
			loadingWasDone = true;
			//engine->showLoadingScreen(false);
			GTRACE(3, "Initializing game state...");
			if( ! scoringTable )
			{
				logicEventQueue.push( new GBaseEvent(GET_Exit, "") );
			}

			GTRACE(5, "HiscoresState::handleLogicThreadLoop() exiting (done loading).");
			return true;
		}


		// create a sub-scope for any locking operations.
		{
			// handle the events
			if( logicEventQueue.size() == 0 )
			{
				// We have no events.
				GTRACE(5, "HiscoresState: handleLogicThreadLoop (preRender): no events.");
			}
			else
			{
				// We have some events
				GBaseEvent * myEvent = logicEventQueue.front ();
				if (myEvent == NULL)
				{
					GTRACE (5, "WARNING: HiscoresState: preRender, has null event in queue: exiting");
					return false;
				}

				switch (myEvent->getType ())
				{
				case GET_Exit:
					this->enginePtr_->getStateMgr ().popStateNextTick ();
					break;
				default:
					break;
				}
				logicEventQueue.pop();
				delete myEvent;
			}
		}//lock released.

		return true;
	}


	bool HiscoresState::renderScene()
	{
		// This should be a gui-only state
		return true;
	}


	bool HiscoresState::renderGUI()
	{
		if (! initialized_)
		{
			return initialize ();
		}
		if (!showedScoreNames_)
		{
			showScoreNames ();
		}

		GTRACE(6, "HiscoresState::renderGUI() entered");
		// The first pass, just render once (to see the loading screen).
		if( !guiWasRenderedOnce )
		{
			guiWasRenderedOnce = true;
			GTRACE(5, "HiscoresState: render overlay exiting (first pass)");
			return true;
		}

		GTRACE(6, "HiscoresState: render overlay exiting");
		return true;
	}

	void HiscoresState::notifyOfShutdown()
	{
	}

	bool HiscoresState::onButtonOkClicked (const CEGUI::EventArgs& evt)
	{
		if (requestNameRead_ && showingNameInputPanel_)
		{
			hideNameInputPanelAndStoreNameIfValid ();
		}
		else
		{
			GTRACE (1, "Error! This is an unforeseen scenario! Item should not be clickable");
		}
		return true;
	}

	bool HiscoresState::onButtonBackClicked (const CEGUI::EventArgs& evt)
	{
		if (showingNameInputPanel_)
		{
			return false;
		}

		GTRACE (4, "Back button clicked. Exiting hi-score state.");
		logicEventQueue.push (new GBaseEvent (GET_Exit, ""));
		return true;
	}

}
