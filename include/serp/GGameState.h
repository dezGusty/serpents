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
//    Class to handle the game state.

#include "GBaseState.h"
#include "GBaseEngine.h"
#include <OIS/OIS.h>
#include <Ogre.h>
#include "GBaseSetting.h"
#include "GGameAlgo.h"
#include "GusCamera.h"

//game stuff
#include "VisualSerpent.h"
#include "Map.h"
#include "VisualBonus.h"
#include "VisualBodyPart.h"

#include "TopSideMessage.h"
#include <gaudiere.h>

#include "GEvent.h"

#include "HealthWidget.h"

#include <guslib/system/thread.h>

// -------- Add scripting support via LUA ----------------

#include <lua.hpp>

#ifndef USE_USER_INSTALLED_LUA
#pragma comment (lib, "lib/lua5.1.lib")
#endif//USE_USER_INSTALLED_LUA

namespace Serpents
{
	class GGameState 
		: public GBase::GBaseState
		, public GGameAlgoStateListener
		, public ScoringTableEventListener
		, public gussound::MusicPlayerListener
	{
		static GGameState * objectPtr;
		GBase::GBaseEngine *engine;
	protected:
#ifdef G_USE_MULTITHREAD
		static guslib::GMutex creationMutex;
		guslib::GMutex uiEventProcessMutex;
		guslib::GMutex logicEventProcessMutex;
		guslib::GMutex luaAccessMutex;
#endif
		GBase::GBaseEventQueue logicEventQueue;
		GBase::GSimpleEventQueue uiEventQueue;
		Ogre::Overlay * overlay;

		GGameState(GBase::GBaseEngine *enginePtr, GScoringTable * aTable, const std::string & levelToUse);
		Ogre::Camera *mCamera;
		Ogre::Camera *secondCamera ;
		Ogre::Camera *thirdCamera;

		bool guiWasRenderedOnce;
		bool loadingWasDone;
		std::string levelToLoad;
		Ogre::RaySceneQuery* raySceneQuery;

		GusCamera *playerCamera;


		// gameplay stuff

		bool firsttimeLenflag;

		VisualSerpent * singleSerpent;
		int gameAltitudeOfDamageArea;
		GBase::GModelConfigHolder modelCfgHolder;
		Map * gameMap;	//	the map object on which the action will take place.
		VisualBonusManager * bonusManager;
		GScoringTable * scoringTable;
		GGameAlgo * gameAlgo;

		guslib::Timer keyTimer;
		FadingMessageMap fadeItems;
		CustomWidgetMap widgets;

		// whenever the playing track's info changes, display the song info.
		virtual void displaySongInfo( const std::string & trackName );

		// LUA binding state.
		lua_State* L;

		virtual void registerLuaFunctions( lua_State * L );

	// ------------- LUA bindings begin -------------------
	#ifdef GAME_LUA_FUNCS
		static int l_addUIFadeText( lua_State *L );
		static int l_setUIFadeText( lua_State *L );

		static int l_setOverlayVisible( lua_State *L );
		static int l_setOverlayElementVisible( lua_State *L );
		static int l_setOverlayElementCaption( lua_State *L );

		static int l_addCustomWidget( lua_State *L );
		static int l_setCustomWidgetValue( lua_State *L );

		static int l_pushUIEvent( lua_State *L );

		static int l_addTemporaryEffect( lua_State *L );

	#endif// GAUDIERE_LUA_FUNCS

		static GGameState * getProtectedInstancePtr();
		virtual void pushUIEvent( GBase::GSimpleEvent *evt );

	private :
		GRenderSnapshotView tempView;
	public:

		virtual ~GGameState();

		static GGameState * getInstancePtr(GBase::GBaseEngine *enginePtr, GScoringTable * aTable, const std::string & levelToUse);
		static void deleteInstance();

		virtual void setLevelToUse( const std::string & fileName ){ levelToLoad = fileName; }

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

		virtual void handleKeys( guslib::TimeUnits passedTime );

		// The overrides to handle the logic callbacks

		virtual void OnGameFinished();
		virtual void OnBonusSpawned( BonusBase * bns );
		virtual void OnBonusPicked( BonusBase * bns );
		virtual void OnPlayerScoreChange( long score );
		virtual void OnPlayerHealthChange( double percentage );
		virtual void OnPlayerHungerChange( double percentage );
		virtual void OnPlayerLengthChange( double newLength );
		virtual void OnPlayerSpeedChange( double speed );
		//virtual void OnPlayerPositionChange( double x, double y );

		// score stuff
		virtual void OnScoringTablePositionChanged( long newPosition, long oldPosition );

		// other funcs

		virtual void showOverlays();
		virtual void hideOverlays();

		virtual void startGameSession();
		virtual void stopGameSession();

		virtual void createCameras();
		virtual void destroyCameras();

		virtual void createLights();
		virtual void destroyLights();

		virtual void createLevelObjects();
		virtual void destroyLevelObjects();

		virtual void loadLevelResources();
		virtual void unloadLevelResources();

		// render stuff

		virtual GRenderSnapshotView & getRenderSnapshot();


		// gameplay stuff
		virtual Map * createMap();
		virtual VisualSerpent * createSerpent();
		virtual VisualBonusManager * createBonusManager();
		virtual void setScoringTable( GScoringTable * aTable );

		// overrides for the GAudiere
		virtual void OnTrackPlay(const std::string & newTrack);
		virtual void OnTrackChange(const std::string & newTrack);
		virtual void OnPlaylistEnd(); // will be encountered in non-repeat mode only.
	};


}