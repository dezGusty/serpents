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
#include "serp/GGameState.h"

#include "GStencilRenderer.h"

//Ogre includes
#include <ogreoverlay.h>
#include "GBaseMainApp.h"
#include "GBaseEngineDefinitions.h"

#include "GBaseSetting.h"
#include "GSimpleOgreBorder.hxx"
#include "GResourceManager.h"

#include <guslib/trace/trace.h>
#include <gussound.h>

#include "SerpConstants.h"

static Ogre::Vector3 sitemTranslationVector = Ogre::Vector3(0,0,0);

#define LEVEL_GROUP "Level"

#ifndef LOAD_RESOURCES_IN_LOGIC_THREAD
#define LOAD_RESOURCES_IN_LOGIC_THREAD 0
#endif

#include "GAudiere.h"
using namespace GAudiere;

using namespace GBase;
using namespace Ogre;
using namespace GUtils;

// the LUA Tracing Level
#define LTL 5

const std::string c_nameOfPlayerLight = "pFollowHeadLight";
const int c_heightOfLight = 30;
namespace Serpents
{
	GGameState * GGameState::objectPtr=NULL;
	guslib::GMutex GGameState::creationMutex;
	GGameState * GGameState::getInstancePtr(GBaseEngine *enginePtr, GScoringTable * aTable, const std::string & levelToUse)
	{
		if( !objectPtr )
		{
			guslib::GScopedLock myLock( creationMutex );
			//recheck to see if the object was not constructed while trying to obtain the lock.
			if( !objectPtr )
			{
				volatile GGameState * tempPtr = new GGameState(enginePtr, aTable, levelToUse);
				objectPtr = (GGameState*)tempPtr;
			}
		}
		objectPtr->setLevelToUse( levelToUse );
		return objectPtr;
	}

	GGameState * GGameState::getProtectedInstancePtr()
	{
		if( !objectPtr )
		{
			throw( std::exception("Tried to get a protected instance ptr, without having created one first"));
		}
		return objectPtr;
	}

	void GGameState::deleteInstance()
	{
		GTRACE(4, "deleting game state static instance...");
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



	GGameState::GGameState(GBaseEngine *enginePtr, GScoringTable * aTable, const std::string & levelToUse)
		: GBaseState("game")
		, engine(enginePtr)
		, playerCamera(NULL)
		, bonusManager(NULL)
		, gameMap(NULL)
		, singleSerpent(NULL)
		, gameAlgo(NULL)
		, scoringTable(aTable)
		, levelToLoad( levelToUse )
		, L(NULL)
	{
	}

	GGameState::~GGameState()
	{
		GTRACE(4, "Game state destroyed");
	}


	void GGameState::showOverlays()
	{
		GTRACE(LTL, "getting lua mutex (showoverlays)");
		guslib::GScopedLock myLock(luaAccessMutex);
		GTRACE(LTL, "got lua mutex (showoverlays)");
		lua_getglobal(L, "onShowHideOverlays");  // function to be called
		lua_pushinteger(L, 1);   // push the argument: true, we show the overlays.
		// make the call, 1 argument, no expected results to pop
		if (lua_pcall(L, 1, 0, 0) != 0)
		{
			GTRACE(1, "Exception: onShowHideOverlays:"<<lua_tostring(L, -1));
			throw( std::exception("error running function onShowHideOverlays:"));
		}
	}

	void GGameState::hideOverlays()
	{
		GTRACE(LTL, "getting lua mutex (hideoverlays)");
		guslib::GScopedLock myLock(luaAccessMutex);
		GTRACE(LTL, "got lua mutex (hideoverlays)");
		lua_getglobal(L, "onShowHideOverlays");  // function to be called
		lua_pushinteger(L, 0);   // push the argument: true, we show the overlays.
		// make the call, 1 argument, no expected results to pop
		if (lua_pcall(L, 1, 0, 0) != 0)
		{
			GTRACE(1, "Exception: onShowHideOverlays:"<<lua_tostring(L, -1));
			throw( std::exception("error running function onShowHideOverlays:"));
		}
	}

	void GGameState::startGameSession()
	{
		// make sure that rendering is not active.
		GTRACE(6, "GGameState::startGameSession getting load and render mutex");
		guslib::GScopedLock myLock( engine->getLoadAndRenderMutex() );
		GTRACE(6, "GGameState::startGameSession got load and render mutex");

		firsttimeLenflag = true;
		engine->getRenderWindow()->resetStatistics();
		this->raySceneQuery = NULL;

		//load the level

		std::string usedLevel = levelToLoad;
		GTRACE(3, "Loading level ["<<usedLevel<<"]");
		if( ! GBaseAppSettings::getPtr ()->loadLevel(usedLevel) )
		{
			GTRACE(1, "ERROR while loading level... aborting load");
			guslib::GScopedLock myOtherLock(logicEventProcessMutex);
			logicEventQueue.push( new GBaseEvent(GET_Exit, "") );
			return;
		}

		// save extra settings; these are stored in local vars.
		gameAltitudeOfDamageArea = GBaseAppSettings::getPtr ()->level.lvl.damageZoneAltitude;
		modelCfgHolder = GBaseAppSettings::getPtr ()->model;

		// load additional resources

		loadLevelResources();

		GTRACE(3, "Loaded level ["<<usedLevel<<"]; creating scene");
		createCameras();

		//create the border

		double w = GBaseAppSettings::getPtr ()->level.lvl.worldSize.x;
		double h = GBaseAppSettings::getPtr ()->level.lvl.worldSize.y;
		double z = 20;
		double bw = 5;
		globalTranslationVector = Vector3( w/2, z, h/2 );

		w = GBaseAppSettings::getPtr ()->level.lvl.damageZoneTo.x 
			- GBaseAppSettings::getPtr ()->level.lvl.damageZoneFrom.x;
		h = GBaseAppSettings::getPtr ()->level.lvl.damageZoneTo.y 
			- GBaseAppSettings::getPtr ()->level.lvl.damageZoneFrom.y;

		//int btvX = GBaseAppSettings::getPtr ()->level.lvl.safeZoneFrom.x
		//	- GBaseAppSettings::getPtr ()->level.lvl.damageZoneFrom.x;
		//int btvY = GBaseAppSettings::getPtr ()->level.lvl.safeZoneFrom.y
		//	- GBaseAppSettings::getPtr ()->level.lvl.damageZoneFrom.y;

		Ogre::Vector3 borderTranslationVector = globalTranslationVector;// + Vector3(-btvX/2,0,-btvY/2);

		GTRACE(3, "calling game border creation for size="<<w<<","<<h);
		OSE_createTheBorder( engine->getSceneManager(), w, h, bw, borderTranslationVector, std::string("Serpents/Stone1"),
			LEVEL_GROUP );
		
		// Now that the border has been created, modify the translation vector by the border size.
		globalTranslationVector.x += bw/2;
		globalTranslationVector.z += bw/2;

		// Set the ambient light. Loaded from the level.

		Vector3 vTemp = GBaseAppSettings::getPtr ()->level.lvl.ambientLight;
		engine->getSceneManager()->setAmbientLight( ColourValue( vTemp.x, vTemp.y, vTemp.z));

		// Use a skybox if required. Loaded from the level.
		std::string sTemp = GBaseAppSettings::getPtr ()->level.lvl.skyBoxName;
		DFLOAT dTemp = GBaseAppSettings::getPtr ()->level.lvl.skyBoxDist;
		GTRACE(4, "Loading skybox...:"<<sTemp);
		if( sTemp != "" )
		{
			engine->getSceneManager()->setSkyBox(true, //enable
												sTemp, //material
												dTemp, //dist
												true,
												Ogre::Quaternion::IDENTITY,
												LEVEL_GROUP);
		}

		GTRACE(4, "Setting shadow options...");

		// Set the SHADOW mode. Loaded from the user options.
		if( GBaseAppSettings::getPtr ()->setting.perf.useShadows )
		{
			switch( GBaseAppSettings::getPtr ()->setting.perf.shadowType )
			{
			case 2:
				engine->getSceneManager()->setShadowTechnique( SHADOWTYPE_STENCIL_ADDITIVE );
				break;
			case 3:
				engine->getSceneManager()->setShadowTechnique( SHADOWTYPE_TEXTURE_ADDITIVE );
				break;
			case 4:
				engine->getSceneManager()->setShadowTechnique( SHADOWTYPE_TEXTURE_MODULATIVE );
				break;
			case 1:
			default:
				engine->getSceneManager()->setShadowTechnique( SHADOWTYPE_STENCIL_MODULATIVE );
				break;
			}
		}
		else
		{
			engine->getSceneManager()->setShadowTechnique( SHADOWTYPE_NONE );
		}
		engine->getSceneManager()->setShadowFarDistance( 2000 );

		//engine->getSceneManager()->set

		engine->getSceneManager()->setShadowColour( ColourValue(
			GBaseAppSettings::getPtr ()->level.lvl.shadowColor.x,
			GBaseAppSettings::getPtr ()->level.lvl.shadowColor.y,
			GBaseAppSettings::getPtr ()->level.lvl.shadowColor.y,
			GBaseAppSettings::getPtr ()->level.lvl.shadowAlpha) );

		GTRACE(3, "Creating map lights...");
		// since the shadow options are now in place, the lights may be created.
		createLights();

		// Load the fog settings.

		Ogre::Vector3 fogColour = GBaseAppSettings::getPtr ()->level.lvl.fogColor;
		ColourValue fadeColour(fogColour.x, fogColour.y, fogColour.z);
		Ogre::FogMode fmode = FOG_NONE;
		switch( GBaseAppSettings::getPtr ()->level.lvl.fogType )
		{
		case 1:
			fmode = FOG_EXP;
			break;
		case 2:
			fmode = FOG_EXP2;
			break;
		case 3: 
			fmode = FOG_LINEAR;
			break;
		}
		Ogre::Real expDensity = GBaseAppSettings::getPtr ()->level.lvl.fogExpDensity;
		Ogre::Real linearStart = GBaseAppSettings::getPtr ()->level.lvl.fogLinStart;
		Ogre::Real linearEnd = GBaseAppSettings::getPtr ()->level.lvl.fogLinStop;

		// the terrain must be loaded prior to applying the fog.
#ifdef GUS_USE_WORLD_GEOMETRY
		GTRACE(4, "Loading terrain...");
		// Set the terrain for the level
		sTemp = GBaseAppSettings::getPtr ()->level.lvl.terrainCfg;
		if( sTemp != "" )
		{
			if( ResourceGroupManager::getSingleton().getWorldResourceGroupName() != "" )
			{
				ResourceGroupManager::getSingleton().unlinkWorldGeometryFromResourceGroup(
					ResourceGroupManager::getSingleton().getWorldResourceGroupName() );
			}
			ResourceGroupManager::getSingleton().linkWorldGeometryToResourceGroup( LEVEL_GROUP, sTemp, 
				engine->getSceneManager() );

			engine->getSceneManager()->setWorldGeometry( sTemp );
		}

		raySceneQuery = engine->getSceneManager()->createRayQuery( Ray(this->mCamera->getPosition(), Vector3::NEGATIVE_UNIT_Y));
		GResourceManager::getPtr()->setRaySceneQuery( raySceneQuery );
#endif
		GTRACE(5, "Setting fog mode...");
		engine->getSceneManager()->setFog( fmode, fadeColour, expDensity, linearStart, linearEnd );

		GTRACE(4, "Creating map objects...");
		createLevelObjects();

		// --- the general level data was initialized, now call the customization script.

		{
			GTRACE(LTL, "getting lua mutex (startGameSession)");
			guslib::GScopedLock myLock(luaAccessMutex);
			GTRACE(LTL, "got lua mutex (startGameSession)");

			lua_getglobal(L, "onLevelLoaded");  // function to be called
			// make the call, no arguments, no expected results to pop
			if (lua_pcall(L, 0, 0, 0) != 0)
			{
				GTRACE(1, "Exception: onLevelLoaded:"<<lua_tostring(L, -1));
				throw( std::exception("error running function onLevelLoaded:"));
			}
		}

		GSOUND_SWITCH_TO_PLAYLIST(GUS_GAME_PLAYLIST);

		GTRACE(6, "GGameState::startGameSession releasing load and render mutex");
	}



	void GGameState::stopGameSession()
	{
		// make sure that rendering is not active.
		GTRACE(6, "GGameState::stopGameSession() getting mutex(loadAndRenderMutex)");
		guslib::GScopedLock myLock( engine->getLoadAndRenderMutex() );
		GTRACE(6, "GGameState::stopGameSession() got mutex(loadAndRenderMutex)");

		if( gameAlgo )
		{
			GTRACE(4, "GGameState deleting algo");
			delete gameAlgo;
			gameAlgo = NULL;
		}

		if( singleSerpent )
		{
			GTRACE(4, "GGameState deleting serpent");
			delete singleSerpent;
			singleSerpent = NULL;
		}

		if( gameMap )
		{
			GTRACE(4, "GGameState deleting map");
			delete gameMap;
			gameMap = NULL;
		}
		if( bonusManager )
		{
			GTRACE(4, "GGameState deleting bonus mgr");
			delete bonusManager;
			bonusManager = NULL;
		}

		if( scoringTable )
		{
			// remove the listener
			scoringTable->setListener( NULL );
		}

		GTRACE(4, "Hiding game overlays...");
		hideOverlays();
		if( raySceneQuery!= NULL) 
		{
			engine->getSceneManager()->destroyQuery( raySceneQuery );
			GResourceManager::getPtr()->setRaySceneQuery( NULL );
			raySceneQuery = NULL;
		}

		GBase::GResourceManager::getPtr()->processUnloadRequests();
		GBase::GResourceManager::getPtr()->cancelAllLoadRequests();

		engine->getSceneManager()->setFog(FOG_NONE);
		GTRACE(4, "Destroying objects...");
		destroyLevelObjects();
		GTRACE(4, "Destroying scene...");
		destroyCameras();

		OSE_destroyTheBorder( engine->getSceneManager() );

		GTRACE(3, "Destroying lights...");
		destroyLights();

		GTRACE(4, "Unloading resources...");
		unloadLevelResources();

		engine->getSceneManager()->clearScene();
		GTRACE(6, "GGameState::stopGameSession() releasing mutex(loadAndRenderMutex)");

		//also destroy UI item lists that may have been populated by the lua scripts.

		fadeItems.clear();
		widgets.clear();
	}

	void GGameState::createCameras()
	{
		GTRACE(2, "Creating game scene...");
		if( engine->getCamera() != NULL )
		{
			mCamera = engine->getCamera();
			mCamera->setNearClipDistance(5);
		}
		else
		{
			GTRACE(1, "ERROR! Game engine doesn't have a camera assigned to it!");
			mCamera = engine->getSceneManager()->createCamera("PlayerCam");
			mCamera->setNearClipDistance(5);
			engine->setCamera(mCamera);
		}

		secondCamera = engine->getSceneManager()->createCamera("OverviewCam");
		secondCamera->setNearClipDistance(5);

		thirdCamera = engine->getSceneManager()->createCamera("ActionCam");
		thirdCamera->setNearClipDistance(5);

		GTRACE(4, "Setting up camera props...");
		this->mCamera->setPosition( GBaseAppSettings::getPtr ()->setting.cam.cam1Pos );
		this->mCamera->lookAt( GBaseAppSettings::getPtr ()->setting.cam.cam1LookAt );
		this->mCamera->setNearClipDistance(5);

		GTRACE(4, "Setting up camera props...2");
		this->secondCamera->setPosition( GBaseAppSettings::getPtr ()->setting.cam.cam2Pos );
		this->secondCamera->lookAt( GBaseAppSettings::getPtr ()->setting.cam.cam2LookAt );
		this->secondCamera->setNearClipDistance(5);

		GTRACE(4, "Setting up camera props...3");
		this->thirdCamera->setPosition( GBaseAppSettings::getPtr ()->setting.cam.cam1Pos );
		this->thirdCamera->lookAt( GBaseAppSettings::getPtr ()->setting.cam.cam1LookAt );
		this->thirdCamera->setNearClipDistance(5);

		// create the player camera afterwards, to keep the original orientation
		playerCamera = new GusCamera( mCamera );

		if( GBaseAppSettings::getPtr ()->setting.perf.postProcessing )
		{
			const RenderSystemCapabilities* caps = Root::getSingleton().getRenderSystem()->getCapabilities();
			if (caps->hasCapability(RSC_VERTEX_PROGRAM) && (caps->hasCapability(RSC_FRAGMENT_PROGRAM)))
			{
				StringList composCopy = GBaseAppSettings::getPtr ()->setting.perf.compositorList;
				for( StringList::iterator it = composCopy.begin(); it!= composCopy.end(); ++it )
				{
					if( *it != "" )
					{
						CompositorManager::getSingleton().addCompositor( mCamera->getViewport() , *it);
						CompositorManager::getSingleton().setCompositorEnabled( mCamera->getViewport() , *it, true);
					}
				}
			}
		}

		//	set camera properties
		playerCamera->setZoomMax( GBaseAppSettings::getPtr ()->setting.cam.zoomMax );
		playerCamera->setZoomLevel( GBaseAppSettings::getPtr ()->setting.cam.zoomLevel );
		playerCamera->setZoomStep( GBaseAppSettings::getPtr ()->setting.cam.zoomStep );
		playerCamera->setFOV( GBaseAppSettings::getPtr ()->setting.cam.fov );
		playerCamera->setTargetPos( globalTranslationVector );
	}

	void GGameState::destroyCameras()
	{
		// the first camera is managed by the engine class itself.
		engine->getSceneManager()->destroyCamera( secondCamera );
		engine->getSceneManager()->destroyCamera( thirdCamera );

		if( playerCamera )
		{
			delete playerCamera;
			playerCamera = NULL;
		}
	}


	void GGameState::createLights()
	{
		GTRACE(3, "creating lights...");
		//try to create a player light if one doesn't exist already
		int numLights = GBaseAppSettings::getPtr ()->level.lvl.numLights;
		if( numLights == 0 || GBaseAppSettings::getPtr ()->level.lvl.lights[numLights-1].name != c_nameOfPlayerLight )
		{
			GBaseAppSettings::getPtr ()->level.lvl.lights.push_back(GBaseLightSettings());
			GBaseAppSettings::getPtr ()->level.lvl.lights[numLights].castShadows = 0;
			GBaseAppSettings::getPtr ()->level.lvl.lights[numLights].type = 0;
			GBaseAppSettings::getPtr ()->level.lvl.lights[numLights].diffuseColor = Ogre::Vector3( 1.0, 1.0, 1.0 );
			GBaseAppSettings::getPtr ()->level.lvl.lights[numLights].specularColor= Ogre::Vector3( 1.0, 1.0, 1.0 );
			GBaseAppSettings::getPtr ()->level.lvl.lights[numLights].position = 
				GBaseAppSettings::getPtr ()->level.lvl.playerSpawn.x, c_heightOfLight, GBaseAppSettings::getPtr ()->level.lvl.playerSpawn.y;
			GBaseAppSettings::getPtr ()->level.lvl.lights[numLights].name = c_nameOfPlayerLight;
			GBaseAppSettings::getPtr ()->level.lvl.lights[numLights].direction = Ogre::Vector3(0, 0, 0);
			GBaseAppSettings::getPtr ()->level.lvl.lights[numLights].range = Ogre::Vector2(5, 5);
			GBaseAppSettings::getPtr ()->level.lvl.numLights ++;
			GTRACE(4, "Added new light");
		}

		numLights = GBaseAppSettings::getPtr ()->level.lvl.numLights;
		for( int i=0; i<numLights; i++ )
		{
			Light * lTemp;
			GBaseLightSettings liSet = GBaseAppSettings::getPtr ()->level.lvl.lights[i];
			lTemp = engine->getSceneManager()->createLight( liSet.name );
			switch( liSet.type )
			{
			case 0:
				lTemp->setType(Light::LT_POINT);
				break;
			case 1:
				lTemp->setType(Light::LT_DIRECTIONAL);
				lTemp->setDirection( liSet.direction.x, liSet.direction.y, liSet.direction.z );
				break;
			case 2:
				lTemp->setType(Light::LT_SPOTLIGHT);
				lTemp->setDirection( liSet.direction.x, liSet.direction.y, liSet.direction.z );
				lTemp->setSpotlightRange( Ogre::Degree(liSet.range.x), Ogre::Degree(liSet.range.y) );
				break;
			}
			lTemp->setDiffuseColour( liSet.diffuseColor.x, liSet.diffuseColor.y, liSet.diffuseColor.z );
			lTemp->setSpecularColour( liSet.specularColor.x, liSet.specularColor.y, liSet.specularColor.z );
			lTemp->setPosition( liSet.position.x, liSet.position.y, liSet.position.z );
			lTemp->setCastShadows( liSet.castShadows );

			BillboardSet* bbs = engine->getSceneManager()->createBillboardSet(liSet.name+std::string("bbs"), 1);
			bbs->setMaterialName("Examples/Flare");
			ColourValue myCol(0.8, 0.5, 0.0);
			Billboard* bb = bbs->createBillboard(0,0,0,myCol);
			bb->setColour(lTemp->getDiffuseColour());

			if( i<numLights-1 )
			{
				SceneNode * myNode = engine->getSceneManager()->createSceneNode(liSet.name+std::string("node"));
				myNode->attachObject( lTemp );
				myNode->attachObject( bbs );
			}
		}
	}

	void GGameState::destroyLights()
	{
		int numLights = GBaseAppSettings::getPtr ()->level.lvl.numLights;
		for( int i=0; i<numLights; i++ )
		{
			GBaseLightSettings liSet = GBaseAppSettings::getPtr ()->level.lvl.lights[i];

			if( i<numLights-1)
			{
				// detach from nodes
				engine->getSceneManager()->getSceneNode(liSet.name+std::string("node"))->removeAllChildren();
				engine->getSceneManager()->destroySceneNode(liSet.name+std::string("node"));
				engine->getSceneManager()->destroyBillboardSet( liSet.name+std::string("bbs"));
				engine->getSceneManager()->destroyLight(liSet.name);
			}
		}
	}

	void GGameState::loadLevelResources()
	{
		// Add the level folder to the resources.
		GTRACE(3, "..:: Loading level resources ::..");
		std::vector<std::string> resList;
		std::string sTemp = std::string("data/levels/") + GBaseAppSettings::getPtr ()->level.lvl.levelFolder;
		resList.push_back( sTemp );

		GBaseMainApp::addResourceLocationList( resList, LEVEL_GROUP );
		ResourceGroupManager::getSingleton().initialiseResourceGroup(LEVEL_GROUP);
		ResourceGroupManager::getSingleton().loadResourceGroup(LEVEL_GROUP);
	}

	void GGameState::unloadLevelResources()
	{
		ResourceGroupManager::getSingleton().unloadResourceGroup(LEVEL_GROUP);
		ResourceGroupManager::getSingleton().clearResourceGroup(LEVEL_GROUP);
		ResourceGroupManager::getSingleton().destroyResourceGroup(LEVEL_GROUP);

		GBaseAppSettings::getPtr ()->unloadLevel();
	}


	void GGameState::createLevelObjects()
	{
		// create additional level objects
		
		for( unsigned i=0; i< GBaseAppSettings::getPtr ()->level.lvl.wobs.size(); i++ )
		{
			GBaseWorldObjectSettings wobSet = GBaseAppSettings::getPtr ()->level.lvl.wobs[i];
			switch( wobSet.type )
			{
			case 0:
			default:
				{
					// plane type
					Ogre::Plane planeTemp;
					planeTemp.normal = Ogre::Vector3::UNIT_Y;
					planeTemp.d = wobSet.position.y;
					GTRACE(4, "creating object : ["<<wobSet.name<<"]");
					GTRACE(5, "size=: ["<<wobSet.size.x<<"]["<<wobSet.size.z<<"] seg=["<<wobSet.seg.x<<"]["<<wobSet.seg.z<<"]");
					Ogre::MeshManager::getSingleton().createPlane( wobSet.name,
						ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, // object belongs to the level's resource group so it will be destroyed along with it.
						planeTemp,
						wobSet.size.x, wobSet.size.z, 
						wobSet.seg.x, wobSet.seg.z,
						true, 1, 1, 1, Ogre::Vector3::UNIT_Z);

					// create the entity, node and attach them to the scene.
					Entity * eTemp;
					eTemp = engine->getSceneManager()->createEntity( wobSet.name, wobSet.name );
					std::string tempNodeName = wobSet.name + "node";
					Ogre::SceneNode *snTemp = engine->getSceneManager()->getRootSceneNode()->createChildSceneNode(tempNodeName);
					snTemp->attachObject(eTemp);
					//eTemp->setNormaliseNormals(true);
					snTemp->translate(globalTranslationVector);
					snTemp->translate(wobSet.position.x, 0, wobSet.position.z);
					if( wobSet.renderGroup != 0 )
					{
						eTemp->setRenderQueueGroup( wobSet.renderGroup );
					}
					if( wobSet.material != "" )
					{
						eTemp->setMaterialName( wobSet.material );
					}
				}
			}
		}
	}

	void GGameState::destroyLevelObjects()
	{
		for( unsigned i=0; i< GBaseAppSettings::getPtr ()->level.lvl.wobs.size(); i++ )
		{
			GBaseWorldObjectSettings wobSet = GBaseAppSettings::getPtr ()->level.lvl.wobs[i];
			Ogre::MeshManager::getSingleton().remove( wobSet.name );
			std::string tempNodeName = wobSet.name + "node";
			GTRACE(4, "removing children of map object node");
			engine->getSceneManager()->getSceneNode(tempNodeName)->removeAndDestroyAllChildren();
			GTRACE(4, "removing map object node");
			engine->getSceneManager()->destroySceneNode(tempNodeName);
			GTRACE(4, "removing entity of map object node");
			engine->getSceneManager()->destroyEntity(wobSet.name);
		}
	}


	// -------------- overloads

	void GGameState::enter(const GAny & param)
	{
		GTRACE(4, "Entering game state...");

		levelToLoad = any_cast<std::string>(param);

		guiWasRenderedOnce = false;
		loadingWasDone = false;

		// Show the loading screen.
		engine->showLoadingScreen(true);

		SoundManager::getPtr ()->getMusicPlayer ()->setListener (this);

		// ------ set up the LUA logic --------
		{
			GTRACE(LTL, "getting lua mutex (enter)");
			guslib::GScopedLock myLock(luaAccessMutex);
			GTRACE(LTL, "got lua mutex (enter)");
			GTRACE(2, "Setting up lua script for game...");
			L = lua_open();
			registerLuaFunctions(L);
			luaL_openlibs(L);

			// open the game lua script.
			int n = luaL_loadfile(L, "game.lua");
			if( !n )
			{
				n = lua_pcall(L, 0, LUA_MULTRET, 0);
			}
			else
			{
				GTRACE(1, "Exception: luaL_loadfile fail! "<<lua_tostring(L, -1));
				throw( std::exception("Unable to load lua script!"));
			}
		}


		GTRACE(4, "Entered game state...");
	}

	void GGameState::registerLuaFunctions( lua_State *L )
	{
		lua_pushcfunction(L, l_soundPlay);
		lua_setglobal(L, "soundPlay");
		lua_pushcfunction(L, GGameState::l_addUIFadeText);
		lua_setglobal(L, "addUIFadeTextWidget");
		lua_pushcfunction(L, GGameState::l_addTemporaryEffect);
		lua_setglobal(L, "addTemporaryEffect");
		lua_pushcfunction(L, GGameState::l_setUIFadeText);
		lua_setglobal(L, "setUIFadeTextWidgetText");
		lua_pushcfunction(L, GGameState::l_setOverlayVisible);
		lua_setglobal(L, "setOverlayVisible");
		lua_pushcfunction(L, GGameState::l_setOverlayElementVisible);
		lua_setglobal(L, "setOverlayElementVisible");
		lua_pushcfunction(L, GGameState::l_setOverlayElementCaption);
		lua_setglobal(L, "setOverlayElementCaption");
		lua_pushcfunction(L, GGameState::l_setCustomWidgetValue);
		lua_setglobal(L, "setCustomWidgetValue");
		lua_pushcfunction(L, GGameState::l_addCustomWidget);
		lua_setglobal(L, "addCustomWidget");
		lua_pushcfunction(L, GGameState::l_pushUIEvent);
		lua_setglobal(L, "pushUIEvent");
	}

	void GGameState::exit()
	{
		GTRACE(4, "Exiting game state...");
		SoundManager::getPtr ()->getMusicPlayer ()->setListener (NULL);
		stopGameSession();
		GTRACE(4, "Completing exit game state...");
		// ------ clean up the LUA logic --------
		lua_close(L);
	}

	void GGameState::pause() 
	{
		GTRACE(4, "Pausing game state...");
		if( gameAlgo )
		{
			gameAlgo->pause();
		}
		hideOverlays();
	}
	void GGameState::resume()
	{
		GTRACE(4, "Resuming game state...");
		if( gameAlgo )
		{
			gameAlgo->lapseTime();
			gameAlgo->resume();
		}
		showOverlays();
	}

	void GGameState::reactToKeyPress(OIS::KeyCode keycode)
	{
		GTRACE(4, "GGameState:reactToKeyPress("<<keycode<<")");
		using namespace OIS;
		switch( keycode )
		{
		case KC_ESCAPE:
			{
				// Show the in-game menu.
				guslib::GScopedLock myOtherLock(logicEventProcessMutex);
				logicEventQueue.push( new GBaseEvent(GET_InGameMenu, "") );
			}
			break;
		default:
			break;
		}
	}

	void GGameState::reactToKeyRelease(OIS::KeyCode keycode)
	{
		GTRACE(5, "GGameState:reactToKeyRelease("<<keycode<<")");
	}


	void GGameState::handleKeys( guslib::TimeUnits passedTime )
	{
		if( !engine )
			return;
		if( engine->getFrameListener()->getOISKeyboard()->isKeyDown(OIS::KC_MINUS) )
		{
			if( playerCamera )
			{
				double newZoomLevel = 0;
				newZoomLevel = playerCamera->getZoomLevel()
					- passedTime * playerCamera->getZoomStep() /1000;
				playerCamera->setZoomLevel( newZoomLevel );
			}
		}
		if( engine->getFrameListener()->getOISKeyboard()->isKeyDown(OIS::KC_EQUALS) )
		{
			if( playerCamera )
			{
				double newZoomLevel = 0;
				newZoomLevel = playerCamera->getZoomLevel()
						+ passedTime * playerCamera->getZoomStep() /1000;
				playerCamera->setZoomLevel( newZoomLevel );
			}
		}
		if( singleSerpent )
		{
			if( engine->getFrameListener()->getOISKeyboard()->isKeyDown(OIS::KC_LEFT) )
			{
				singleSerpent->turn(  0.1 * passedTime * singleSerpent->getTurnSpeed(), false );
			}
			if( engine->getFrameListener()->getOISKeyboard()->isKeyDown(OIS::KC_RIGHT) )
			{
				singleSerpent->turn( -0.1 * passedTime * singleSerpent->getTurnSpeed(), false );
			}
			if( engine->getFrameListener()->getOISKeyboard()->isKeyDown(OIS::KC_UP) )
			{
				singleSerpent->setSpeed( singleSerpent->getSpeed() + singleSerpent->getSpeedStep()* 0.1 * passedTime  );
				OnPlayerSpeedChange( singleSerpent->getSpeed() );
			}
			if( engine->getFrameListener()->getOISKeyboard()->isKeyDown(OIS::KC_DOWN) )
			{
				singleSerpent->setSpeed( singleSerpent->getSpeed() - singleSerpent->getSpeedStep() * 0.1 * passedTime );
				OnPlayerSpeedChange( singleSerpent->getSpeed() );
			}
		}
	}

	bool GGameState::handleLogicThreadLoop()
	{
		static bool lastWasActive=true;
		if(!engine)
			return false;
		if(!engine->isWindowActive())
		{
			// is inactive now.
			if( lastWasActive )
			{
				// just became inactive; pause the processing.
				if( gameAlgo )
				{
					gameAlgo->pause();
				}
			}
		}
		else
		{
			//is active now.
			if( !lastWasActive )
			{
				// just became active; resume the processing.
				if( gameAlgo )
				{
					gameAlgo->lapseTime();
					gameAlgo->resume();
				}
			}
		}
		lastWasActive = engine->isWindowActive();
		// handle some pre-rendering actions; this is done only once.
		if( guiWasRenderedOnce && !loadingWasDone )
		{
			startGameSession();

			loadingWasDone = true;
			engine->showLoadingScreen(false);
			showOverlays();
			GTRACE(3, "Initializing game state...");
			gameAlgo = NULL;

			gameMap = createMap();
			bonusManager = createBonusManager();
			singleSerpent = createSerpent();
			if( scoringTable )
			{
				std::string hiScoreFile = std::string("data/levels/")
					+ GBaseAppSettings::getPtr ()->level.lvl.levelFolder
					+ std::string("/records.txt");
				scoringTable->loadFromFile( hiScoreFile );
				scoringTable->setListener(this);
			}
			
			gameAlgo = new GGameAlgo();

			gameAlgo->setBonusManager( bonusManager );
			gameAlgo->setSerpent( singleSerpent );
			gameAlgo->setMap( gameMap );
			gameAlgo->setEngine( engine );
			gameAlgo->setScoringTable( scoringTable );

			gameAlgo->setStateListener( this );

			GTRACE(3, "Starting game algo...");
			if( !gameAlgo->start() )
			{
				GTRACE(1, "ERROR! unable to start game!");
			}

			//reset the key timer, so that the first time the player presses something it won't be considered as having been pressed for hours.
			keyTimer.reset();

			GTRACE(5, "GGameState::handleLogicThreadLoop() exiting (done loading).");
			return true;
		}

		// create a sub-scope for the lock operation.
		{
			GTRACE(5, "GGameState::handleLogicThreadLoop() entered; getting logicEventProcessMutex");
			guslib::GScopedLock myOtherLock(logicEventProcessMutex);
			GTRACE(5, "GGameState::handleLogicThreadLoop() locked logicEventProcessMutex");
			// handle the events
			if( logicEventQueue.size() == 0 )
			{
				// We have no events.
				GTRACE(5, "GGameState: handleLogicThreadLoop (preRender): no events.");
				//return true;
			}
			else
			{
				// We have some events
				GBaseEvent * myEvent = logicEventQueue.front();
				if( myEvent == NULL )
				{
					GTRACE(5, "WARNING: GGameState: preRender, has null event in queue: exiting");
					return false;
				}

				switch( myEvent->getType() )
				{
				case GET_Exit:
					GBaseStateManager::getPtr()->popStateNextTick();
					break;
				case GET_InGameMenu:
					GBaseStateManager::getPtr()->pushStateNextTick( "ingamemenu" );
					break;
				case GET_Hiscore:
					// a game was finished and the player entered the hiscore list;
					// don't push the state, because we don't need to return to the game, but to the menu afterwards
					GBaseStateManager::getPtr()->setStateNextTick( "hiscore", true );
					break;
				default:
					break;
				}
				logicEventQueue.pop();
				delete myEvent;
			}
		}//lock released.

		// do the game loop

		try
		{
			if( gameAlgo )
			{
				gameAlgo->lapseTime();
			}
			if( firsttimeLenflag )
			{
				try
				{
					if( singleSerpent )
					{
						//also attach a light to the serpent.
						VisualBodyPart * myPart = (VisualBodyPart*) singleSerpent->getBodyPart(0);
						Light * ltemp = engine->getSceneManager()->getLight(c_nameOfPlayerLight);
						Ogre::SceneNode *node = engine->getSceneManager()->getSceneNode( myPart->getSceneNodeName() );
						DFLOAT myX = GBaseAppSettings::getPtr ()->level.lvl.playerSpawn.x;
						DFLOAT myY = GBaseAppSettings::getPtr ()->level.lvl.playerSpawn.y;
						ltemp->setPosition( myX, c_heightOfLight, myY);
						node->attachObject( ltemp );
						firsttimeLenflag = false;
					}
				}
				catch( std::exception & e )
				{
					GTRACE(1, "Exception: "<<e.what());
				}
			}
			guslib::TimeUnits duration = keyTimer.renew();
			for( FadingMessageMap::iterator it = fadeItems.begin();
				it!= fadeItems.end(); ++it )
			{
				it->second.onTimeGoesBy( duration );
			}
			for( CustomWidgetMap::iterator it = widgets.begin();
				it!= widgets.end(); ++it )
			{
				it->second.get()->elapseTime( duration );
			}
			handleKeys(duration);
		}
		catch( Exception &e )
		{
			GTRACE(1, "Exception caught:"<<e.getFullDescription());
		}

		GTRACE(5, "GGameState::handleLogicThreadLoop() exiting");
		return true;
	}


	GRenderSnapshotView & GGameState::getRenderSnapshot()
	{
		tempView.data.clear();
		if( singleSerpent )
		{
			for( int i=0; i<singleSerpent->getLength(); i++ )
			{
				VisualBodyPart * myPart = (VisualBodyPart*) singleSerpent->getBodyPart(i);
				if( myPart == NULL )
					continue;
				bool isVisible = myPart->isSpawned();
				if( isVisible )
				{
					myPart->setVisible( true );
					GOgreRenderable *myR = myPart->createTemporaryClone();
					tempView.data.push_back( *myR );
				}
				else
				{
					myPart->setVisible( false );
				}

				if( i==0 )
				{
					if( playerCamera )
					{
						int x = myPart->getLocation().x;
						int y = myPart->getLocation().y;
						playerCamera->slideToLocation( Vector3(x, 0, y) );
					}
				}
			}
		}
		// draw the bonuses
		if( bonusManager )
		{
			for( int i=0; i<bonusManager->getNumberOfItems(); i++ )
			{
				VisualBonus * myBonus = (VisualBonus*) bonusManager->getItemAt(i);
				if( myBonus == NULL )
					continue;
				bool isVisible = myBonus->getTimeToLive() > 0;
				if( isVisible )
				{
					myBonus->setVisible( true );
					GOgreRenderable *myR = myBonus->createTemporaryClone();
					tempView.data.push_back( *myR );
				}
				else
				{
					myBonus->setVisible( false );
				}
			}
		}
		return tempView;
	}


	bool GGameState::renderScene()
	{
		// do the preloads
		GBase::GResourceManager::getPtr()->processRequests();
		GRenderSnapshotView aux = getRenderSnapshot();

		for( std::vector<GOgreRenderable>::iterator it = aux.data.begin();
			it!= aux.data.end(); ++it )
		{
			it->render();
		}
		return true;
	}


	bool GGameState::renderGUI()
	{
		GTRACE(5, "GGameState::renderGUI() entered");
		// If this is the first pass, just render once (to see the loading screen).
		if( !guiWasRenderedOnce )
		{
			guiWasRenderedOnce = true;
			GTRACE(5, "GGameState: render overlay exiting (first pass)");
			return true;
		}

		// Make sure all timed Fading items get rendered.
		for( FadingMessageMap::iterator it = fadeItems.begin();
			it!= fadeItems.end(); ++it )
		{
			it->second.render();
		}
		for( CustomWidgetMap::iterator it = widgets.begin();
			it!= widgets.end(); ++it )
		{
			it->second.get()->render();
		}

		// update stats when necessary
		try 
		{
			static String currFps = "FPS: ";
			OverlayElement * fpsCurr = OverlayManager::getSingleton().getOverlayElement("GusApp/FPSText");
			if( fpsCurr != 0 )
			{
				const RenderTarget::FrameStats& stats = engine->getRenderWindow()->getStatistics();
				fpsCurr->setCaption( currFps + StringConverter::toString((int)stats.lastFPS) );
			}

			// process all ui events.
			guslib::GScopedLock myLock( uiEventProcessMutex );
			while( !uiEventQueue.empty() )
			{
				GSimpleEvent * myEvent = uiEventQueue.front();
				if( myEvent == NULL )
				{
					GTRACE(4, "WARNING: GGameState: renderGUI, has null event in queue: exiting");
					break;
				}
				// handleEvent
				{
					bool useProcessing = true;
					GTRACE(LTL, "getting lua mutex (rendergui)");
					if( !luaAccessMutex.try_lock())
					{
						GTRACE(4, "Unable to get lock while rendering gui... skipping proc");
						useProcessing = false;
						//todo: add to secondary queue for processing next frame.
					}

					if( useProcessing )
					{
						GTRACE(LTL, "got lua mutex (rendergui)");

						lua_getglobal(L, "renderThread_handleUIEvent");  // function to be called
						lua_pushinteger(L, myEvent->getType());   // push the arguments
						lua_pushstring(L, myEvent->getDescription().c_str());
						lua_pushnumber(L, myEvent->getParam());
						GTRACE(4, "calling handling of evt "<<myEvent->getType());
						if (lua_pcall(L, 3, 0, 0) != 0)
						{
							GTRACE(1, "Exception: renderThread_handleUIEvent:"<<lua_tostring(L, -1));
							luaAccessMutex.unlock();
							throw( std::exception("error running function renderThread_handleUIEvent:"));
						}
						luaAccessMutex.unlock();
					}
				}
					
				uiEventQueue.pop();
				delete myEvent;	
			}
		}
		catch(std::exception & e)
		{
			GTRACE(1, "Exception:"<<e.what());
		}
		
		GTRACE(5, "GGameState: render overlay exiting");
		return true;
	}

	void GGameState::notifyOfShutdown()
	{
	}



	// gameplay stuff

	Map * GGameState::createMap()
	{
		GTRACE(2, "Creating map for GGameState" );
		IPOINT mapCells;
		mapCells.x = GBaseAppSettings::getPtr ()->level.lvl.worldCells.x;
		mapCells.y = GBaseAppSettings::getPtr ()->level.lvl.worldCells.y;

		Map * mTemp = new Map( GBaseAppSettings::getPtr ()->level.lvl.worldSize.x,
			GBaseAppSettings::getPtr ()->level.lvl.worldSize.y, mapCells );

		DRECT dropArea;
		dropArea.left = GBaseAppSettings::getPtr ()->level.lvl.dropZoneFrom.x;
		dropArea.top = GBaseAppSettings::getPtr ()->level.lvl.dropZoneFrom.y;
		dropArea.right = GBaseAppSettings::getPtr ()->level.lvl.dropZoneTo.x;
		dropArea.bottom = GBaseAppSettings::getPtr ()->level.lvl.dropZoneTo.y;

		DRECT damageArea;
		damageArea.left = GBaseAppSettings::getPtr ()->level.lvl.damageZoneFrom.x;
		damageArea.top = GBaseAppSettings::getPtr ()->level.lvl.damageZoneFrom.y;
		damageArea.right = GBaseAppSettings::getPtr ()->level.lvl.damageZoneTo.x;
		damageArea.bottom = GBaseAppSettings::getPtr ()->level.lvl.damageZoneTo.y;

		DRECT safeArea;
		safeArea.left = GBaseAppSettings::getPtr ()->level.lvl.safeZoneFrom.x;
		safeArea.top = GBaseAppSettings::getPtr ()->level.lvl.safeZoneFrom.y;
		safeArea.right = GBaseAppSettings::getPtr ()->level.lvl.safeZoneTo.x;
		safeArea.bottom = GBaseAppSettings::getPtr ()->level.lvl.safeZoneTo.y;

		mTemp->setBonusDropArea( dropArea );
		mTemp->setDamageArea( damageArea );
		mTemp->setSafeArea( safeArea );

		mTemp->setDamageAreaDamageRatio( GBaseAppSettings::getPtr ()->level.lvl.damageZoneDamage );
		return mTemp;
	}

	VisualSerpent * GGameState::createSerpent()
	{
		VisualSerpent *ret;
		GTRACE(4, "Creating visual serpent...");
		ret = new VisualSerpent( engine->getSceneManager(), engine );
		ret->setHeadConfig( GBaseAppSettings::getPtr ()->model.srphead );
		ret->setBodyConfig( GBaseAppSettings::getPtr ()->model.srpbody );
		return ret;
	}

	VisualBonusManager * GGameState::createBonusManager()
	{
		GTRACE(4, "Creating bonus manager...");
		GTRACE(6, "GGameState::createBonusManager getting load and render mutex");
		guslib::GScopedLock( engine->getLoadAndRenderMutex() );
		GTRACE(6, "GGameState::createBonusManager got load and render mutex");
		VisualBonusManager * ret;
		ret = new VisualBonusManager( engine->getSceneManager(), engine );

		ret->setMaxValueForSpawnRandom( GBaseAppSettings::getPtr ()->level.lvl.bonusSpawnOddsMax );
		ret->setGenValueForSpawnRandom( GBaseAppSettings::getPtr ()->level.lvl.bonusSpawnOdds );
		ret->setDefaultTimeToLive( GBaseAppSettings::getPtr ()->level.lvl.maxBonusLifetime );
		ret->setMaximumAllowedBonuses( GBaseAppSettings::getPtr ()->level.lvl.maxBonusesOnScreen );

		int num = (int)modelCfgHolder.bonus.size();
		for( int i=0; i<num; i++ )
		{
			std::string bonusPropType = modelCfgHolder.bonus[i].bonusType;
			//int iBonusProp;
			int odds = modelCfgHolder.bonus[i].odds;
			Ogre::Vector3 animVector = modelCfgHolder.bonus[i].animRotationVector;
			Ogre::Vector3 rotationVector = modelCfgHolder.bonus[i].rotationVector;
			Ogre::Vector3 scalingVector = modelCfgHolder.bonus[i].scalingVector;
			Ogre::Vector3 translationVector = modelCfgHolder.bonus[i].translationVector;

			int numProps = modelCfgHolder.bonus[i].numProps;
			std::string matName = modelCfgHolder.bonus[i].nameOfMaterial;
			std::string meshName = modelCfgHolder.bonus[i].nameOfMesh;
			std::string stencilMatName = modelCfgHolder.bonus[i].stencilMaterial;
			Ogre::uint8 renderGrp = modelCfgHolder.bonus[i].renderGroup;
			Ogre::uint8 stencilGrp = modelCfgHolder.bonus[i].stencilGroup;

			BonusRewardList props;
			for( int j=0; j< numProps; j++ )
			{
				std::string sTemp = modelCfgHolder.bonus[i].properties[j].type;
				double iTemp = modelCfgHolder.bonus[i].properties[j].value;
				BonusRewardType btt = getRewardTypeFromString( sTemp );
				props.push_back( BonusReward(btt, "", iTemp) );
			}

			VisualBonusPrototype * myProto = (VisualBonusPrototype*) ret->createPrototype( bonusPropType );
			myProto->setGenerationOddsWeight( odds );
			myProto->setRewards( props );

			GTRACE(4, "Created bonus prototype for ["<<myProto->getInternalName()<<"]with properties...");
			for( unsigned j=0; j< myProto->getRewardsCopy().size(); j++ )
			{
				GTRACE(4, "Property " <<j<< ", type="<<getNameFromRewardType(myProto->getRewardsCopy()[j].getType())
					<<", value="<<myProto->getRewardsCopy()[j].getValue());
			}
			myProto->requestLoading( meshName, matName, stencilMatName, renderGrp, stencilGrp );

			myProto->setAnimRotationVector( animVector );
			myProto->setRotationVector( rotationVector );
			myProto->setScalingVector( scalingVector );
			myProto->setTranslationVector( translationVector );

			ret->addBonusPrototype( myProto );
 		}

		GTRACE(4, "created bonus manager for visual serpents engine.");
		GTRACE(6, "GGameState::createBonusManager releasing load and render mutex");
		return ret;	
	}



	void GGameState::setScoringTable( GScoringTable * aTable )
	{
		scoringTable = aTable;
	}


	// callbacks




	void GGameState::OnGameFinished()
	{
		GTRACE(2, "A game was finished.");
		guslib::GScopedLock myOtherLock(logicEventProcessMutex);

		if( scoringTable && scoringTable->scoresWereChanged() )
		{
			GTRACE(4, "Signaled game finish with high score");
			logicEventQueue.push( new GBaseEvent(GET_Hiscore, "") );
			return;
		}

		GTRACE(4, "Signaled game finish");
		logicEventQueue.push( new GBaseEvent(GET_Exit, "") );
	}

	void GGameState::OnBonusSpawned( BonusBase * bns )
	{
		if( ! bns )
			return;
		GTRACE(6, "GGameState::OnBonusSpawned");
		GTRACE(LTL, "getting lua mutex (bonusspawn)");
		guslib::GScopedLock myLock(luaAccessMutex);
		GTRACE(LTL, "got lua mutex (bonusspawn)");

		// Prepare the the lua func.call
		std::string s = bns->getLinkToPrototype()->getInternalName();
		DPOINT pnt = bns->getLocation();
		lua_getglobal(L, "onBonusSpawn");  // function to be called
		lua_pushstring(L, s.c_str());   // push the argument
		lua_pushnumber(L, pnt.x);
		lua_pushnumber(L, pnt.y);
		// make the call, 3 arguments, no expected results to pop
		if (lua_pcall(L, 3, 0, 0) != 0)
		{
			GTRACE(1, "Exception: onBonusSpawn:"<<lua_tostring(L, -1));
			throw( std::exception("error running function onBonusSpawn:"));
		}
	}

	void GGameState::OnBonusPicked( BonusBase * bns )
	{
		if( ! bns )
			return;
		GTRACE(6, "GGameState::OnBonusPicked");
		GTRACE(LTL, "getting lua mutex (bonuspicked)");
		guslib::GScopedLock myLock(luaAccessMutex);
		GTRACE(LTL, "got lua mutex (bonuspicked)");

		// Prepare the the lua func.call
		std::string s = bns->getLinkToPrototype()->getInternalName();
		DPOINT pnt = bns->getLocation();
		lua_getglobal(L, "onBonusPickup");  // function to be called
		lua_pushstring(L, "self");   // push the 1st argument, picker
		lua_pushstring(L, s.c_str());   // push the 2nd argument, bonus
		GTRACE(4, "Picked bonus of type:"<<s);
		lua_pushnumber(L, pnt.x);   // push the location
		lua_pushnumber(L, pnt.y);   // 
		// make the call, 4 arguments, no expected results to pop
		if (lua_pcall(L, 4, 0, 0) != 0)
		{
			GTRACE(1, "Exception: onBonusPickup:"<<lua_tostring(L, -1));
			throw( std::exception("error running function onBonusPickup:"));
		}
	}

	void GGameState::OnPlayerScoreChange( long score )
	{
		GTRACE(LTL, "getting lua mutex (onPlayerScoreChange)");
		guslib::GScopedLock myLock(luaAccessMutex);
		GTRACE(LTL, "got lua mutex (onPlayerScoreChange)");

		lua_getglobal(L, "onPlayerScoreChange");  // function to be called
		lua_pushinteger(L, score);   // push the argument
		// make the call, 1 argument, no expected results to pop
		if (lua_pcall(L, 1, 0, 0) != 0)
		{
			GTRACE(1, "Exception: onPlayerScoreChange:"<<lua_tostring(L, -1));
			throw( std::exception("error running function onPlayerScoreChange:"));
		}
	}

	void GGameState::OnPlayerHealthChange( double percentage )
	{
		GTRACE(LTL, "getting lua mutex (onPlayerHealthChange)");
		guslib::GScopedLock myLock(luaAccessMutex);
		GTRACE(LTL, "got lua mutex (onPlayerHealthChange)");
		lua_getglobal(L, "onPlayerHealthChange");  // function to be called
		lua_pushnumber(L, percentage);   // push the argument
		// make the call, 1 argument, no expected results to pop
		if (lua_pcall(L, 1, 0, 0) != 0)
		{
			GTRACE(1, "Exception: onPlayerHealthChange:"<<lua_tostring(L, -1));
			throw( std::exception("error running function onPlayerHealthChange:"));
		}
	}

	void GGameState::OnPlayerHungerChange( double percentage )
	{
		GTRACE(LTL, "getting lua mutex (onPlayerHungerChange)");
		guslib::GScopedLock myLock(luaAccessMutex);
		GTRACE(LTL, "got lua mutex (onPlayerHungerChange)");
		lua_getglobal(L, "onPlayerHungerChange");  // function to be called
		lua_pushnumber(L, percentage);   // push the argument
		// make the call, 1 argument, no expected results to pop
		if (lua_pcall(L, 1, 0, 0) != 0)
		{
			GTRACE(1, "Exception: onPlayerHungerChange:"<<lua_tostring(L, -1));
			throw( std::exception("error running function onPlayerHungerChange:"));
		}
	}

	void GGameState::OnPlayerLengthChange( double newLen )
	{
		GTRACE(LTL, "getting lua mutex (onPlayerLengthChange)");
		guslib::GScopedLock myLock(luaAccessMutex);
		GTRACE(LTL, "got lua mutex (onPlayerLengthChange)");
		lua_getglobal(L, "onPlayerLengthChange");  // function to be called
		lua_pushnumber(L, newLen);   // push the argument
		// make the call, 1 argument, no expected results to pop
		if (lua_pcall(L, 1, 0, 0) != 0)
		{
			GTRACE(1, "Exception: onPlayerLengthChange:"<<lua_tostring(L, -1));
			throw( std::exception("error running function onPlayerLengthChange:"));
		}
	}


	void GGameState::OnPlayerSpeedChange( double newSpeed )
	{
		GTRACE(LTL, "getting lua mutex (onPlayerSpeedChange)");
		guslib::GScopedLock myLock(luaAccessMutex);
		GTRACE(LTL, "got lua mutex (onPlayerSpeedChange)");
		lua_getglobal(L, "onPlayerSpeedChange");  // function to be called
		lua_pushnumber(L, newSpeed);   // push the argument
		// make the call, 1 argument, no expected results to pop
		if (lua_pcall(L, 1, 0, 0) != 0)
		{
			GTRACE(1, "Exception: onPlayerSpeedChange:"<<lua_tostring(L, -1));
			throw( std::exception("error running function onPlayerSpeedChange:"));
		}
	}

	

	void GGameState::OnScoringTablePositionChanged( long newPosition, long oldPosition )
	{
		GTRACE(LTL, "getting lua mutex (onPlayerHiscorePositionChange)");
		guslib::GScopedLock myLock(luaAccessMutex);
		GTRACE(LTL, "got lua mutex (onPlayerHiscorePositionChange)");

		lua_getglobal(L, "onPlayerHiscorePositionChange");  // function to be called
		lua_pushnumber(L, newPosition);   // push the argument
		lua_pushnumber(L, oldPosition);   // push the argument
		// make the call, 2 argument, no expected results to pop
		if (lua_pcall(L, 2, 0, 0) != 0)
		{
			GTRACE(1, "Exception: onPlayerHiscorePositionChange:"<<lua_tostring(L, -1));
			throw( std::exception("error running function onPlayerHiscorePositionChange:"));
		}
	}


	void GGameState::displaySongInfo( const std::string & newTrack )
	{
		// initialize the artist and song values with "unnown". they may remain like this if the tracks
		// don't carry any information.
		std::string myArtist("Unknown Artist");
		std::string mySong("Unknown Song");
		TagList tags = static_cast<GSmartPtr <GAudiere::GaudiereSound>>
			(SoundManager::getPtr ()->getRepository ()->getSound (newTrack))->getTags ();
		TagList::iterator iter = tags.find (std::string("artist"));
		if( iter != tags.end() )
		{
			myArtist = tags["artist"];
		}
		iter = tags.find(std::string("title"));
		if( iter != tags.end() )
		{
			mySong = tags["title"];
		}
		try
		{
			FadingMessageMap::iterator it = fadeItems.find("singer");
			if( it != fadeItems.end() )
			{
				it->second.setMessage( myArtist );
			}
			it = fadeItems.find("song");
			if( it != fadeItems.end() )
			{
				it->second.setMessage( mySong );
			}
		}
		catch(std::exception &e)
		{
			GTRACE(1, "Exception caught:"<<e.what());
		}
	}


	void GGameState::OnTrackPlay(const std::string & newTrack)
	{
		GTRACE(3, "Playing track:"<<newTrack);
		displaySongInfo(newTrack);
	}

	void GGameState::OnTrackChange(const std::string & newTrack)
	{
		GTRACE(3, "Track changing to:"<<newTrack);
		displaySongInfo(newTrack);
	}
	void GGameState::OnPlaylistEnd()
	{
	}

	int GGameState::l_addUIFadeText( lua_State *L )
	{
		GTRACE(4, "l_adding ui text fader");
		std::string itemName("");
		std::string ogreElement("");
		int fadeDuration(1000);
		{
			itemName= luaL_checkstring(L, 1);
			ogreElement= luaL_checkstring(L, 2);
			fadeDuration= luaL_checkint(L, 3);
		}

		if( itemName =="" || ogreElement == "" || fadeDuration<=0 )
		{
			GTRACE(2, "Warning: trying to create an invalid UI Fade Element! Params:"
				<<itemName<<", "<<ogreElement<<", "<<fadeDuration);
		}

		GGameState::getProtectedInstancePtr()->fadeItems.insert( 
			std::make_pair(itemName, FadingMessage(ogreElement, fadeDuration)) );
		return 1;
	}

	int GGameState::l_addTemporaryEffect( lua_State *L )
	{
		GTRACE(4, "l_adding temporary effect");
		std::string itemName("");
		std::string particleScript("");
		int x(0);
		int y(0);
		int effectDuration(1000);
		{
			particleScript= luaL_checkstring(L, 1);
			x = luaL_checkint(L, 2);
			y = luaL_checkint(L, 3);
			effectDuration= luaL_checkint(L, 4);
		}

		if( particleScript == "" || effectDuration<=0 )
		{
			GTRACE(2, "Warning: trying to create an invalid UI Fade Element! Params:"
				<<itemName<<", "<<particleScript<<", "<<effectDuration);
		}

		//GGameState::getProtectedInstancePtr()->addEffect( new Effect(particleScript, x, y, effectDuration)
		return 1;
	}

	int GGameState::l_setUIFadeText( lua_State *L )
	{
		GTRACE(4, "l_setting ui fade text");
		std::string itemName("");
		std::string content("");
		{
			itemName= luaL_checkstring(L, 1);
			content= luaL_checkstring(L, 2) ;
		}
		FadingMessageMap::iterator it = GGameState::getProtectedInstancePtr()->fadeItems.find(itemName);
		if( it != GGameState::getProtectedInstancePtr()->fadeItems.end() )
		{
			it->second.setMessage( content );
		}
		return 1;
	}

	int GGameState::l_setOverlayVisible( lua_State *L )
	{
		GTRACE(4, "l_setting overlay visibility");
		std::string itemName("");
		int visible;
		{
			itemName= luaL_checkstring(L, 1) ;
			visible= luaL_checkint(L, 2) ;
		}
		Overlay * temp  = OverlayManager::getSingletonPtr()->getByName(itemName);
		if( !temp )
		{
			GTRACE(2, "Warning: tried to set visibility for non-existing overlay:" << itemName);
			return 0;
		}
		if( visible) temp->show();
		else temp->hide();
		return 1;
	}

	int GGameState::l_setOverlayElementVisible( lua_State *L )
	{
		GTRACE(4, "l_setting overlay item visibility");
		int visible;
		std::string itemName("");
		{
			itemName= luaL_checkstring(L, 1) ;
			visible = luaL_checkint(L, 2);
		}
		OverlayElement *temp = OverlayManager::getSingletonPtr()->getOverlayElement(itemName);
		if( ! temp )
		{
			GTRACE(2, "Warning: tried to set visibility for non-existing element:" << itemName);
			return 0;
		}
		if( visible) temp->show();
		else temp->hide();
		return 1;
	}

	int GGameState::l_setOverlayElementCaption( lua_State *L )
	{
		GTRACE(4, "l_setting overlay item caption");
		std::string itemName("");
		std::string caption("");
		{
			itemName=luaL_checkstring(L, 1);
			caption=luaL_checkstring(L, 2);
		}
		OverlayElement *temp = OverlayManager::getSingletonPtr()->getOverlayElement(itemName);
		if( ! temp )
		{
			GTRACE(2, "Warning: tried to set caption for non-existing element:" << itemName);
			return 0;
		}
		temp->setCaption( caption );
		return 1;
	}

	int GGameState::l_addCustomWidget( lua_State *L )
	{
		GTRACE(4, "l_adding custom widget");
		std::string itemName("");
		std::string ogreElement1("");
		std::string ogreElement2("");
		//double value(0);
		{
			itemName=luaL_checkstring(L, 1);
			ogreElement1=luaL_checkstring(L, 2);
			ogreElement2=luaL_checkstring(L, 3);
		}
		
		if( itemName =="" || (ogreElement1 == "" && ogreElement2 == ""))//2nd and 3rd can't both be empty
		{
			GTRACE(2, "Warning: trying to create an invalid UI Fade Element! Params:"
				<<itemName<<", "<<ogreElement1 <<", "<<ogreElement2);
		}
		
		CustomWidgetPtr tempptr = GSharedPtr<CustomWidget>(new HealthWidget(ogreElement1, ogreElement2));
		GGameState::getProtectedInstancePtr()->widgets.insert( 
			std::make_pair(itemName, tempptr) );
		return 1;
	}


	int GGameState::l_setCustomWidgetValue( lua_State *L )
	{
		GTRACE(4, "l_setting custom widget value");
		std::string itemName("");
		double value(0);
		{
			itemName=luaL_checkstring(L, 1);
			value=luaL_checknumber(L, 2);
		}
		CustomWidgetMap::iterator it = GGameState::getProtectedInstancePtr()->widgets.find(itemName);
		if( it != GGameState::getProtectedInstancePtr()->widgets.end() )
		{
			it->second.get()->setValue( value );
			return 1;
		}
		return 0;
	}

	int GGameState::l_pushUIEvent(lua_State *L)
	{
		GTRACE(4, "l_pushing ui event");
		int evtType;
		std::string desc("");
		DFLOAT evtParam;
		{
			evtType= luaL_checkint(L, 1);
			desc= luaL_checkstring(L,2);
			evtParam= luaL_checknumber(L, 3);
		}

		getProtectedInstancePtr()->pushUIEvent( new GSimpleEvent( evtType, desc, evtParam ) );
		return 1;
	}

	void GGameState::pushUIEvent( GBase::GSimpleEvent* evt )
	{
		if( !evt ) return;
		guslib::GScopedLock myLock( uiEventProcessMutex );
		uiEventQueue.push( evt );
	}
}
