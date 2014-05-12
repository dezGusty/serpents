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

#include "GInGameMenuState.h"
#include "SerpConstants.h"

#include <ogreoverlay.h>
#include <OgrePanelOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>

#include <guslib/trace/trace.h>
#include "GAudiere.h"

using namespace GAudiere;
using namespace GBase;
using namespace Ogre;
using namespace guslib;

namespace Serpents
{
	GInGameMenuState * GInGameMenuState::objectPtr=NULL;
#ifdef G_USE_MULTITHREAD
	guslib::GMutex GInGameMenuState::creationMutex;
#endif

	GInGameMenuState * GInGameMenuState::getInstancePtr(GBaseEngine *enginePtr)
	{
		if( !objectPtr )
		{
		#ifdef G_USE_MULTITHREAD
			guslib::GScopedLock myLock( creationMutex );
		#endif
			//recheck to see if the object was not constructed while trying to obtain the lock.
			if( !objectPtr )
			{
				volatile GInGameMenuState * tempPtr = new GInGameMenuState(enginePtr);
				objectPtr = (GInGameMenuState*)tempPtr;
			}
		}
		return objectPtr;
	}


	void GInGameMenuState::deleteInstance()
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



	GInGameMenuState::GInGameMenuState(GBaseEngine *enginePtr)
		: GBaseState("ingamemenu")
		, engine(enginePtr)
		, menu( NULL )
		, overlay( NULL )
	{
		GTRACE(3, "GInGameMenuState ctor.");
		GTRACE(4, "GInGameMenuState loading overlays.");
		loadStateOverlays();
		GTRACE(4, "GInGameMenuState loaded ov.");
		// create the menu
		GTRACE(4, "GInGameMenuState calling menu creation.");
		menu = createGameMenu();
	}


	GBaseMenu * GInGameMenuState::createGameMenu()
	{
		GTRACE(4, "GInGameMenuState::createGameMenu()");
		GBaseMenu * myMenu = new GBase::GBaseMenu();

		myMenu->loadFromXML("data/ingamemenu.xml", "");

		using namespace OIS;
		myMenu->clearKeyMap();
		myMenu->setKeyMap(guslib::cmdActionEsc, KC_ESCAPE);
		myMenu->setKeyMap(guslib::cmdActionIn, KC_RETURN);
		myMenu->setKeyMap(guslib::cmdActionLeft, KC_LEFT);
		myMenu->setKeyMap(guslib::cmdActionRight, KC_RIGHT);
		myMenu->setKeyMap(guslib::cmdMenuDown, KC_DOWN);
		myMenu->setKeyMap(guslib::cmdMenuUp, KC_UP);

		myMenu->setListener( this );
		return myMenu;
	}


	void GInGameMenuState::loadStateOverlays()
	{
		overlay = OverlayManager::getSingletonPtr()->getByName(c_inGameMenuOverlay);
	}


	void GInGameMenuState::showStateOverlays( bool show )
	{
		if( !overlay )
			loadStateOverlays();
		if( overlay )
		{
			if( show )
				overlay->show();
			else
				overlay->hide();
		}
	}


	void GInGameMenuState::clearStateLogic()
	{
		while( ! logicEventQueue.empty() )
		{
			logicEventQueue.pop();
		}
		while( ! menuEventQueue.empty() )
		{
			menuEventQueue.pop();
		}
	}



	// ------------ state management

	void GInGameMenuState::enter(const GUtils::GAny & param)
	{
		GTRACE(4, "Entering in-game-menu state");
		// Make sure there are no left over events.
		showStateOverlays();

		// Since the menu is only created once, it will retain its state, including the selection.
		// Need to reset that.

		try
		{
			menu->getActive()->setSelectedChild((int)0);
		}
		catch( std::exception & e )
		{
			GTRACE(1, "Exception encountered!" << e.what());
		}
		this->OnMenuShouldRedraw( this->menu );
	}


	void GInGameMenuState::exit()
	{
		clearStateLogic();
		showStateOverlays( false );
	}


	// This is the pause... You can't pause the pause... nor resume it. :)
	void GInGameMenuState::pause() {}
	void GInGameMenuState::resume() {}


	void GInGameMenuState::reactToKeyPress(OIS::KeyCode keycode)
	{
		GTRACE(3, "GMenuState::reactToKeyPress: "<<keycode);
		if( menu )
		{
			menu->reactToKeyPress(keycode);
		}
	}


	void GInGameMenuState::reactToKeyRelease(OIS::KeyCode keycode){}


	bool GInGameMenuState::handleLogicThreadLoop()
	{
		GTRACE(5, "GInGameMenuState::handleLogicThreadLoop() entered; getting logicEventProcessMutex");
		// lock the menu event queue
		guslib::GScopedLock myOtherLock(logicEventProcessMutex);
		GTRACE(5, "GInGameMenuState::handleLogicThreadLoop() locked logicEventProcessMutex");
		if( logicEventQueue.size() == 0 )
		{
			// We have no events.
			GTRACE(5, "GInGameMenuState: preRender, but no events: exiting");
			return true;
		}

		// We have some events
		GBaseEvent * myEvent = logicEventQueue.front();
		if( myEvent == NULL )
		{
			GTRACE(5, "WARNING: GInGameMenuState: preRender, has null event in queue: exiting");
			return false;
		}

		switch( myEvent->getType() )
		{
		case GET_Exit:
			GBaseStateManager::getPtr()->popStateNextTick();
			break;
		case GET_DualStateBack:
			GBaseStateManager::getPtr()->popStateNextTick();
			GBaseStateManager::getPtr()->popStateNextTick();
			break;
		default:
			break;
		}
		logicEventQueue.pop();
		delete myEvent;
		GTRACE(5, "GInGameMenuState::handleLogicThreadLoop() exiting");
		return true;	
	}



	bool GInGameMenuState::renderScene()
	{
		return true;
	}



	bool GInGameMenuState::renderGUI()
	{
		// lock the menu event queue
		guslib::GScopedLock myOtherLock(menuEventProcessMutex);

		if( menuEventQueue.size() == 0 )
		{
			// We have no events.
			GTRACE(6, "GInGameMenuState: render overlay, but no events: exiting");
			return true;
		}

		GTRACE(5, "GInGameMenuState::renderGUI() entered (with events)");

		// We have some events
		GBaseEvent * myEvent = menuEventQueue.front();
		if( myEvent == NULL )
		{
			GTRACE(5, "WARNING: GInGameMenuState: render overlay, has null event in queue: exiting");
			return false;
		}

		if( myEvent->getDescription() == this->menu->getRoot()->getName() )
		{
			// The event is a menu event, related to this menu.
			// Currently, the only event of importance to the render is the redraw requirement.
			// The event contains no additional information atm, so it can be removed.
			menuEventQueue.pop();

			// do the redrawing of the menu.
			if( ! overlay )
				return false;
		}
		else
		{
			return false;
		}

		GTRACE(5, "GInGameMenuState::renderGUI() preparing to draw");

		GMenuItemList vect = menu->getVisible()->getChildren();
		for(GMenuItemList::iterator it = vect.begin(); it!= vect.end(); ++it)
		{
			GTRACE(5, "GInGameMenuState::renderGUI() preparing to draw item="<<(*it)->getName());
			if( (*it)->getAuxData() != NULL )
			{
				GTRACE(5, "GInGameMenuState::renderGUI() item has aux data");

				GAuxQuadStringInfo * tempPtr = (GAuxQuadStringInfo * )(*it)->getAuxData();
				// name of btn container
				try
				{
					if( tempPtr->getInfo() != "" )
					{
						OverlayElement * elem = OverlayManager::getSingleton().getOverlayElement(tempPtr->getInfo());
						if( elem != NULL )
						{
							elem->show();
							GTRACE(5, "GInGameMenuState::renderGUI() show elem="<<elem->getName());

							if( (*it)->isSelected() )
							{
								elem->setMaterialName(tempPtr->getInfo3());
							}
							else
							{
								elem->setMaterialName(tempPtr->getInfo2());
							}
						}
					}
					else
					{
						GTRACE(5, "info is empty");
					}
					if( tempPtr->getInfo4() != "" )
					{
						OverlayElement * elem;
						// set the text color
						//elem = overlay->getChild( tempPtr->getInfo4() );
						elem = OverlayManager::getSingleton().getOverlayElement(tempPtr->getInfo4());
						if( elem != NULL )
						{
							static const Ogre::ColourValue activeSelectionColorTop(1.0, 1.0, 0.5, 1.0);
							static const Ogre::ColourValue activeSelectionColorBtm(1, 0, 0, 1.0);
							static const Ogre::ColourValue inactiveSelectionColorTop(0.90, 0.76, 0.56, 0.7);
							static const Ogre::ColourValue inactiveSelectionColorBtm(0.85, 0.44, 0.26, 0.7);
							static const Ogre::ColourValue editSelectionColorTop(1.0, 0.0, 0.0, 1.0);
							static const Ogre::ColourValue editSelectionColorBtm(0.1, 0.0, 0.0, 1.0);

							((TextAreaOverlayElement*)elem)->setCaption( (*it)->getCaption() );
							if( (*it)->isSelected() )
							{
								if( (*it) == menu->getActive() )
								{
									// This is a special case for options.
									((TextAreaOverlayElement*)elem)->setColourTop( editSelectionColorTop );
									((TextAreaOverlayElement*)elem)->setColourBottom( editSelectionColorBtm );
								}
								else
								{
									((TextAreaOverlayElement*)elem)->setColourTop( activeSelectionColorTop );
									((TextAreaOverlayElement*)elem)->setColourBottom( activeSelectionColorBtm );
								}
							}
							else
							{
								((TextAreaOverlayElement*)elem)->setColourTop( inactiveSelectionColorTop );
								((TextAreaOverlayElement*)elem)->setColourBottom( inactiveSelectionColorBtm );
							}
						}
					}
					else
					{
						GTRACE(5, "info4 is empty");
					}

				}
				catch(...){}
			}
		}
		// end of menu draw
		return true;
	}


	void GInGameMenuState::notifyOfShutdown()
	{
	}


	void GInGameMenuState::OnMenuAction( GMenuItem * item )
	{
		GTRACE(3, "GInGameMenuState::OnMenuAction:"<<item->getName());
		GSOUND_PLAY( GUS_SOUND_MENU_SELECTION_CLICKED );

		if( item->getName() == "back" )
		{
			guslib::GScopedLock myOtherLock(logicEventProcessMutex);
			logicEventQueue.push( new GBaseEvent(GET_Exit, menu->getRoot()->getName()) );
		}
		else if( item->getName() == "quit" )
		{
			guslib::GScopedLock myOtherLock(logicEventProcessMutex);
			logicEventQueue.push( new GBaseEvent(GET_DualStateBack, menu->getRoot()->getName()) );
		}
	}


	bool GInGameMenuState::OnMenuBack( GMenuItem * item )
	{
		GTRACE(5, "OnMenuBack");
		GSOUND_PLAY( GUS_SOUND_MENU_SELECTION_CHANGED );
		return true;
	}


	void GInGameMenuState::OnMenuDepthChange( GMenu * menu, bool fromTop )
	{
		if( menu == NULL )
		{
			// treat it as an exit command.
			// This will come when the "ESC" command is issued on the up-most level.

			guslib::GScopedLock myOtherLock(logicEventProcessMutex);
			logicEventQueue.push( new GBaseEvent(GET_Exit, "") );
			return;
		}
		GTRACE(5, "OnMenuDepthChange; active="<<menu->getActive()->getName());
		GSOUND_PLAY( GUS_SOUND_MENU_SELECTION_CLICKED );
	}


	void GInGameMenuState::OnMenuSelectionChange( GMenuItem *item )
	{
		GTRACE(5, "OnMenuSelectionChange, active="<<item->getName()<<";"<<item->getSelectedChild()->getName());
		GSOUND_PLAY( GUS_SOUND_MENU_SELECTION_CHANGED );
	}

	// The in-game menu should be simple for the time being at least.
	void GInGameMenuState::OnMenuOptionStartEdit( GMenuItem *optionGroup, GMenuItem *option ){}
	void GInGameMenuState::OnMenuOptionStopEdit( GMenuItem *optionGroup, GMenuItem *option, bool bCanceled ){}
	void GInGameMenuState::OnMenuOptionValueChange( GMenuItem *optionGroup, GMenuItem *option ){}


	void GInGameMenuState::OnMenuShouldRedraw( GMenu * menu )
	{
		GTRACE(5, "GInGameMenuState::OnMenuShouldRedraw");
		// lock the menu event queue
		guslib::GScopedLock myOtherLock(menuEventProcessMutex);

		menuEventQueue.push( new GBaseEvent(GET_MenuShouldRedraw, menu->getRoot()->getName()) );
	}


}