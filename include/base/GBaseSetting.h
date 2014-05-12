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

// TODO: use iniconfig class instead!
#include "guslib/thirdparty/SimpleIni.h"
//#include "GGeneralUtils.h"
#include <vector>
#include <map>
#include <Ogre.h>
#include <guslib/system/thread.h>
#include <guslib/common/singleton.hpp>

namespace GBase
{
	// The video modes.

	const std::string Direct3D = "Direct3D";
	const std::string OpenGL = "OpenGL";

	// GAudiere specific settings...

	#define GBASESET_GAUDIERE_MASTERVOL "gaudiere.mastervol"
	#define GBASESET_GAUDIERE_MUSICVOL "gaudiere.musicvol"
	#define GBASESET_GAUDIERE_VOICEVOL "gaudiere.voicevol"
	#define GBASESET_GAUDIERE_FXVOL "gaudiere.fxvol"

	
	#define G_MAX_OPTION_LIST_SUBITEMS_COUNT 100

	typedef std::vector<std::string> StringList;

	/// This configures a 3d object.
	typedef struct VisibleEntityConfigStruct
	{
		std::string nameOfMaterial;
		std::string nameOfMesh;
		std::string stencilMaterial;
		Ogre::uint8 renderGroup;
		Ogre::uint8 stencilGroup;
		Ogre::Vector3 translationVector;
		Ogre::Vector3 rotationVector;
		Ogre::Vector3 animRotationVector;	//	TODO: This is only used for the bonuses, so it's basically just wasting 
		//	memory for the others. Should be fixed in next version. (2007.10.09)
		Ogre::Vector3 scalingVector;
	}VisibleEntityConfig;

	/// Utility struct to be used as an entry for a mapping, or map type.
	typedef struct PropertyConfigStruct
	{
		std::string type;
		double value;
	}PropertyConfig;

	/// Configuration for a bonus, containing all necessary input to create the prototypes.
	typedef struct BonusConfigStruct
	{
	public:
		std::string nameOfMaterial;
		std::string nameOfMesh;
		std::string stencilMaterial;
		Ogre::uint8 renderGroup;
		Ogre::uint8 stencilGroup;
		Ogre::Vector3 translationVector;
		Ogre::Vector3 rotationVector;
		Ogre::Vector3 animRotationVector;	//	TODO: This is only used for the bonuses, so it's basically just wasting 
		//	memory for the others. Should be fixed in next version. (2007.10.09)
		Ogre::Vector3 scalingVector;
		std::string bonusType;
		int numProps;
		int odds;
		std::vector<PropertyConfig> properties;
	}BonusConfig;

	typedef struct GBaseSoundSettingsStruct
	{
		double masterVolume;
		double musicVolume;
		double voiceVolume;
		double fxVolume;
	}GBaseSoundSettings;

	typedef struct GBaseResourceSettingsStruct
	{
		std::string pluginDir;
		StringList plugins;
		StringList preload;
		StringList resources;
	}GBaseResourceSettings;

	typedef struct GBaseLogSettingsStruct
	{
		std::string ogreLog;
		std::string traceFile;
		int traceLevel;
	}GBaseLogSettings;

	typedef struct GBaseVideoSettingsStruct
	{
		bool vsync;			// vertical sync, only valid for fullscr.
		bool fullscreen;
		int windowWidth;
		int windowHeight;
		int fsaa;			// fullscreen antialiasing
		int refreshRate;	// not specifiable in all video modes
		int colourDepth;    // used mainly for the Direct3D mode
		std::string renderEngine;
		std::string renderEngineFullName;
		std::string renderModeString;
	}GBaseVideoSettings;

	typedef struct GBasePerfSettingsStruct
	{
		int delayBetweenRenders;
		bool showFPS;
		bool useShadows;
		int shadowType;
		int meshQuality; // 0 = low quality, higher values mean higher quality.
		int cpuSleep; // amount of sleep in millis;
		bool postProcessing;
		StringList compositorList;
	}GBasePerfSettings;


	/**
		The settings are given as class members.
		A nicer approach may be chosen sometime in the future, but for now, these settings will stay the way they are.
		The priority is to get to each of the settings in a time as close to zer0 as possible, since some of these
		settings may be queried on a constant basis, with each frame.
	*/
	class GBaseAppSettingHolder
	{
	protected:
		std::string fileName; /// the name of the file whence the settings are loaded.
		bool bConfigWasLoaded; /// flag that says wether the config was loaded or not.

		// functions that make the process of reading option values from the simple ini much easier.
		virtual double getFloatValue(CSimpleIniA * si, std::string category, std::string key, std::string defaultValue);
		virtual std::string getStringValue(CSimpleIniA * si, std::string category, std::string key, std::string defaultValue);
		virtual int getIntValue(CSimpleIniA * si, std::string category, std::string key, std::string defaultValue);
		virtual bool getBoolValue(CSimpleIniA * si, std::string category, std::string key, std::string defaultValue);
		virtual StringList getIndexedList(CSimpleIniA * si, std::string category, std::string key, int nFrom);
		virtual Ogre::Vector3 getVector3(CSimpleIniA * si, std::string category, std::string key, Ogre::Vector3 defaultValue);
		virtual Ogre::Vector2 getVector2(CSimpleIniA * si, std::string category, std::string key, Ogre::Vector2 defaultValue);

		// functions that make the process of writing option values to the simple ini much easier.
		virtual void setFloatValue(CSimpleIniA * si, std::string category, std::string key, double value);
		virtual void setStringValue(CSimpleIniA * si, std::string category, std::string key, std::string value);
		virtual void setIntValue(CSimpleIniA * si, std::string category, std::string key, int value);
		virtual void setBoolValue(CSimpleIniA * si, std::string category, std::string key, bool value);
		// the nFrom should be zero normally.
		virtual void setIndexedList(CSimpleIniA * si, std::string category, std::string key, int nFrom, StringList value);
		virtual void setCoords3(CSimpleIniA * si, std::string category, std::string key, Ogre::Vector3 value);
		virtual void setCoords2(CSimpleIniA * si, std::string category, std::string key, Ogre::Vector2 value);


	public:
		GBaseAppSettingHolder();
		virtual ~GBaseAppSettingHolder();
		virtual void load( CSimpleIniA *si );
		virtual void save( CSimpleIniA *si );
	};


	typedef struct GBaseCameraSettingsStruct
	{
		Ogre::Vector3 cam1Pos;
		Ogre::Vector3 cam1LookAt;
		Ogre::Vector3 cam2Pos;
		Ogre::Vector3 cam2LookAt;
		double zoomMax;
		double zoomStep;
		double zoomLevel;
		double fov;
		int defaultMode;
	}GBaseCameraSettings;


	typedef struct GBaseRenderGroupEntitySettingsStruct
	{
		Ogre::uint8 id;
		Ogre::CompareFunction compareFunc;
		Ogre::StencilOperation stencilFailOp;
		Ogre::StencilOperation depthFailOp;
		Ogre::StencilOperation passOp;
		int value;
	}GBaseRenderGroupEntitySettings;


	typedef struct GBaseRenderGroupSettingsStruct
	{
		std::vector<GBaseRenderGroupEntitySettings> groups;
		Ogre::uint8 firstID;
		Ogre::uint8 lastID;
	}GBaseRenderGroupSettings;

	typedef struct GBaseLightSettingsStruct
	{
		bool castShadows;
		int type;

		Ogre::Vector3 diffuseColor;
		Ogre::Vector3 specularColor;
		Ogre::Vector3 position;
		Ogre::Vector3 direction;
		Ogre::Vector2 range;
		std::string name;
	}GBaseLightSettings;

	typedef struct GBaseWorldObjectSettingsStruct
	{
		int type;
		std::string material;
		Ogre::Vector3 position;
		Ogre::Vector3 size;
		Ogre::Vector3 seg;
		std::string name;
		int renderGroup;
	}GBaseWorldObjectSettings;

	typedef struct GBaseLevelSettingsStruct
	{
		std::string skyBoxName;
		double skyBoxDist;
		Ogre::Vector2 worldSize;
		Ogre::Vector2 worldCells;
		Ogre::Vector2 playerSpawn;
		double playerSpawnDirection; // initial angle in radians for player heading
		std::string terrainCfg;
		std::string levelFolder;
		Ogre::Vector2 dropZoneFrom;
		Ogre::Vector2 dropZoneTo;
		Ogre::Vector2 safeZoneFrom;
		Ogre::Vector2 safeZoneTo;
		Ogre::Vector2 damageZoneFrom;
		Ogre::Vector2 damageZoneTo;
		double damageZoneAltitude;
		double damageZoneDamage;

		int fogType;
		Ogre::Vector3 fogColor;
		double fogExpDensity;
		double fogLinStart;
		double fogLinStop;

		int maxBonusesOnScreen;
		int maxBonusLifetime;
		int bonusSpawnOdds;
		int bonusSpawnOddsMax;


		int numLights; //redundant; TODO:remove
		Ogre::Vector3 ambientLight;
		Ogre::Vector3 shadowColor;
		double shadowAlpha;
		std::vector<GBaseLightSettings> lights;
		std::vector<GBaseWorldObjectSettings> wobs;
	}GBaseLevelSettings;


	typedef struct GBaseMusicSettingsStruct
	{
		std::vector<std::string> playlist;
	}GBaseMusicSettings;

	/// Class to handle the extended settings
	class GXAppSettingHolder : public GBaseAppSettingHolder
	{
	protected:
	public:
		GXAppSettingHolder();
		virtual ~GXAppSettingHolder();

		virtual void load( CSimpleIniA *si );
		virtual void save( CSimpleIniA *si );

		GBaseSoundSettings sound;
		GBaseResourceSettings res;
		GBaseVideoSettings video;
		GBaseVideoSettings videoTemp;
		GBaseLogSettings log;
		GBasePerfSettings perf;
		GBaseCameraSettings cam;
		GBaseRenderGroupSettings renderg;
	};

	typedef struct GLevelDefinitionStruct
	{
		std::string id;
		std::string folder;
		std::string fileOfLevel;
		std::string hi; // hiscrore id;
	}GLevelDefinition;

	class GLevelConfigHolder : public GBaseAppSettingHolder
	{
	protected:
		bool singleDefinitionsLoaded;
	public:
		GLevelConfigHolder();
		virtual ~GLevelConfigHolder();
		virtual void load( CSimpleIniA *si );
		virtual void loadSingleDefs( CSimpleIniA *si );
		virtual void clear();

		// contains the current level.
		GBaseLevelSettings lvl;

		// contains all levels (read only once).

		std::vector<GLevelDefinition> defs;
		
	};


	class GMusicConfigHolder :  public GBaseAppSettingHolder
	{
	protected:
	public:
		GMusicConfigHolder();
		virtual ~GMusicConfigHolder();
		virtual void load( CSimpleIniA *si );
		virtual void clear();

		GBaseMusicSettings menu;
		GBaseMusicSettings game;
		std::string copyrightNotice;
	};


	class GModelConfigHolder : public GBaseAppSettingHolder
	{
	protected:
	public:
		GModelConfigHolder();
		virtual ~GModelConfigHolder();
		virtual void load( CSimpleIniA *si );
		virtual void clear();

		VisibleEntityConfig srphead;
		VisibleEntityConfig srpbody;
		std::vector<BonusConfig> bonus;

		double healthIconDivision;
		double healthIconOffset;
	};


	/// Class that will hold all of the application's settings.
	/// It will be a singleton.
	class GBaseAppSettingsUtil
	{
	protected:
		CSimpleIniA *simpleIni;

		std::string loadedFile;

#ifdef G_USE_MULTITHREAD
		guslib::GMutex writeMutex;
#endif

	public:
		GXAppSettingHolder setting;
		GLevelConfigHolder level;
		GModelConfigHolder model;
		GMusicConfigHolder music;

		GBaseAppSettingsUtil();
		virtual ~GBaseAppSettingsUtil();

		virtual bool isLoaded(){ return loadedFile != ""; }
		virtual void load(std::string fileName);
		virtual bool save(std::string fileName="");

		virtual bool loadLevel(const std::string &fileName);
		virtual void unloadLevel();

		virtual bool loadModels(const std::string &fileName);
		virtual void unloadModels();

		virtual bool loadMusic( const std::string &fileName);
		virtual void unloadMusic();

		virtual bool loadLevelList( const std::string &fileName );


		static std::vector<std::string> getRegulatedList( const std::vector<std::string> &inputList );
	};

	typedef guslib::Singleton <GBase::GBaseAppSettingsUtil> GBaseAppSettings;


}