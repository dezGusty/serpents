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
#if 0
#include <serp/GMenuState.h>

#include <app/SerpEngine.h>

//Ogre includes
#include <ogreoverlay.h>
#include <OgrePanelOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>
#include "SerpConstants.h"
#include "GBaseSetting.h"

#include "GBaseMainApp.h"
#include "GBaseEngineDefinitions.h"

#include <guslib/trace/trace.h>
#include <guslib/system/random.h>
#include <guslib/system/apprestart.h>

#include <app/SerpentsGameHelper.h>

//TODO: reactivate sound?
//#include "GAudiere.h"
//using namespace GAudiere;

using namespace GBase;
using namespace Ogre;
using namespace guslib;

namespace Serpents
{
	// utility functions.
	std::string getShortRenderDeviceNameFromLongName( std::string input )
	{
		if( input == G_RENDER_ENGINE_D3D_LONG_NAME )
			return G_RENDER_ENGINE_D3D_SHORT_NAME;
		if( input == G_RENDER_ENGINE_OGL_LONG_NAME )
			return G_RENDER_ENGINE_OGL_SHORT_NAME;
		return "";	
	}

	std::string getLongRenderDeviceNameFromShortName( std::string input )
	{
		if( input == G_RENDER_ENGINE_D3D_SHORT_NAME )
			return G_RENDER_ENGINE_D3D_LONG_NAME;
		if( input == G_RENDER_ENGINE_OGL_SHORT_NAME )
			return G_RENDER_ENGINE_OGL_LONG_NAME;
		return "";		
	}


	//end utility funcs



	GMenuState::GMenuState (const std::string& name, app::SerpEngine *enginePtr, yasper::ptr<Serpents::GScoringTable> scoringTablePtr)
		: SerpState (name, enginePtr)
		, scoringTable (scoringTablePtr)
		, engine (enginePtr)
		//, menu (NULL)
	{
		loadSounds();
		loadOverlays();
		scoringTable = createScoringTable();

		//// create the menu
		//if( menu == NULL )
		//{
		//	GTRACE(3, "Base Engine calling menu creation.");
		//	menu = createGameMenu();
		//}
		GTRACE(3, "Base Engine calling menu content load.");
		//loadMenuContents();	
	}


	GScoringTable * GMenuState::createScoringTable ()
	{
		GScoringTable * tempTable = new GScoringTable();
		return tempTable;
	}


	//GBaseMenu * GMenuState::createGameMenu()
	//{
	//	GBaseMenu * myMenu = new GBase::GBaseMenu();

	//	myMenu->loadFromXML("data/menu.xml", "");

	//	using namespace OIS;
	//	myMenu->clearKeyMap();
	//	myMenu->setKeyMap(guslib::cmdActionEsc, KC_ESCAPE);
	//	myMenu->setKeyMap(guslib::cmdActionIn, KC_RETURN);
	//	myMenu->setKeyMap(guslib::cmdActionLeft, KC_LEFT);
	//	myMenu->setKeyMap(guslib::cmdActionRight, KC_RIGHT);
	//	myMenu->setKeyMap(guslib::cmdMenuDown, KC_DOWN);
	//	myMenu->setKeyMap(guslib::cmdMenuUp, KC_UP);

	//	myMenu->setListener( this );
	//	return myMenu;
	//}


	//void GMenuState::loadMenuContents()
	//{
	//	if( menu == NULL )
	//	{
	//		GTRACE(1, "ERROR! Loading menu contents in NULL menu!");
	//		return;
	//	}

	//	// Set some non-video mode settings.

	//	GMenuItem *tempItem;
	//	tempItem = menu->searchForItemByName(ME_MENU_SOUNDMASTER);
	//	if( tempItem )
	//	{
	//		std::stringstream gss;
	//		gss<<GBaseAppSettings::getPtr ()->setting.sound.masterVolume;
	//		std::string sVol = gss.str();
	//		GMenuItem *auxItem = menu->searchForItemByName(sVol, tempItem);
	//		if( auxItem )
	//		{
	//			tempItem->setSelectedChild( auxItem );
	//		}
	//	}
	//	tempItem = menu->searchForItemByName(ME_MENU_SOUNDMUSIC);
	//	if( tempItem )
	//	{
	//		std::stringstream gss;
	//		gss<<GBaseAppSettings::getPtr ()->setting.sound.musicVolume;
	//		std::string sVol = gss.str();
	//		GMenuItem *auxItem = menu->searchForItemByName(sVol, tempItem);
	//		if( auxItem )
	//		{
	//			tempItem->setSelectedChild( auxItem );
	//		}
	//	}
	//	tempItem = menu->searchForItemByName(ME_MENU_SOUNDVOICE);
	//	if( tempItem )
	//	{
	//		std::stringstream gss;
	//		gss<<GBaseAppSettings::getPtr ()->setting.sound.voiceVolume;
	//		std::string sVol = gss.str();
	//		GMenuItem *auxItem = menu->searchForItemByName(sVol, tempItem);
	//		if( auxItem )
	//		{
	//			tempItem->setSelectedChild( auxItem );
	//		}
	//	}
	//	tempItem = menu->searchForItemByName(ME_MENU_SOUNDFX);
	//	if( tempItem )
	//	{
	//		std::stringstream gss;
	//		gss<<GBaseAppSettings::getPtr ()->setting.sound.fxVolume;
	//		std::string sVol = gss.str();
	//		GMenuItem *auxItem = menu->searchForItemByName(sVol, tempItem);
	//		if( auxItem )
	//		{
	//			tempItem->setSelectedChild( auxItem );
	//		}
	//	}

	//	tempItem = menu->searchForItemByName(ME_MENU_MESHQUAL);
	//	if( tempItem )
	//	{
	//		std::string sItem = GBaseAppSettings::getPtr ()->setting.perf.meshQuality==0 ? "low" : "high";
	//		GMenuItem *auxItem = menu->searchForItemByName(sItem, tempItem);
	//		if( auxItem )
	//		{
	//			tempItem->setSelectedChild( auxItem );
	//		}
	//	}
	//	tempItem = menu->searchForItemByName(ME_MENU_SHADOWS);
	//	if( tempItem )
	//	{
	//		std::string sItem = GBaseAppSettings::getPtr ()->setting.perf.useShadows ? "on" : "off";
	//		GMenuItem *auxItem = menu->searchForItemByName(sItem, tempItem);
	//		if( auxItem )
	//		{
	//			tempItem->setSelectedChild( auxItem );
	//		}
	//	}
	//	tempItem = menu->searchForItemByName(ME_MENU_POSTPROCESSING);
	//	if( tempItem )
	//	{
	//		std::string sItem = GBaseAppSettings::getPtr ()->setting.perf.postProcessing ? "on" : "off";
	//		GMenuItem *auxItem = menu->searchForItemByName(sItem, tempItem);
	//		if( auxItem )
	//		{
	//			tempItem->setSelectedChild( auxItem );
	//		}
	//	}
	//	tempItem = menu->searchForItemByName(ME_MENU_CPUSLEEP);
	//	if( tempItem )
	//	{
	//		std::stringstream gss;
	//		gss<<GBaseAppSettings::getPtr ()->setting.perf.cpuSleep;
	//		std::string sItem = gss.str();
	//		GMenuItem *auxItem = menu->searchForItemByName(sItem, tempItem);
	//		if( auxItem )
	//		{
	//			tempItem->setSelectedChild( auxItem );
	//		}
	//	}

	//	// graphical settings
	//	tempItem = menu->searchForItemByName(ME_MENU_RENDERING_ENGINE);
	//	if( tempItem )
	//	{
	//		int w, h;
	//		std::string enginestr = app::SerpentsGameHelper::get().getPreferredRendererName ();
	//		app::SerpentsGameHelper::get ().getSizesFromResolutionString (enginestr, w, h);
	//		enginestr = getShortRenderDeviceNameFromLongName(enginestr);
	//		GTRACE(3, "trying to set the selected engine to "<<enginestr);
	//		GMenuItem *auxItem = menu->searchForItemByName(enginestr, tempItem);
	//		if( auxItem )
	//		{
	//			tempItem->setSelectedChild( auxItem );
	//			GTRACE(3, "set the selected child to "<<enginestr);
	//		}
	//	}
	//}


	//void GMenuState::loadOverlays()
	//{
	//	Ogre::Overlay * overlay;
	//	overlay = OverlayManager::getSingletonPtr()->getByName(c_mainMenuOverlay);
	//}


	//void GMenuState::showOverlays()
	//{
	//	try
	//	{
	//		Ogre::Overlay * overlay= OverlayManager::getSingletonPtr()->getByName(c_mainMenuOverlay);
	//		if(!overlay)
	//		{
	//			GTRACE(2, "*** GMenuState can't show overlay, it is NULL!");
	//			return;
	//		}
	//		overlay->show();
	//		std::string gsCopyrightNotice = std::string( GUS_COPYRIGHT_NOTICE ) + std::string( GUS_APP_VERSION );
	//		OverlayManager::getSingleton().getOverlayElement("GS/TAMenuCredits")->
	//			setCaption(gsCopyrightNotice);
	//		std::string webSiteText = std::string( G_WEBSITE_ADDRESS );
	//		OverlayManager::getSingleton().getOverlayElement(c_webSiteAddressItem)->
	//			setCaption(webSiteText);

	//		std::string musicText = GBaseAppSettings::getPtr ()->music.copyrightNotice;
	//		OverlayManager::getSingleton().getOverlayElement(OE_MENU_MUSIC_NOTICE)->setCaption( musicText );
	//	}
	//	catch( Exception & e )
	//	{
	//		GTRACE(1, "Exception caught in GMenuState::showOverlays(): "<<e.getFullDescription());
	//	}
	//}

	void GMenuState::hideOverlays()
	{
		Ogre::Overlay * overlay;
		overlay = OverlayManager::getSingletonPtr()->getByName(c_mainMenuOverlay);
		overlay->hide();
	}


	void GMenuState::loadSounds()
	{
		// loaded by loader state from engine
	}

	const std::string GMenuState::addResolutionOptionsAndReturnSelected()
	{
		std::string sRenderEngineItem;
		GMenuItem * myItem;

		myItem = menu->searchForItemByName(ME_MENU_RENDERING_ENGINE);
		if( myItem == NULL )
		{
			GTRACE(1, "Unable to set resolution options. Unable to find item "<<ME_MENU_RENDERING_ENGINE);
			return "";
		}
		myItem = myItem->getSelectedChild();
		if( myItem == NULL )
		{
			GTRACE(1, "Unable to set resolution options. No options for "<<ME_MENU_RENDERING_ENGINE);
		}
		// The rendering engine item must contain the CURRENT settings. This is because the user can select
		// a different rendering engine withouth hitting the apply button.
		sRenderEngineItem = getLongRenderDeviceNameFromShortName(myItem->getName());
		GTRACE(3, "Currently selected render engine item = "<<sRenderEngineItem);
		myItem = menu->searchForItemByName(ME_MENU_RESOLUTION);
		if( myItem == NULL )
		{
			GTRACE(1, "Unable to set resolution options. Unable to find item "<<ME_MENU_RENDERING_ENGINE);
			return "";
		}
		myItem->removeAllChildren();

		std::vector<std::string> options = app::SerpentsGameHelper::get ().getEditableResolutionsMapping()[sRenderEngineItem];
		int selectionIndex;
		std::string selection; // this will not actually get used.
		
		//GBaseMainApp::getResolutionOptionsForEngine(sRenderEngineItem, &options, &selectionIndex, &selection);
		for( std::vector<std::string>::iterator it = options.begin(); it!= options.end(); ++it )
		{
			GTRACE(4, "Adding child "<< *it << " to: "<<myItem->getName() );
			GMenuItem *tempItem = new GMenuItem(menu, *it, *it); // using same value for name and caption.
			myItem->addChild(tempItem);
		}
		myItem->setSelectedChild(selectionIndex);

		selection = myItem->getCaption();

		GTRACE(3, "options.size="<<options.size());
		if( options.size() < 1 )
		{
			if( GBaseAppSettings::getPtr ()->setting.videoTemp.renderEngine == Direct3D )
			{
				OverlayElement * elem = OverlayManager::getSingleton().getOverlayElement("taVidModeInfo");
				if( elem )
					((TextAreaOverlayElement*)elem)->setCaption( "Unable to use Direct3D! Make sure you have the latest version!" );
			}
			else// OpenGL mode.
			{
				OverlayElement * elem = OverlayManager::getSingleton().getOverlayElement("taVidModeInfo");
				if( elem )
					((TextAreaOverlayElement*)elem)->setCaption( "OpenGL is no longer supported by this game!" );
			}
		}
		else
		{
				OverlayElement * elem = OverlayManager::getSingleton().getOverlayElement("taVidModeInfo");
				if( elem )
					((TextAreaOverlayElement*)elem)->setCaption( "" );
		}
		GTRACE(3, "Added res opts");
		return selection;
	}


	void GMenuState::hideAllMenuPages()
	{
		Ogre::Overlay * overlay;
		overlay = OverlayManager::getSingletonPtr()->getByName(c_mainMenuOverlay);
		if(!overlay)
		{
			GTRACE(2, "*** Unable to hide all menu pages, overlay is NULL!");
			return;
		}
		OverlayElement * elem;
		elem = overlay->getChild(c_mainMenuPanel);
		if( elem )
			elem->hide();
		elem = overlay->getChild(c_optionsPanel);
		if( elem )
			elem->hide();
		elem = overlay->getChild(c_mapSelectionPanel);
		if( elem )
			elem->hide();
		elem = overlay->getChild(c_graphicsOptionsPanel);
		if( elem )
			elem->hide();
		elem = overlay->getChild(c_videoModeOptionsPanel);
		if( elem )
			elem->hide();
		elem = overlay->getChild(c_soundOptionsPanel);
		if( elem )
			elem->hide();
	}



	// -------------------- overloads


	void GMenuState::enter (const app::SerpStateParams& param)
	{
		GTRACE(4, "Entering main menu state...");

		// TODO: reactivate sound.
		//GTRACE(2, "Starting music play...");
		//GSOUND_START_MUSIC();
		//GSOUND_SWITCH_TO_PLAYLIST(GUS_MENU_PLAYLIST);

		showOverlays();
		OnMenuShouldRedraw(menu);
	}

	void GMenuState::exit()
	{
		GTRACE(4, "Exiting menu state...");
	}

	void GMenuState::pause() 
	{
		GTRACE(4, "Pausing menu state");

		// make sure that rendering is not active.
		//GTRACE(6, "GMenuState::pause getting load and render mutex");
		//guslib::GScopedLock myLock( engine->getLoadAndRenderMutex() );
		//GTRACE(6, "GMenuState::pause got load and render mutex");
		GTRACE(4, "Hiding main menu pages...");
		hideAllMenuPages();
		GTRACE(4, "Hiding main menu overlay...");
		hideOverlays();
		GTRACE(6, "GMenuState::pause releasing load and render mutex");
	}

	void GMenuState::resume()
	{
		GTRACE(4, "Resuming menu state");

		// TODO: reactivate sound.
		//GSOUND_SWITCH_TO_PLAYLIST(GUS_MENU_PLAYLIST);

		// make sure that rendering is not active.
		//GTRACE(6, "GMenuState::resume getting load and render mutex");
		//guslib::GScopedLock myLock( engine->getLoadAndRenderMutex() );
		//GTRACE(6, "GMenuState::resume got load and render mutex");
		GTRACE(5, "Showing main menu overlay...");
		showOverlays();

		GTRACE(5, "Redrawing menu due to state change...");
		OnMenuShouldRedraw(menu);
		GTRACE(4, "Menu state resumed");
		GTRACE(6, "GMenuState::resume releasing load and render mutex");
	}

	void GMenuState::reactToKeyPress(OIS::KeyCode keycode)
	{
		if( menu )
		{
			menu->reactToKeyPress(keycode);
		}
	}

	void GMenuState::reactToKeyRelease(OIS::KeyCode keycode)
	{
	}


	void GMenuState::reactToMouseMoved (int x, int y){}

	void GMenuState::reactToMousePressed (const OIS::MouseButtonID& btn, int x, int y){}

	void GMenuState::reactToMouseReleased (const OIS::MouseButtonID& btn, int x, int y){}

	bool GMenuState::handleLogicThreadLoop()
	{
		GTRACE(6, "GMenuState::handleLogicThreadLoop() entered; getting logicEventProcessMutex");
		// lock the menu event queue
		//guslib::GScopedLock myOtherLock(logicEventProcessMutex);
		GTRACE(6, "GMenuState::handleLogicThreadLoop() locked logicEventProcessMutex");
		if( logicEventQueue.size() == 0 )
		{
			// We have no events.
			GTRACE(5, "GMenuState: preRender, but no events: exiting");
			return true;
		}

		// We have some events
		GBaseEvent * myEvent = logicEventQueue.front();
		if( myEvent == NULL )
		{
			GTRACE(5, "WARNING: GMenuState: preRender, has null event in queue: exiting");
			return false;
		}

		switch( myEvent->getType() )
		{
		case GET_Exit:
			enginePtr_->getStateMgr ().popStateNextTick ();
			break;
		case GET_Reboot:
			// user settings were saved; we can restart the application.
			guslib::AppRestarter::getPtr ()->restart ("");

			// now close the current instance.
			enginePtr_->getStateMgr ().popStateNextTick ();
			break;
		case GET_Quick:
			{
				// get a random level.
				int itemNo = (int)guslib::Random::getPtr()->getValueUpTo
					((long) GBaseAppSettings::getPtr ()->level.defs.size() );
				std::string fileOfLevel = std::string("data/levels/") + GBaseAppSettings::getPtr ()->level.defs[itemNo].fileOfLevel;
				guslib::Configuration cfgLevel;
				cfgLevel["general"]["fileOfLevel"].setAsString (fileOfLevel);
				enginePtr_->getStateMgr ().pushStateNextTick ("game", cfgLevel);
			}
			break;
		case GET_NormGame:
			{
				//TODO:reenable
				std::string fileOfLevel = std::string("");
				for(std::vector<GLevelDefinition>::iterator it= GBaseAppSettings::getPtr ()->level.defs.begin();
					it != GBaseAppSettings::getPtr ()->level.defs.end(); ++it )
				{
					if( (std::string)it->id == (std::string)myEvent->getDescription() )
					{
						fileOfLevel = it->fileOfLevel;
						break;
					}
				}

				if( fileOfLevel != std::string("") )
				{
					std::string actualFile = std::string("data/levels/") + fileOfLevel;
					guslib::Configuration cfgLevel;
					cfgLevel["general"]["fileOfLevel"].setAsString (actualFile);
					enginePtr_->getStateMgr ().pushStateNextTick ("game", cfgLevel);
				}
			}
			break;
		case GET_ViewHiscore:
			{
				std::string folderName = std::string("");
				for(std::vector<GLevelDefinition>::iterator it= GBaseAppSettings::getPtr ()->level.defs.begin();
					it != GBaseAppSettings::getPtr ()->level.defs.end(); ++it )
				{
					if( (std::string)it->hi == (std::string)myEvent->getDescription() )
					{
						folderName = it->folder;
						break;
					}
				}

				if( folderName != std::string("") )
				{
					std::string hiScoreFile = std::string("data/levels/")
						+ folderName
						+ std::string("/records.txt");
					scoringTable->loadFromFile( hiScoreFile );
					guslib::Configuration cfgHiscore;
					cfgHiscore["general"]["readName"].setAsBool (false);
					enginePtr_->getStateMgr ().pushStateNextTick ("hiscore", cfgHiscore);
				}
			}
		default:
			break;
		}
		GTRACE(5, "GMenuState::handleLogicThreadLoop() popping event q");
		logicEventQueue.pop();
		delete myEvent;
		GTRACE(5, "GMenuState::handleLogicThreadLoop() exiting (some events were popped).");
		return true;
	}

	bool GMenuState::renderScene()
	{
		return true;
	}

	bool GMenuState::renderGUI()
	{
		GTRACE(5, "GMenuState::renderGUI() entered");
		// lock the menu event queue
		//guslib::GScopedLock myOtherLock(menuEventProcessMutex);

		if( menuEventQueue.size() == 0 )
		{
			// We have no events.
			GTRACE(5, "GBaseEngine: render overlay, but no events: exiting");
			return true;
		}

		// We have some events
		GBaseEvent * myEvent = menuEventQueue.front();
		if( myEvent == NULL )
		{
			GTRACE(5, "WARNING: GBaseEngine: render overlay, has null event in queue: exiting");
			return false;
		}

		if( myEvent->getDescription() == this->menu->getRoot()->getName() )
		{
			// The event is a menu event.
			// Currently, the only event of importance to the render is the redraw requirement.
			// The event contains no additional information atm, so it can be removed.
			menuEventQueue.pop();
			delete myEvent;

			Ogre::Overlay * overlay;
			overlay = OverlayManager::getSingletonPtr()->getByName(c_mainMenuOverlay);
			// do the redrawing of the menu.
			if( ! overlay )
				return false;
			if( !overlay->isVisible() )
				overlay->show();

			Ogre::OverlayElement * activePage;

			hideAllMenuPages();

			// Try to show the active page.
			try
			{
				std::string sName = menu->getVisible()->getName();
				if( sName == "root" )
				{
					activePage = overlay->getChild(c_mainMenuPanel);
					if( activePage )
						activePage->show();
					else return false;
				}
				else if( sName == "game" )
				{
					activePage = overlay->getChild(c_mapSelectionPanel);
					if( activePage )
						activePage->show();
					else return false;
				}
				else if( sName == "options" )
				{
					activePage = overlay->getChild(c_optionsPanel);
					if( activePage )
						activePage->show();
					else return false;
				}
				else if( sName == "graphdetail" )
				{
					activePage = overlay->getChild(c_graphicsOptionsPanel);
					if( activePage )
						activePage->show();
					else return false;
				}
				else if( sName == "vidmode" )
				{
					activePage = overlay->getChild(c_videoModeOptionsPanel);
					if( activePage )
						activePage->show();
					else return false;
				}
				else if( sName == "sound" )
				{
					activePage = overlay->getChild(c_soundOptionsPanel);
					if( activePage )
						activePage->show();
					else return false;
				}
			}
			catch(...)
			{
				GTRACE(2, "OnMenuShouldRedraw: Some exception was triggered on menu draw");
			}

			//TODO: use fading / black transition screen?
								//static const Ogre::ColourValue transpColour(1.0, .0, 0.0, 0.3);
								//activePage->setColour(transpColour);
								//Ogre::MaterialManager::getSingletonPtr()->getByName(activePage->getMaterialName())->
								//	setParameter(

			GMenuItemList vect = menu->getVisible()->getChildren();
			for(GMenuItemList::iterator it = vect.begin(); it!= vect.end(); ++it)
			{
				if( (*it)->getAuxData() != NULL )
				{
					GAuxQuadStringInfo * tempPtr = (GAuxQuadStringInfo * )(*it)->getAuxData();
					// name of btn container
					try
					{
						if( tempPtr->getInfo() != "" )
						{
							//TODO: remove singleton access, and add additional aux info.
							//OverlayElement * elem = activePage->getChild( tempPtr->getInfo() );
							OverlayElement * elem;
							//elem = overlay->getChild( tempPtr->getInfo() );
							elem = OverlayManager::getSingleton().getOverlayElement(tempPtr->getInfo());
							if( elem != NULL )
							{
								elem->show();
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
						if( tempPtr->getInfo4() != "" )
						{
							OverlayElement * elem;
							// set the text color
							//elem = overlay->getChild( tempPtr->getInfo4() );
							elem = OverlayManager::getSingleton().getOverlayElement(tempPtr->getInfo4());
							if( elem != NULL )
							{
								static const Ogre::ColourValue activeSelectionColorTop(1.0f, 1.0f, 0.5f, 1.0f);
								static const Ogre::ColourValue activeSelectionColorBtm(1.0f, 0.0f, 0.0f, 1.0f);
								static const Ogre::ColourValue inactiveSelectionColorTop(0.90f, 0.76f, 0.56f, 0.7f);
								static const Ogre::ColourValue inactiveSelectionColorBtm(0.85f, 0.44f, 0.26f, 0.7f);
								static const Ogre::ColourValue editSelectionColorTop(1.0f, 0.0f, 0.0f, 1.0f);
								static const Ogre::ColourValue editSelectionColorBtm(0.1f, 0.0f, 0.0f, 1.0f);

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

					}
					catch(...){}
				}
			}
			// end of menu draw
		}
		GTRACE(5, "GBaseEngine: render overlay exiting");
		return true;
	}


	void GMenuState::notifyOfShutdown()
	{
	}


	// menu event handling


	void GMenuState::OnMenuAction( GMenuItem * item )
	{
		GTRACE(5, "OnMenuAction");
		// TODO: reactivate sound.
		//GSOUND_PLAY( GUS_SOUND_MENU_SELECTION_CLICKED );

		if( item->getName() == "exit" )
		{
			bool bResult = GBaseAppSettings::getPtr ()->save();
			if( !bResult )
			{
				GTRACE(1, "Warning! Unable to save user settings to disk!");
			}
			//GBaseStateManager::getPtr()->popState();
			GTRACE(5, "GMenuState::OnMenuAction, locking logicEventProcessMutex");
			//guslib::GScopedLock myOtherLock(logicEventProcessMutex);
			logicEventQueue.push( new GBaseEvent(GET_Exit, menu->getRoot()->getName()) );
			GTRACE(5, "GMenuState::OnMenuAction, releasing logicEventProcessMutex");
		}
		else if( item->getName() == "applyVid" )
		{
			GTRACE(4, "DBG BEFORE: fullscr="<<
				GBaseAppSettings::getPtr ()->setting.video.fullscreen
				<<",width="<< GBaseAppSettings::getPtr ()->setting.video.windowWidth
				<<", height="<< GBaseAppSettings::getPtr ()->setting.video.windowHeight
				);

			// Command to apply the video settings; this will copy the temp settings which are set at menu
			// option items into the saveable settings.
			GBaseAppSettings::getPtr ()->setting.video = GBaseAppSettings::getPtr ()->setting.videoTemp;
/*		bool vsync;			// vertical sync, only valid for fullscr.
		bool fullscreen;
		int windowWidth;
		int windowHeight;
		int fsaa;			// fullscreen antialiasing
		int refreshRate;	// not specifiable in all video modes
		int colourDepth;    // used mainly for the Direct3D mode
		std::string renderEngine;
		std::string renderEngineFullName;
		std::string renderModeString;
*/
			GTRACE(4, "applying vid settings: fullscr="<<
				GBaseAppSettings::getPtr ()->setting.video.fullscreen
				<<",width="<< GBaseAppSettings::getPtr ()->setting.video.windowWidth
				<<", height="<< GBaseAppSettings::getPtr ()->setting.video.windowHeight
				);

			if( ! GBaseAppSettings::getPtr ()->save() )
			{
				GTRACE(1, "Unable to save user settings!");
			}
			else
			{
				//// user settings were saved; we can restart the application.
				//// now close the current instance.
				GTRACE(5, "GMenuState::OnMenuAction, locking logicEventProcessMutex");
				//guslib::GScopedLock myOtherLock(logicEventProcessMutex);
				logicEventQueue.push( new GBaseEvent(GET_Reboot, menu->getRoot()->getName()) );
				GTRACE(5, "GMenuState::OnMenuAction, releasing logicEventProcessMutex");
			}
		}
		else if( item->getName() ==  std::string("quick") )
		{
			GTRACE(5, "GMenuState::OnMenuAction, locking logicEventProcessMutex");
			//guslib::GScopedLock myOtherLock(logicEventProcessMutex);
			logicEventQueue.push( new GBaseEvent(GET_Quick, menu->getRoot()->getName()) );
			GTRACE(5, "GMenuState::OnMenuAction, releasing logicEventProcessMutex");
		}
		else if( (item->getName() == std::string("map1")) || (item->getName() == std::string("map2")) )
		{
			GTRACE(5, "GMenuState::OnMenuAction, locking logicEventProcessMutex");
			//guslib::GScopedLock myOtherLock(logicEventProcessMutex);
			logicEventQueue.push( new GBaseEvent(GET_NormGame, item->getName()) );
			GTRACE(5, "GMenuState::OnMenuAction, releasing logicEventProcessMutex");
		}
		else if( (item->getName() == std::string("map1hi")) || (item->getName() == std::string("map2hi")) )
		{
			GTRACE(5, "GMenuState::OnMenuAction, locking logicEventProcessMutex");
			//guslib::GScopedLock myOtherLock(logicEventProcessMutex);
			logicEventQueue.push( new GBaseEvent(GET_ViewHiscore, item->getName()) );
			GTRACE(5, "GMenuState::OnMenuAction, releasing logicEventProcessMutex");
		}
	}


	bool GMenuState::OnMenuBack( GMenuItem * item )
	{
		GTRACE(5, "OnMenuBack");
		// TODO: reactivate sound.
		//GSOUND_PLAY( GUS_SOUND_MENU_SELECTION_CHANGED );
		return true;
	}

	void GMenuState::OnMenuDepthChange( GMenu * menu, bool fromTop )
	{
		if( menu == NULL )
		{
			// treat it as an exit command.
			// This will come when the "ESC" command is issued on the up-most level.
			//GBaseStateManager::getPtr()->popState();
			GTRACE(5, "OnMenuDepthChange, \"BACK\" command at up-most level... locking (logicEventProcessMutex)");
			//guslib::GScopedLock myOtherLock(logicEventProcessMutex);
			GTRACE(5, "OnMenuDepthChange, \"BACK\" command at up-most level... pushing exit command; locked (logicEventProcessMutex)");
			logicEventQueue.push( new GBaseEvent(GET_Exit, "") );
			GTRACE(5, "OnMenuDepthChange, pushed exit cmd; releasing mutex(logicEventProcessMutex)");
			return;
		}
		if( menu->getActive()->getName() == ME_MENU_VIDEOMODE  && fromTop )
		{
			GTRACE(4, "*** ME_MENU_VIDEOMODE (menu depth changed to this from top)");
			// Special case for entering the video mode settings.
			// We need to load each time the current settings and to clear the (possibly existing)
			// previous settings.

			// Store the current settings as the temp settings to be saved when the appply button is pressed.
			//GBaseAppSettings::getPtr ()->setting.videoTemp = GBaseAppSettings::getPtr ()->setting.video;

			std::string sRenderEngineItem;
			GMenuItem * myItem;
			myItem = menu->searchForItemByName(ME_MENU_RENDERING_ENGINE);
			if( myItem == NULL )
			{
				GTRACE(1, "Unable to set resolution options. Unable to find item "<<ME_MENU_RENDERING_ENGINE);
				return;
			}

			GMenuItemList itemList = myItem->getChildren();
			int myWidth = 0;
			int myHeight= 0;
			std::string engineStr = app::SerpentsGameHelper::get ().getPreferredRendererName ();
			app::SerpentsGameHelper::get ().getSizesFromResolutionString (engineStr, myWidth, myHeight);
			//GBaseMainApp::getCurrentRenderOptions(myWidth, myHeight);
			for( GMenuItemList::iterator it = itemList.begin(); it!= itemList.end(); ++it )
			{
				if( ((*it)->getName() == G_RENDER_ENGINE_D3D_SHORT_NAME && 
					G_RENDER_ENGINE_D3D_LONG_NAME == engineStr )
					|| ((*it)->getName() == G_RENDER_ENGINE_OGL_SHORT_NAME &&
					G_RENDER_ENGINE_OGL_LONG_NAME == engineStr ) )
				{
					// The rendering engine item must contain the CURRENT settings. This is because the user can select
					// a different rendering engine without hitting the apply button.
					GMenuItem *mySecondItem;
					mySecondItem = menu->searchForItemByName(ME_MENU_RESOLUTION);
					if( mySecondItem == NULL )
					{
						GTRACE(1, "Unable to set resolution options. Unable to find item "<<ME_MENU_RENDERING_ENGINE);
						return;
					}

					GTRACE(3, "Setting selected child... to "<<(*it)->getName());
					myItem->setSelectedChild( *it );

					std::vector<std::string> options = app::SerpentsGameHelper::get ().getEditableResolutionsMapping()[engineStr];

					GTRACE(4, "Got resolution options for engine:"<<engineStr<<","<<myWidth<<","<<myHeight);
					for( std::vector<std::string>::iterator myIt = options.begin(); myIt != options.end(); ++myIt )
					{
						GTRACE(5, "res; "<<*myIt);
					}

					if( options.size() > 0 )
					{
						GMenuItemList myItemList = mySecondItem->getChildren();
						GTRACE(3,"adding res options (item lsit size="<<myItemList.size()<<". eng="<<engineStr);
						//if( myItemList.size() == 0 )//always add?
						{
							// The res. options were not yet added, so add them now.
							GTRACE(4, "adding res options");

							// This function will also select the active resolution if possible.
							addResolutionOptionsAndReturnSelected();

							myItemList = mySecondItem->getChildren();
							if( myItemList.size() == 0 )
							{
								// If still no options are available
								GTRACE(2, "Warning: no resolution options available!");
								return;
							}
						}
					}

				}
			}

			// Also set the fullscreen flag.
			myItem = menu->searchForItemByName(ME_MENU_FULLSCREEN);
			if( !myItem )
			{
				GTRACE(1, "Unable to find fullscr menu entry "<<ME_MENU_FULLSCREEN);
				return;
			}
			itemList = myItem->getChildren();
			if( itemList.size() > 0 )
			{
				for( GMenuItemList::iterator it = itemList.begin(); it!= itemList.end(); ++it )
				{
					if(( (*it)->getName() == VIDEO_MODE_FULLSCREEN_ON
						&& GBaseAppSettings::getPtr ()->setting.video.fullscreen )
						|| (*it)->getName() == VIDEO_MODE_FULLSCREEN_OFF
						&& !GBaseAppSettings::getPtr ()->setting.video.fullscreen )
					{
						myItem->setSelectedChild( *it );
					}
				}
			}
			GBaseAppSettings::getPtr ()->setting.videoTemp = GBaseAppSettings::getPtr ()->setting.video;
		}
		GTRACE(5, "OnMenuDepthChange; active="<<menu->getActive()->getName());
		// TODO: reactivate sound.
		//GSOUND_PLAY( GUS_SOUND_MENU_SELECTION_CLICKED );
	}

	void GMenuState::OnMenuSelectionChange( GMenuItem *item )
	{
		GTRACE(5, "OnMenuSelectionChange, active="<<item->getName()<<";"<<item->getSelectedChild()->getName());
		// TODO: reactivate sound.
		//GSOUND_PLAY( GUS_SOUND_MENU_SELECTION_CHANGED );
	}

	void GMenuState::OnMenuOptionStartEdit( GMenuItem *optionGroup, GMenuItem *option )
	{
		GTRACE(5, "OnMenuOptionStartEdit; "<<(optionGroup? optionGroup->getName():"")
			<<":"<< (option? option->getName():"") );
		// TODO: reactivate sound.
		//GSOUND_PLAY( GUS_SOUND_MENU_OPTION_CHANGED );
	}

	void GMenuState::OnMenuOptionStopEdit( GMenuItem *optionGroup, GMenuItem *option, bool bCanceled )
	{
		GTRACE(5, "OnMenuOptionStopEdit; "<<(bCanceled?"Cancelled":"Accepted")<< " "<<
			(optionGroup? optionGroup->getName():"")<<":"<< (option? option->getName():"") );
		// TODO: reactivate sound.
		//GSOUND_PLAY( GUS_SOUND_MENU_OPTION_CHANGED );

		if( !bCanceled && optionGroup && option )
		{
			// (Changes accepted)
			if( optionGroup->getName() == ME_MENU_RENDERING_ENGINE )
			{
				GBaseAppSettings::getPtr ()->setting.videoTemp.renderEngine= (
					option->getName()=="Direct3D" ? Direct3D : OpenGL  );

				// if the rendering engine changed, we need to re-load options.
				GTRACE(3, "adding res opts after engine change!");
				std::string newRes = addResolutionOptionsAndReturnSelected();

				// The stored temp settings must also be updated.
				//GMenuItem *tempItem = menu->searchForItemByName( ME_MENU_RESOLUTION );
				//if( tempItem )
				//{
					//std::string sFullEntry = tempItem->getName();
					int width = 0;
					int height = 0;
					GTRACE(4, "Parsing :"<<newRes);
					app::SerpentsGameHelper::get ().getSizesFromResolutionString (newRes, width, height);

					GTRACE(3, "Setting resolution to :"<<width<<" x "<<height);
					GBaseAppSettings::getPtr ()->setting.videoTemp.windowWidth = width;
					GBaseAppSettings::getPtr ()->setting.videoTemp.windowHeight = height;
				//}
			}
			else if( optionGroup->getName() == ME_MENU_RESOLUTION )
			{
				std::string sFullEntry = option->getName();
				int width;
				int height;
				app::SerpentsGameHelper::get ().getSizesFromResolutionString (sFullEntry, width, height);

				GBaseAppSettings::getPtr ()->setting.videoTemp.windowWidth = width;
				GBaseAppSettings::getPtr ()->setting.videoTemp.windowHeight = height;
				GTRACE(4, "Set temp res to "<<width <<" x x"<<height);
			}
			else if( optionGroup->getName() == ME_MENU_FULLSCREEN )
			{
				GBaseAppSettings::getPtr ()->setting.videoTemp.fullscreen = (option->getName()=="on");
				GTRACE(4, "Set temp use fullscr to "<<GBaseAppSettings::getPtr ()->setting.videoTemp.fullscreen );
			}
			else if( optionGroup->getName() == ME_MENU_SOUNDMASTER )
			{
				float f = atof(option->getName().c_str());
				GBaseAppSettings::getPtr ()->setting.sound.masterVolume = f;
				// TODO: reactivate sound.
				//GSOUND_SET_VOLUME_FOR_CATEGORY( GSC_Master, f );
			}
			else if( optionGroup->getName() == ME_MENU_SOUNDMUSIC )
			{
				float f = atof(option->getName().c_str());
				GBaseAppSettings::getPtr ()->setting.sound.musicVolume = f;
				// TODO: reactivate sound.
				//GSOUND_SET_VOLUME_FOR_CATEGORY( GSC_Music, f );
			}
			else if( optionGroup->getName() == ME_MENU_SOUNDVOICE )
			{
				float f = atof(option->getName().c_str());
				GBaseAppSettings::getPtr ()->setting.sound.voiceVolume = f;
				// TODO: reactivate sound.
				//GSOUND_SET_VOLUME_FOR_CATEGORY( GSC_Voice, f );
			}
			else if( optionGroup->getName() == ME_MENU_SOUNDFX )
			{
				float f = atof(option->getName().c_str());
				GBaseAppSettings::getPtr ()->setting.sound.fxVolume = f;
				// TODO: reactivate sound.
				//GSOUND_SET_VOLUME_FOR_CATEGORY( GSC_Effect, f );
			}
			else if( optionGroup->getName() == ME_MENU_SHADOWS )
			{
				bool b = (option->getName() == "on");
				GBaseAppSettings::getPtr ()->setting.perf.useShadows = b;
				GTRACE(4, "Set temp use shadows to "<<b);
			}
			else if( optionGroup->getName() == ME_MENU_MESHQUAL )
			{
				int n = 0;
				if( option->getName()=="low" ) n=0;
				if( option->getName()=="high" ) n=1;
				GBaseAppSettings::getPtr ()->setting.perf.meshQuality = n;
				GTRACE(4, "Set temp meshq to "<<n);
			}
			else if( optionGroup->getName() == ME_MENU_POSTPROCESSING )
			{
				bool b = (option->getName() == "on");
				GBaseAppSettings::getPtr ()->setting.perf.postProcessing = b;
				GTRACE(4, "Set temp use postpr to "<<b);
			}
			else if( optionGroup->getName() == ME_MENU_CPUSLEEP )
			{
				int n = atoi(option->getName().c_str());
				GBaseAppSettings::getPtr ()->setting.perf.cpuSleep = n;
				GTRACE(4, "Set cpu delay to "<<n);
			}
		}
	}

	void GMenuState::OnMenuOptionValueChange( GMenuItem *optionGroup, GMenuItem *option )
	{
		GTRACE(5, "OnMenuOptionValueChange; "<<(optionGroup? optionGroup->getName():"")
			<<":"<< (option? option->getName():"") );
		// TODO: reactivate sound.
		//GSOUND_PLAY( GUS_SOUND_MENU_OPTION_CHANGED );
	}





	void GMenuState::OnMenuShouldRedraw( GMenu * menu )
	{
		GTRACE(5, "Menu Should redraw");
		// lock the menu event queue
		//guslib::GScopedLock myOtherLock(menuEventProcessMutex);
		menuEventQueue.push( new GBaseEvent(GET_MenuShouldRedraw, menu->getRoot()->getName()) );
		GTRACE(5, "Menu Should redraw (pushed event)");
	}



}
#endif