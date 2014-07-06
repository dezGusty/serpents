#include "GusEngine.h"
#include "../utils/GUtils.h"
#include "../mains/GusFrameListener.h"
#include "../engine/GAppSettings.h"
#include "../utils/GCommonDef.h"
#include <ogreoverlay.h>
#include <OgrePanelOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>
#include "GusAudioEngine.h"
#include "EngineConstants.h"

#include "../mains/GusMainApp.h"

#define GUS_SOUND_MENU_SELECTION_CHANGED "data/sounds/MenuSelectionChange.wav"
#define GUS_SOUND_MENU_SELECTION_CLICKED "data/sounds/MenuSelectionClicked.wav"
#define GUS_SOUND_MENU_OPTION_CHANGED "data/sounds/MenuOptionChange.wav"


namespace Serpents
{
	GusEngine::GusEngine(Ogre::Root * myroot, GusFrameListener * mylistener)
	{
		GTRACE(3, "GusEngine constructor entered.");
		bExitCommandReceived = false;
		this->frameListener = mylistener;
		this->root = myroot;
		loaderOverlay = NULL;
		overlay = NULL;
		overlayManager = NULL;
		this->stateManager = new StateManager();
		this->bBootResourcesWereLoaded = false;
		bMenuResourcesWereLoaded = false;
		timer.reset();
	}

	GusEngine::~GusEngine()
	{
		//
		GTRACE(3, "GusEngine destructor entered.");
		if( stateManager != NULL )
		{
			delete stateManager;
			stateManager = NULL;
		}
		if( frameListener != NULL )
		{
			GTRACE(4, "GusEngine releasing self from framelistener");
			frameListener->releaseEngine();
			// The frame listener is not owned by the engine
			GTRACE(4, "GusEngine released connection to frameListener");
			releaseFrameListener();
		}
		GTRACE(3, "GusEngine destructor exiting.");
	}

	GusString getLongRenderDeviceNameFromShortName( GusString input )
	{
		if( input == RENDER_ENGINE_D3D_SHORT_NAME )
			return RENDER_ENGINE_D3D_LONG_NAME;
		if( input == RENDER_ENGINE_OGL_SHORT_NAME )
			return RENDER_ENGINE_OGL_LONG_NAME;
		return "";
	}


	void GusEngine::addResolutionOptions()
	{
		GusString sRenderEngineItem;
		GMenuItem * myItem;
		myItem = menu->searchForItemByName(ME_MENU_RENDERING_ENGINE);
		if( myItem == NULL )
		{
			GTRACE(1, "Unable to set resolution options. Unable to find item "<<ME_MENU_RENDERING_ENGINE);
			return;
		}
		myItem = myItem->getSelectedChild();
		if( myItem == NULL )
		{
			GTRACE(1, "Unable to set resolution options. No options for "<<ME_MENU_RENDERING_ENGINE);
		}
		// The rendering engine item must contain the CURRENT settings. This is because the user can select
		// a different rendering engine withouth hitting the apply button.
		sRenderEngineItem = getLongRenderDeviceNameFromShortName(myItem->getName());
		GTRACE(3, "Currently selected render engine = "<<sRenderEngineItem);
		myItem = menu->searchForItemByName(ME_MENU_RESOLUTION);
		if( myItem == NULL )
		{
			GTRACE(1, "Unable to set resolution options. Unable to find item "<<ME_MENU_RENDERING_ENGINE);
			return;
		}
		myItem->removeAllChildren();

		int myWidth = 0;
		int myHeight= 0;
		GusString engineStr = GusMainApp::getCurrentRenderOptions(myWidth, myHeight);
		GusVector<GusString> options = GusMainApp::getResolutionOptions(sRenderEngineItem);

		for( GusVector<GusString>::iterator it = options.begin(); it!= options.end(); ++it )
		{
			GTRACE(4, "Adding child "<< *it << " to: "<<myItem->getName() );
			GMenuItem *tempItem = new GMenuItem(menu, *it, *it); // using same value for name and caption.
			myItem->addChild(tempItem);
		}
		GTRACE(3, "Added res opts");
	}


	bool GusEngine::loadBootResources()
	{
		if( bBootResourcesWereLoaded )
		{
			GTRACE(1, "Warning: received command to load boot resources again!");
			return false;
		}
		ResourceGroupManager::getSingleton().initialiseResourceGroup("Bootstrap");
		ResourceGroupManager::getSingleton().loadResourceGroup("Bootstrap");
		overlayManager = OverlayManager::getSingletonPtr();
		GTRACE(3, "overlayManager is NULL?:" << (overlayManager==NULL));
		loaderOverlay = overlayManager->getByName("GusApp/Loader4_3");

		bool result = (loaderOverlay != NULL);
		bBootResourcesWereLoaded = result;
		return result;
	}


	void GusEngine::loadOverlays()
	{
		//loaderOverlay = OverlayManager::getSingletonPtr()->getByName("GusApp/Loader4_3");
		overlay = OverlayManager::getSingletonPtr()->getByName("GS/MenuOverlay");
		//Post init - hide templates and copy them.

		overlay->show();
		GusString gsCopyrightNotice = GusString( GUS_COPYRIGHT_NOTICE ) + GusString( GUS_APP_VERSION );
		OverlayManager::getSingleton().getOverlayElement("GS/TAMenuCredits")->
			setCaption(gsCopyrightNotice);

	}


	void GusEngine::loadOverlaysAccordingToState()
	{
		ApplicationState myState = stateManager->getState();
		switch( myState )
		{
			case Serpents::ASMainMenu:
				OverlayManager::getSingleton().getOverlayElement("Serpents/MainMenuPanel")->show();
				break;
		}
	}


	void GusEngine::loadResources()
	{
		std::vector<GusString> resList = GAppSettings::getInstancePtr()->getGeneralSettingsPtr()->getResourcePath();
		GTRACE(3, "..:: Creating General resource folder list ::..");
		for( std::vector<GusString>::iterator it=resList.begin(); it!= resList.end(); ++it )
		{
			GusString secName, typeName, archName;
			archName = *it;
			secName = "General";
			typeName = "FileSystem";
			if( archName.length()>4 )
			{
				if(archName.substr(archName.length()-4) == ".zip" )
				{
					typeName = "Zip";
				}
			}
			ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
			GTRACE(3, "Adding location to resource list:["<<archName<<"]");
		}
		ResourceGroupManager::getSingleton().initialiseResourceGroup("General");
		ResourceGroupManager::getSingleton().loadResourceGroup("General");

		bMenuResourcesWereLoaded = true;

		// TODO: add a listener and a progress bar for the resources.
		// Repeat for the level load as well, and make sure each mesh is displayed once (behind a fully opaque 2D Gui)
		//ResourceGroupListener * myListener;
	}

	void GusEngine::loadSounds()
	{
#ifdef GUS_ENABLE_SOUND
		GTRACE(3, "Loading sounds...");
		// This function is supposed to handle all sound loading.
		// The settings file may contain wildcards ("*"), meaning all files will be loaded.
		// TODO: load from settings.

		GusAudioEngine::getInstancePtr()->addSound(GUS_SOUND_MENU_SELECTION_CHANGED, GUS_SOUND_MENU_SELECTION_CHANGED, GST_Internal, GSC_Effect, true);

		//if( ! GusAudioEngine::getInstancePtr()->addSound( * new GSound(Serpents::ST_Internal, GUS_SOUND_MENU_SELECTION_CHANGED,
		//	GUS_SOUND_MENU_SELECTION_CHANGED, true, 1.0f) ) )
		//	return;

		GTRACE(3, "Loaded "<<GUS_SOUND_MENU_SELECTION_CHANGED);

		GusAudioEngine::getInstancePtr()->addSound(GUS_SOUND_MENU_SELECTION_CLICKED, GUS_SOUND_MENU_SELECTION_CLICKED, GST_Internal, GSC_Effect, true);

		//if( ! GusAudioEngine::getInstancePtr()->addSound( * new GSound(Serpents::ST_Internal, GUS_SOUND_MENU_SELECTION_CLICKED,
		//	GUS_SOUND_MENU_SELECTION_CLICKED, true, 1.0f) ) )
		//	return;
		GTRACE(3, "Loaded "<<GUS_SOUND_MENU_SELECTION_CLICKED);
		//if( ! GusAudioEngine::getInstancePtr()->addSound( * new GSound(Serpents::ST_Internal, GUS_SOUND_MENU_OPTION_CHANGED,
		//	GUS_SOUND_MENU_OPTION_CHANGED, true, 1.0f) ) )
		//	return;
		GusAudioEngine::getInstancePtr()->addSound(GUS_SOUND_MENU_OPTION_CHANGED, GUS_SOUND_MENU_OPTION_CHANGED, GST_Internal, GSC_Effect, true);
		GTRACE(3, "Loaded "<<GUS_SOUND_MENU_OPTION_CHANGED);

#ifdef GUS_ENABLE_MUSIC
		GusAudioEngine::getInstancePtr()->addPlaylist("menumusic");
		GusAudioEngine::getInstancePtr()->addPlaylist("gamemusic");

		GusVector<GusString> playlist = GAppSettings::getInstancePtr()->getMusicSettingsPtr()->getMusicMenuList();
		//GSound *snd;
		for( GusVector<GusString>::iterator it = playlist.begin(); it!= playlist.end(); ++it )
		{
			GAdvSound * mytempsnd = new GAdvSound(* GusAudioEngine::getInstancePtr(), *it, *it,
				GST_UserDefined, GSC_Music, false);
			GusAudioEngine::getInstancePtr()->addSound( mytempsnd );
			GusAudioEngine::getInstancePtr()->getPlaylist("menumusic")->addSound(mytempsnd);
			GTRACE(3, "Loading ["<<*it<<"]");
		}
		playlist = GAppSettings::getInstancePtr()->getMusicSettingsPtr()->getMusicGameList();
		for( GusVector<GusString>::iterator it = playlist.begin(); it!= playlist.end(); ++it )
		{
			GAdvSound * mytempsnd = new GAdvSound(* GusAudioEngine::getInstancePtr(), *it, *it,
				GST_UserDefined, GSC_Music, false);
			GusAudioEngine::getInstancePtr()->addSound( mytempsnd );
			GusAudioEngine::getInstancePtr()->getPlaylist("gamemusic")->addSound(mytempsnd);

			GTRACE(3, "Loading ["<<*it<<"]");
		}

		GTRACE(2, "Starting music play...");
		GSOUNDPLAYLISTWITHFADEIN("menumusic");
		GusAudioEngine::getInstancePtr()->getPlaylist("menumusic")->setRepeat(true);
#endif //GUS_ENABLE_MUSIC
		float fMasterVolume = 1.0f;
		GTRACE(2, "setting master audio volume to "<<fMasterVolume);
		GusAudioEngine::getInstancePtr()->setVolumeForCategory( GSC_Master,
			GAppSettings::getInstancePtr()->getUserSettingsPtr()->getSoundMasterVolume());
		GusAudioEngine::getInstancePtr()->setVolumeForCategory( GSC_Music,
			GAppSettings::getInstancePtr()->getUserSettingsPtr()->getSoundMusicVolume());
		GusAudioEngine::getInstancePtr()->setVolumeForCategory( GSC_Voice,
			GAppSettings::getInstancePtr()->getUserSettingsPtr()->getSoundVoiceVolume());
		GusAudioEngine::getInstancePtr()->setVolumeForCategory( GSC_Effect,
			GAppSettings::getInstancePtr()->getUserSettingsPtr()->getSoundFXVolume());
#endif //GUS_ENABLE_SOUND
	}


	bool GusEngine::mainLoopBeforeRender()
	{
		// currently, this will hold the app. logic.
		// it's projected to be moved to a separated thread in the future.


		// run thru the events.
		// maybe some events will allow other to be treated as well, but some will require that after
		// they are treted, the function exits, a frame is rendered and a followup event is processed.

		if( eventQueue.empty() )
		{
			// nothing to do
		}
		else
		{
			GusEvent * myevt = eventQueue.front();
			GTRACE(3, "event:"<<myevt->getDescription());
			eventQueue.pop();

			if( myevt->getParams() != NULL )
			{
				if( typeid( myevt->getParams() ) == typeid(GusEventParam*) )
				{
					GTRACE(4, "params for state change");
					loadOverlaysAccordingToState();
				}
				else
				{
					GTRACE(4, "unknown params:"<<typeid(myevt->getParams()).name());
					GTRACE(4, "expected:"<< typeid(GusEventParam*).name());
				}
			}
		}
		GusAudioEngine::getInstancePtr()->elapseTime();
		return true;
	}


	bool GusEngine::renderScene()
	{
		boost::mutex::scoped_lock myLock( loadAndRenderMutex );
		return true;
	}

	bool GusEngine::renderOverlay()
	{
		boost::mutex::scoped_lock myLock( loadAndRenderMutex );

		GTRACE(5, "GusEngine: render overlay entered");
		if( overlay == NULL || frameListener == NULL )
			return false;

		static String currFps = "FPS: ";

		// update stats when necessary
		try
		{
			OverlayElement * fpsCurr = OverlayManager::getSingleton().getOverlayElement("GusApp/FPSText");
			if( fpsCurr != 0 )
			{
				const RenderTarget::FrameStats& stats = frameListener->getRenderWindow()->getStatistics();
				fpsCurr->setCaption( currFps + StringConverter::toString((int)stats.lastFPS) );
			}
		}
		catch(...) {}


		// lock the menu event queue
		boost::mutex::scoped_lock myOtherLock(menuEventProcessMutex);

		if( menuEventQueue.size() == 0 )
		{
			// We have no events.
			GTRACE(5, "GusEngine: render overlay, but no events: exiting");
			return true;
		}

		// We have some events
		GusEvent * myEvent = menuEventQueue.front();
		if( myEvent == NULL )
		{
			GTRACE(5, "WARNING: GusEngine: render overlay, has null event in queue: exiting");
			return false;
		}

		if( myEvent->getDescription() == this->menu->getRoot()->getName() )
		{
			// The event is a menu event.
			// Currently, the only event of importance to the render is the redraw requirement.
			// The event contains no additional information atm, so it can be removed.
			menuEventQueue.pop();

			// do the redrawing of the menu.
			if( ! overlay )
				return false;

			Ogre::OverlayElement * activePage;
			// Hide all menu pages.
			{
				OverlayElement * elem;
				elem = overlay->getChild("GS/MainMenuPanel");
				if( elem )
					elem->hide();
				elem = overlay->getChild("GS/OptionsPanel");
				if( elem )
					elem->hide();
				elem = overlay->getChild("GS/MapSelectionPanel");
				if( elem )
					elem->hide();
				elem = overlay->getChild("GS/OptionsGrafxPanel");
				if( elem )
					elem->hide();
				elem = overlay->getChild("GS/OptionsVidModePanel");
				if( elem )
					elem->hide();
				elem = overlay->getChild("GS/OptionsSoundPanel");
				if( elem )
					elem->hide();
			}

			// Try to show the active page.
			try
			{
				// Now activate the needed ones according to state.
				switch( stateManager->getState() )
				{
				case ASMainMenu:
					{
						GusString sName = menu->getVisible()->getName();
						if( sName == "root" )
						{
							activePage = overlay->getChild("GS/MainMenuPanel");
							if( activePage )
								activePage->show();
							else return false;
						}
						else if( sName == "game" )
						{
							activePage = overlay->getChild("GS/MapSelectionPanel");
							if( activePage )
								activePage->show();
							else return false;
						}
						else if( sName == "options" )
						{
							activePage = overlay->getChild("GS/OptionsPanel");
							if( activePage )
								activePage->show();
							else return false;
						}
						else if( sName == "graphdetail" )
						{
							activePage = overlay->getChild("GS/OptionsGrafxPanel");
							if( activePage )
								activePage->show();
							else return false;
						}
						else if( sName == "vidmode" )
						{
							activePage = overlay->getChild("GS/OptionsVidModePanel");
							if( activePage )
								activePage->show();
							else return false;
						}
						else if( sName == "sound" )
						{
							activePage = overlay->getChild("GS/OptionsSoundPanel");
							if( activePage )
								activePage->show();
							else return false;
						}
					}
					break;
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
					GusAuxQuadStringInfo * tempPtr = (GusAuxQuadStringInfo * )(*it)->getAuxData();
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

					}
					catch(...){}
				}
			}
			// end of menu draw
		}
		GTRACE(5, "GusEngine: render overlay exiting");
		return true;
	}


	GRenderSnapshot & GusEngine::getRenderSnapshot()
	{
		GTRACE(5, "Engine: get render snapshot");
	#ifdef GTHREADSYNC
		boost::mutex::scoped_lock myLock( writingSnapshotMutex );
	#endif
		return * new GRenderSnapshot();
	}

	bool GusEngine::renderCompleteScene()
	{
		GTRACE(5, "Engine: render complete scene()");
		GRenderSnapshot mySnapShot = getRenderSnapshot();

		bool bTemp = root->renderOneFrame();
		return bTemp;
	}


	void GusEngine::createGameMenu()
	{
		menu = new SerpentsMenu();
		menu->loadFromXML();
		//addResolutionOptions();

		using namespace OIS;
		menu->clearKeyMap();
		menu->setKeyMap(GUtils::cmdActionEsc, KC_ESCAPE);
		menu->setKeyMap(GUtils::cmdActionIn, KC_RETURN);
		menu->setKeyMap(GUtils::cmdActionLeft, KC_LEFT);
		menu->setKeyMap(GUtils::cmdActionRight, KC_RIGHT);
		menu->setKeyMap(GUtils::cmdMenuDown, KC_DOWN);
		menu->setKeyMap(GUtils::cmdMenuUp, KC_UP);

		menu->setListener( this );
		this->getStateManager()->setState(ASMainMenu);
		this->OnMenuShouldRedraw( menu );

		// Set some non-video mode settings.

		GMenuItem *tempItem;
		tempItem = menu->searchForItemByName(ME_MENU_SOUNDMASTER);
		if( tempItem )
		{
			GusStringStream gss;
			gss<<GAppSettings::getInstancePtr()->getUserSettingsPtr()->getSoundMasterVolume();
			GusString sVol = gss.str();
			GMenuItem *auxItem = menu->searchForItemByName(sVol, tempItem);
			if( auxItem )
			{
				tempItem->setSelectedChild( auxItem );
			}
		}
		tempItem = menu->searchForItemByName(ME_MENU_SOUNDMUSIC);
		if( tempItem )
		{
			GusStringStream gss;
			gss<<GAppSettings::getInstancePtr()->getUserSettingsPtr()->getSoundMusicVolume();
			GusString sVol = gss.str();
			GMenuItem *auxItem = menu->searchForItemByName(sVol, tempItem);
			if( auxItem )
			{
				tempItem->setSelectedChild( auxItem );
			}
		}
		tempItem = menu->searchForItemByName(ME_MENU_SOUNDVOICE);
		if( tempItem )
		{
			GusStringStream gss;
			gss<<GAppSettings::getInstancePtr()->getUserSettingsPtr()->getSoundVoiceVolume();
			GusString sVol = gss.str();
			GMenuItem *auxItem = menu->searchForItemByName(sVol, tempItem);
			if( auxItem )
			{
				tempItem->setSelectedChild( auxItem );
			}
		}
		tempItem = menu->searchForItemByName(ME_MENU_SOUNDFX);
		if( tempItem )
		{
			GusStringStream gss;
			gss<<GAppSettings::getInstancePtr()->getUserSettingsPtr()->getSoundFXVolume();
			GusString sVol = gss.str();
			GMenuItem *auxItem = menu->searchForItemByName(sVol, tempItem);
			if( auxItem )
			{
				tempItem->setSelectedChild( auxItem );
			}
		}

		tempItem = menu->searchForItemByName(ME_MENU_MESHQUAL);
		if( tempItem )
		{
			GusString sItem = GAppSettings::getInstancePtr()->getUserSettingsPtr()->getMeshQuality()==0 ? "low" : "high";
			GMenuItem *auxItem = menu->searchForItemByName(sItem, tempItem);
			if( auxItem )
			{
				tempItem->setSelectedChild( auxItem );
			}
		}
		tempItem = menu->searchForItemByName(ME_MENU_SHADOWS);
		if( tempItem )
		{
			GusString sItem = GAppSettings::getInstancePtr()->getUserSettingsPtr()->getUseShadows() ? "on" : "off";
			GMenuItem *auxItem = menu->searchForItemByName(sItem, tempItem);
			if( auxItem )
			{
				tempItem->setSelectedChild( auxItem );
			}
		}
		tempItem = menu->searchForItemByName(ME_MENU_POSTPROCESSING);
		if( tempItem )
		{
			GusString sItem = GAppSettings::getInstancePtr()->getUserSettingsPtr()->getUsePostProcessing() ? "on" : "off";
			GMenuItem *auxItem = menu->searchForItemByName(sItem, tempItem);
			if( auxItem )
			{
				tempItem->setSelectedChild( auxItem );
			}
		}
		tempItem = menu->searchForItemByName(ME_MENU_CPUSLEEP);
		if( tempItem )
		{
			GusStringStream gss;
			gss<<GAppSettings::getInstancePtr()->getUserSettingsPtr()->getCPUDelay();
			GusString sItem = gss.str();
			GMenuItem *auxItem = menu->searchForItemByName(sItem, tempItem);
			if( auxItem )
			{
				tempItem->setSelectedChild( auxItem );
			}
		}


	}



	void GusEngine::setLoading( bool bLoading )
	{
	#ifdef GTHREADSYNC
		boost::mutex::scoped_lock myLock( loadingCmdWriteMutex );
	#endif
		GTRACE(3, "Engine::setting loading screen to "<<bLoading);

		try
		{
			if( loaderOverlay )
			{
				if( bLoading )
				{
					loaderOverlay->show();
				}
				else
				{
					loaderOverlay->hide();
				}
			}
			else
			{
				GTRACE(1, "NULL Overlay!");
			}
		}
		catch( ... ){}
	}



	void GusEngine::setExitCommandReceived()
	{
		boost::mutex::scoped_lock myLock( loadAndRenderMutex );
		this->shutDown();
	}

	void GusEngine::shutDown()
	{
		boost::mutex::scoped_lock myLock( exitCmdWriteMutex );
		GTRACE(3, "engine received cmd to shutdown");
		this->bExitCommandReceived = true;
	}


	bool GusEngine::startSession()
	{
		return true;
	}

	bool GusEngine::stopSession()
	{
		return true;
	}

	void GusEngine::releaseFrameListener()
	{
		frameListener = NULL;
	}

	//overloads + interf.-------------------------------
	void GusEngine::OnStateChanged( ApplicationState oldState, ApplicationState newState )
	{
		this->eventQueue.push( new GusEvent(GET_StateChange
			, GusString("state is being changed.")
			, new GusStateEventParam(oldState, newState) ) );
	}

	bool GusEngine::keyPressed( const OIS::KeyEvent &arg )
	{
		if( bExitCommandReceived ) return true;
		GTRACE(4, "keyPressed, "<<arg.key);
		switch( stateManager->getState() )
		{
		case ASMainMenu:
			GTRACE(4, "giving code to menu");
			menu->reactToKeyPress(arg.key);
			break;

		}
		return true;
	}
	bool GusEngine::keyReleased( const OIS::KeyEvent &arg )
	{
		if( bExitCommandReceived ) return true;
		GTRACE(4, "keyReleased, "<<arg.key);
		return true;
	}


	void GusEngine::OnMenuAction( GMenuItem * item )
	{
		GTRACE(5, "OnMenuAction");
		GSOUNDPLAY( GUS_SOUND_MENU_SELECTION_CLICKED );

		if( item->getName() == "exit" )
		{
			bool bResult = GAppSettings::getInstancePtr()->getUserSettingsPtr()->saveToFile();
			if( !bResult )
			{
				GTRACE(1, "Warning! Unable to save user settings to disk!");
			}
			this->shutDown();
		}
		else if( item->getName() == "applyVid" )
		{
			// Command to apply the video settings; this will copy the temp settings which are set at menu
			// option items into the saveable settings.
			// TODO, XXX
			GAppSettings::getInstancePtr()->getUserSettingsPtr()->setRenderEngine(
				GAppSettings::getInstancePtr()->getUserSettingsPtr()->getTempEngine() );
			GAppSettings::getInstancePtr()->getUserSettingsPtr()->setUseFullScreen(
				GAppSettings::getInstancePtr()->getUserSettingsPtr()->getTempUseFullscreen() );
			GAppSettings::getInstancePtr()->getUserSettingsPtr()->setWindowHeight(
				GAppSettings::getInstancePtr()->getUserSettingsPtr()->getTempWindowHeight() );
			GAppSettings::getInstancePtr()->getUserSettingsPtr()->setWindowWidth(
				GAppSettings::getInstancePtr()->getUserSettingsPtr()->getTempWindowWidth() );

			GTRACE(4, "applying vid settings: fullscr="<<
				GAppSettings::getInstancePtr()->getUserSettingsPtr()->getUseFullScreen()
				<<",width="<<GAppSettings::getInstancePtr()->getUserSettingsPtr()->getWindowWidth()
				<<", height="<<GAppSettings::getInstancePtr()->getUserSettingsPtr()->getWindowHeight()
				);

			if( ! GAppSettings::getInstancePtr()->getUserSettingsPtr()->saveToFile() )
			{
				GTRACE(1, "Unable to save user settings!");
			}
		}
	}



	bool GusEngine::OnMenuBack( GMenuItem * item )
	{
		GTRACE(5, "OnMenuBack");
		GSOUNDPLAY( GUS_SOUND_MENU_SELECTION_CHANGED );
		return true;
	}



	void GusEngine::OnMenuShouldRedraw( GMenu * menu )
	{
		GTRACE(5, "Menu Should redraw");
		// lock the menu event queue
		boost::mutex::scoped_lock myOtherLock(menuEventProcessMutex);

		menuEventQueue.push( new GusEvent(GET_MenuShouldRedraw, menu->getRoot()->getName()) );
	}



	void GusEngine::OnMenuSelectionChange( GMenuItem *item )
	{
		GTRACE(5, "OnMenuSelectionChange, active="<<item->getName()<<";"<<item->getSelectedChild()->getName());
		GSOUNDPLAY( GUS_SOUND_MENU_SELECTION_CHANGED );
	}



	void GusEngine::OnMenuDepthChange( GMenu * menu, bool fromTop )
	{
		if( menu == NULL )
		{
			// treat it as an exit command.
			// This will come when the "ESC" command is issued on the up-most level.
			this->shutDown();
			return;
		}

		if( menu->getActive()->getName() == ME_MENU_VIDEOMODE  && fromTop )
		{
			GTRACE(3, "*** ME_MENU_VIDEOMODE (menu depth changed to this)");
			// Special case for entering the video mode settings.
			// We need to load each time the current settings and to clear the (possibly existing)
			// previous settings.

			// Store the current settings as the temp settings to be saved when the appply button is pressed.
			GAppSettings::getInstancePtr()->getUserSettingsPtr()->setTempEngine(
				GAppSettings::getInstancePtr()->getUserSettingsPtr()->getRenderEngine() );
			GAppSettings::getInstancePtr()->getUserSettingsPtr()->setTempFullscreen(
				GAppSettings::getInstancePtr()->getUserSettingsPtr()->getUseFullScreen() );
			GAppSettings::getInstancePtr()->getUserSettingsPtr()->setTempWindowHeight(
				GAppSettings::getInstancePtr()->getUserSettingsPtr()->getWindowHeight() );
			GAppSettings::getInstancePtr()->getUserSettingsPtr()->setTempWindowWidth(
				GAppSettings::getInstancePtr()->getUserSettingsPtr()->getWindowWidth() );

			GusString sRenderEngineItem;
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
			GusString engineStr = GusMainApp::getCurrentRenderOptions(myWidth, myHeight);
			for( GMenuItemList::iterator it = itemList.begin(); it!= itemList.end(); ++it )
			{
				if( ((*it)->getName() == RENDER_ENGINE_D3D_SHORT_NAME &&
					RENDER_ENGINE_D3D_LONG_NAME == engineStr )
					|| ((*it)->getName() == RENDER_ENGINE_OGL_SHORT_NAME &&
					RENDER_ENGINE_OGL_LONG_NAME == engineStr ) )
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
					GusVector<GusString> options = GusMainApp::getResolutionOptions(engineStr, true, myWidth, myHeight);
					if( options.size() > 0 )
					{
						GusString currentName = *options.begin();
						GMenuItemList myItemList = mySecondItem->getChildren();
						if( myItemList.size() == 0 )
						{
							// The res. options were not yet added, so add them now.

							addResolutionOptions();

							myItemList = mySecondItem->getChildren();
							if( myItemList.size() == 0 )
							{
								// If still no options are available
								GTRACE(2, "Warning: no resolution options available!");
								return;
							}
						}
						// Set the selected resolution to the current one.
						for( GMenuItemList::iterator it = myItemList.begin(); it!= myItemList.end(); ++it )
						{
							if( (*it)->getName() == currentName )
							{
								mySecondItem->setSelectedChild( *it );
							}
						}
					}

					GTRACE(3, "Setting selected child... to "<<(*it)->getName());
					myItem->setSelectedChild( *it );
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
						&& GAppSettings::getInstancePtr()->getUserSettingsPtr()->getUseFullScreen() )
						|| (*it)->getName() == VIDEO_MODE_FULLSCREEN_OFF
						&& !GAppSettings::getInstancePtr()->getUserSettingsPtr()->getUseFullScreen() )
					{
						myItem->setSelectedChild( *it );
					}
				}
			}


		}
		GTRACE(5, "OnMenuDepthChange; active="<<menu->getActive()->getName());
		GSOUNDPLAY( GUS_SOUND_MENU_SELECTION_CLICKED );
	}



	void GusEngine::OnMenuOptionStartEdit( GMenuItem *optionGroup, GMenuItem *option )
	{
		GTRACE(5, "OnMenuOptionStartEdit; "<<(optionGroup? optionGroup->getName():"")
			<<":"<< (option? option->getName():"") );
		GSOUNDPLAY( GUS_SOUND_MENU_OPTION_CHANGED );
	}



	void GusEngine::OnMenuOptionStopEdit( GMenuItem *optionGroup, GMenuItem *option, bool bCanceled )
	{
		GTRACE(5, "OnMenuOptionStopEdit; "<<(bCanceled?"Cancelled":"Accepted")<< " "<<
			(optionGroup? optionGroup->getName():"")<<":"<< (option? option->getName():"") );
		GSOUNDPLAY( GUS_SOUND_MENU_OPTION_CHANGED );

		if( !bCanceled && optionGroup && option )
		{
			// (Changes accepted)
			if( optionGroup->getName() == ME_MENU_RENDERING_ENGINE )
			{
				// if the rendering engine changed, we need to re-load options.
				this->addResolutionOptions();

				GAppSettings::getInstancePtr()->getUserSettingsPtr()->setTempEngine(
					option->getName()=="Direct3D" ? Direct3D : OpenGL  );

				// The stored temp settings must also be updated.
				GMenuItem *tempItem = menu->searchForItemByName( ME_MENU_RESOLUTION );
				if( tempItem )
				{
					GusString sFullEntry = tempItem->getName();
					int width;
					int height;
					GusMainApp::getResolutionFromString( sFullEntry, width, height );

					GAppSettings::getInstancePtr()->getUserSettingsPtr()->setTempWindowWidth(
						width
						);
					GAppSettings::getInstancePtr()->getUserSettingsPtr()->setTempWindowHeight(
						height
						);
				}

				// TODO: also add an event, to ask the user to restart the app.
				// an alternative would be to auto-restart the app with params.
			}
			else if( optionGroup->getName() == ME_MENU_RESOLUTION )
			{
				GusString sFullEntry = option->getName();
				int width;
				int height;
				GusMainApp::getResolutionFromString( sFullEntry, width, height );

				GAppSettings::getInstancePtr()->getUserSettingsPtr()->setTempWindowWidth(
					width
					);
				GAppSettings::getInstancePtr()->getUserSettingsPtr()->setTempWindowHeight(
					height
					);
				GTRACE(4, "Set temp res to "<<width <<" x x"<<height);
			}
			else if( optionGroup->getName() == ME_MENU_FULLSCREEN )
			{
				GAppSettings::getInstancePtr()->getUserSettingsPtr()->setTempFullscreen(
					option->getName()=="on"
					);
				GTRACE(4, "Set temp use fullscr to "<<GAppSettings::getInstancePtr()->getUserSettingsPtr()->getTempUseFullscreen() );
			}
			else if( optionGroup->getName() == ME_MENU_SOUNDMASTER )
			{
				float f = atof(option->getName().c_str());
				GAppSettings::getInstancePtr()->getUserSettingsPtr()->setSoundMasterVolume(f);
				GusAudioEngine::getInstancePtr()->setVolumeForCategory( GSC_Master, f );
			}
			else if( optionGroup->getName() == ME_MENU_SOUNDMUSIC )
			{
				float f = atof(option->getName().c_str());
				GAppSettings::getInstancePtr()->getUserSettingsPtr()->setSoundMusicVolume(f);
				GusAudioEngine::getInstancePtr()->setVolumeForCategory( GSC_Music, f );
			}
			else if( optionGroup->getName() == ME_MENU_SOUNDVOICE )
			{
				float f = atof(option->getName().c_str());
				GAppSettings::getInstancePtr()->getUserSettingsPtr()->setSoundVoiceVolume(f);
				GusAudioEngine::getInstancePtr()->setVolumeForCategory( GSC_Voice, f );
			}
			else if( optionGroup->getName() == ME_MENU_SOUNDFX )
			{
				float f = atof(option->getName().c_str());
				GAppSettings::getInstancePtr()->getUserSettingsPtr()->setSoundFXVolume(f);
				GusAudioEngine::getInstancePtr()->setVolumeForCategory( GSC_Effect, f );
			}
			else if( optionGroup->getName() == ME_MENU_SHADOWS )
			{
				bool b = (option->getName() == "on");
				GAppSettings::getInstancePtr()->getUserSettingsPtr()->setUseShadows(b);
				GTRACE(4, "Set temp use shadows to "<<b);
			}
			else if( optionGroup->getName() == ME_MENU_MESHQUAL )
			{
				int n = 0;
				if( option->getName()=="low" ) n=0;
				if( option->getName()=="high" ) n=1;
				GAppSettings::getInstancePtr()->getUserSettingsPtr()->setMeshQuality(n);
				GTRACE(4, "Set temp meshq to "<<n);
			}
			else if( optionGroup->getName() == ME_MENU_POSTPROCESSING )
			{
				bool b = (option->getName() == "on");
				GAppSettings::getInstancePtr()->getUserSettingsPtr()->setUsePostProcessing(b);
				GTRACE(4, "Set temp use postpr to "<<b);
			}
			else if( optionGroup->getName() == ME_MENU_CPUSLEEP )
			{
				int n = atoi(option->getName().c_str());
				GAppSettings::getInstancePtr()->getUserSettingsPtr()->setCPUDelay(n);
				GTRACE(4, "Set cpu delay to "<<n);
			}
		}
	}


	void GusEngine::OnMenuOptionValueChange( GMenuItem *optionGroup, GMenuItem *option )
	{
		GTRACE(5, "OnMenuOptionValueChange; "<<(optionGroup? optionGroup->getName():"")
			<<":"<< (option? option->getName():"") );
		GSOUNDPLAY( GUS_SOUND_MENU_OPTION_CHANGED );
	}


}
