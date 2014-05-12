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


#include "SharedStrings.h"

#include "GBaseSetting.h"
#include "GBaseEngineDefinitions.h"

#ifdef G_USE_BOOST_PATH
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#endif // G_USE_BOOST_PATH
#include <iostream>               // for std::cout

#include <guslib/trace/trace.h>

namespace GBase
{
	GBaseAppSettingHolder::GBaseAppSettingHolder()
	{
		// Set the defaults
	}

	GBaseAppSettingHolder::~GBaseAppSettingHolder()
	{
	}

	double GBaseAppSettingHolder::getFloatValue(CSimpleIniA * si, std::string category, std::string key, std::string defaultValue)
	{
		char * result;
		result = (char*) si->GetValue(category.c_str(), key.c_str(), defaultValue.c_str());
		double ret = 0;
		if( result == NULL || std::string("")==std::string(result) )
		{
			ret = 0;
		}
		else
		{
			ret = atof(result);
			//if( result != NULL ) delete result;
		}
		return ret;
	}

	std::string GBaseAppSettingHolder::getStringValue(CSimpleIniA * si, std::string category, std::string key, std::string defaultValue)
	{
		char * result;
		result = (char*) si->GetValue(category.c_str(), key.c_str(), defaultValue.c_str());
		std::string ret = result;
		//delete result;
		return ret;
	}

	int GBaseAppSettingHolder::getIntValue(CSimpleIniA * si, std::string category, std::string key, std::string defaultValue)
	{
		char * result;
		result = (char*) si->GetValue(category.c_str(), key.c_str(), defaultValue.c_str());
		int ret = atoi(result);
		//delete result;
		return ret;
	}

	bool GBaseAppSettingHolder::getBoolValue(CSimpleIniA * si, std::string category, std::string key, std::string defaultValue)
	{
		char * result;
		result = (char*) si->GetValue(category.c_str(), key.c_str(), defaultValue.c_str());
		int ret = atoi(result);
		//delete result;
		return (ret!=0);
	}


	StringList GBaseAppSettingHolder::getIndexedList(CSimpleIniA * si, std::string category, std::string key, int nFrom)
	{
		StringList ret;
		ret.clear();
		bool bCanStillLoad = true;
		for( int i=nFrom; (i<=nFrom+G_MAX_OPTION_LIST_SUBITEMS_COUNT) && bCanStillLoad; i++ )
		{
			std::stringstream ssKey;
			ssKey.clear();ssKey.str("");
			ssKey<< key <<"." << i;
			char * result = (char*) si->GetValue(category.c_str(), ssKey.str().c_str(), "");
			if( result == NULL || std::string("")==std::string(result) ) 
			{
				bCanStillLoad = false;
			}
			else
			{
				ret.push_back(std::string(result));
			}
			//delete result;
		}
		return ret;
	}


	Ogre::Vector3 GBaseAppSettingHolder::getVector3(CSimpleIniA * si, std::string category, std::string key, Ogre::Vector3 defaultValue)
	{
		Ogre::Vector3 ret = defaultValue;
		std::stringstream ssKey;
		char * result;

		ssKey.clear();ssKey.str("");
		ssKey<< key <<".x";
		result = (char*) si->GetValue(category.c_str(), ssKey.str().c_str(), "");
		if( result == NULL || std::string("")==std::string(result) ) 
		{
			ret.x = defaultValue.x;
		}
		else
		{
			ret.x = (Ogre::Real) atof(result);
			//if( result != NULL) delete result;
		}

		ssKey.clear();ssKey.str("");
		ssKey<< key <<".y";
		result = (char*) si->GetValue(category.c_str(), ssKey.str().c_str(), "");
		if( result == NULL || std::string("")==std::string(result) ) 
		{
			ret.y = defaultValue.y;
		}
		else
		{
			ret.y = (Ogre::Real) atof(result);
			//if( result != NULL) delete result;
		}

		ssKey.clear();ssKey.str("");
		ssKey<< key <<".z";
		result = (char*) si->GetValue(category.c_str(), ssKey.str().c_str(), "");
		if( result == NULL || std::string("")==std::string(result) ) 
		{
			ret.z = defaultValue.z;
		}
		else
		{
			ret.z = (Ogre::Real) atof(result);
			//if( result != NULL) delete result;
		}

		return ret;
	}


	Ogre::Vector2 GBaseAppSettingHolder::getVector2(CSimpleIniA * si, std::string category, std::string key, Ogre::Vector2 defaultValue)
	{
		Ogre::Vector2 ret = defaultValue;
		std::stringstream ssKey;
		char * result;

		ssKey.clear();ssKey.str("");
		ssKey<< key <<".x";
		result = (char*) si->GetValue(category.c_str(), ssKey.str().c_str(), "");
		if( result == NULL || std::string("")==std::string(result) ) 
		{
			ret.x = defaultValue.x;
		}
		else
		{
			ret.x = (Ogre::Real) atof(result);
			//if( result != NULL) delete result;
		}

		ssKey.clear();ssKey.str("");
		ssKey<< key <<".y";
		result = (char*) si->GetValue(category.c_str(), ssKey.str().c_str(), "");
		if( result == NULL || std::string("")==std::string(result) ) 
		{
			ret.y = defaultValue.y;
		}
		else
		{
			ret.y = (Ogre::Real) atof(result);
			//if( result != NULL) delete result;
		}
		return ret;
	}

	void GBaseAppSettingHolder::setFloatValue(CSimpleIniA * si, std::string category, std::string key, double value)
	{
		std::stringstream ss;
		ss<<value;
		si->SetValue( category.c_str(), key.c_str(), ss.str().c_str() );
	}

	void GBaseAppSettingHolder::setStringValue(CSimpleIniA * si, std::string category, std::string key, std::string value)
	{
		std::stringstream ss;
		ss<<value;
		si->SetValue( category.c_str(), key.c_str(), ss.str().c_str() );
	}

	void GBaseAppSettingHolder::setIntValue(CSimpleIniA * si, std::string category, std::string key, int value)
	{
		std::stringstream ss;
		ss<<value;
		si->SetValue( category.c_str(), key.c_str(), ss.str().c_str() );
	}

	void GBaseAppSettingHolder::setBoolValue(CSimpleIniA * si, std::string category, std::string key, bool value)
	{
		std::stringstream ss;
		ss<<value;
		si->SetValue( category.c_str(), key.c_str(), ss.str().c_str() );
	}

	void GBaseAppSettingHolder::setIndexedList(CSimpleIniA * si, std::string category, std::string key, int nFrom, StringList value)
	{
		for( int i=0; i<=(int)value.size(); i++ )
		{
			std::stringstream ssKey;
			ssKey.clear();ssKey.str("");
			ssKey<< key <<"." << (i+nFrom);
			si->SetValue( category.c_str(), ssKey.str().c_str(), value[i].c_str() );
		}
	}

	void GBaseAppSettingHolder::setCoords3(CSimpleIniA * si, std::string category, std::string key, Ogre::Vector3 value)
	{
		std::stringstream ssKey;
		std::stringstream ssVal;
		ssKey.clear();ssKey.str("");
		ssVal.clear();ssVal.str("");
		ssKey<< key <<".x";
		ssVal<< value.x;
		si->SetValue( category.c_str(), ssKey.str().c_str(), ssVal.str().c_str() );

		ssKey.clear();ssKey.str("");
		ssVal.clear();ssVal.str("");
		ssKey<< key <<".y";
		ssVal<< value.y;
		si->SetValue( category.c_str(), ssKey.str().c_str(), ssVal.str().c_str() );

		ssKey.clear();ssKey.str("");
		ssVal.clear();ssVal.str("");
		ssKey<< key <<".z";
		ssVal<< value.z;
		si->SetValue( category.c_str(), ssKey.str().c_str(), ssVal.str().c_str() );
	}


	void GBaseAppSettingHolder::setCoords2(CSimpleIniA * si, std::string category, std::string key, Ogre::Vector2 value)
	{
		std::stringstream ssKey;
		std::stringstream ssVal;
		ssKey.clear();ssKey.str("");
		ssVal.clear();ssVal.str("");
		ssKey<< key <<".x";
		ssVal<< value.x;
		si->SetValue( category.c_str(), ssKey.str().c_str(), ssVal.str().c_str() );

		ssKey.clear();ssKey.str("");
		ssVal.clear();ssVal.str("");
		ssKey<< key <<".y";
		ssVal<< value.y;
		si->SetValue( category.c_str(), ssKey.str().c_str(), ssVal.str().c_str() );
	}


	void GBaseAppSettingHolder::load( CSimpleIniA *si )
	{
		if( !si )
			return;
	}



	void GBaseAppSettingHolder::save( CSimpleIniA *si )
	{
		if( !si )
			return;
	}


	//GBaseAppSettings * GBaseAppSettings::staticInstance = NULL;
//#ifdef G_USE_MULTITHREAD
//	guslib::GMutex GBaseAppSettings::creationMutex;
//#endif


	///// Getter for the instance pointer.
	//GBaseAppSettings * GBaseAppSettings::getPtr ()
	//{
	//	// first check if it was created. If the mutex would be placed first, a performance hit would occur, since
	//	// all operations would create and release a lock.
	//	if( ! staticInstance )
	//	{
	//		// try to lock
	//	#ifdef G_USE_MULTITHREAD
	//		guslib::GScopedLock myLock( creationMutex );
	//	#endif

	//		// While the lock tried to take control, another thread may have already finished instantiating the object,
	//		// so a new check should be done to ensure safety (double-checked locking pattern).
	//		if( ! staticInstance )
	//		{
	//			// Should create the object as volatile to prevent some compiler optimisations that may lead to the lines being
	//			// executed internally in a slightly different order than the one given here.
	//			// (see also: http://aszt.inf.elte.hu/~gsd/klagenfurt/material/ch03s05.html)
	//			volatile GBaseAppSettings * volatileptr = new GBaseAppSettings();
	//			staticInstance = (GBaseAppSettings*)volatileptr;
	//		}
	//	}
	//	return staticInstance;
	//}

	GBaseAppSettingsUtil::GBaseAppSettingsUtil()
	{
		simpleIni = NULL;
		loadedFile = "";
		load(G_BASE_APP_INI_FILE);
		loadModels(G_BASE_APP_MODELS_INI_FILE);
		loadLevelList(GUS_LEVEL_INI_FILE);
		loadMusic(GUS_MUSIC_INI_FILE);
	}

	GBaseAppSettingsUtil::~GBaseAppSettingsUtil()
	{
		if( simpleIni != NULL )
		{
			delete simpleIni;
		}
	}


	void GBaseAppSettingsUtil::load(std::string fileName)
	{
		if( simpleIni != NULL )
		{
			delete simpleIni;
		}

		simpleIni = new CSimpleIniA();
		simpleIni->Reset();
		simpleIni->LoadFile( fileName.c_str() );
		loadedFile = fileName;

		setting.load(simpleIni);
	}


	bool GBaseAppSettingsUtil::save(std::string fileName)
	{
		if( fileName == "" )
		{
			fileName = loadedFile;
		}
		setting.save(simpleIni);
		SI_Error err = simpleIni->SaveFile( fileName.c_str() );
		if( err < 0 )
		{
			return false;
		}
		return true;
	}

	bool GBaseAppSettingsUtil::loadLevel( const std::string &fileName )
	{
		CSimpleIniA * myAuxIni = new CSimpleIniA();
		myAuxIni->Reset();
		
		SI_Error sie = myAuxIni->LoadFile( fileName.c_str() );
		if(sie<0)
		{
			return false;
		}
		level.load(myAuxIni);
		delete myAuxIni;
		return true;
	}

	
	void GBaseAppSettingsUtil::unloadLevel()
	{
		level.clear();
	}


	bool GBaseAppSettingsUtil::loadModels( const std::string &fileName )
	{
		CSimpleIniA * myAuxIni = new CSimpleIniA();
		myAuxIni->Reset();
		
		SI_Error sie = myAuxIni->LoadFile( fileName.c_str() );
		if(sie<0)
		{
			return false;
		}
		model.load(myAuxIni);
		delete myAuxIni;
		return true;
	}


	void GBaseAppSettingsUtil::unloadModels()
	{
		model.clear();
	}


	bool GBaseAppSettingsUtil::loadLevelList( const std::string &fileName )
	{
		CSimpleIniA * myAuxIni = new CSimpleIniA();
		myAuxIni->Reset();
		
		SI_Error sie = myAuxIni->LoadFile( fileName.c_str() );
		if(sie<0)
		{
			return false;
		}
		level.loadSingleDefs(myAuxIni);
		delete myAuxIni;
		return true;
	}


	bool GBaseAppSettingsUtil::loadMusic( const std::string &fileName )
	{
		CSimpleIniA * myAuxIni = new CSimpleIniA();
		myAuxIni->Reset();
		
		SI_Error sie = myAuxIni->LoadFile( fileName.c_str() );
		if(sie<0)
		{
			return false;
		}
		music.load(myAuxIni);
		delete myAuxIni;
		return true;
	}

	void GBaseAppSettingsUtil::unloadMusic()
	{
		music.clear();
	}


	GXAppSettingHolder::GXAppSettingHolder()
	{
		// Set the defaults
	}

	GXAppSettingHolder::~GXAppSettingHolder()
	{
	}

	void GXAppSettingHolder::load( CSimpleIniA *si )
	{
		if( !si )
			return;
		GBaseAppSettingHolder::load( si );

		// Tracing & Logging options

		log.traceFile = getStringValue(si, "general", "traceFile", "");
		log.traceLevel = getIntValue(si, "general", "traceLevel", "1");
		log.ogreLog = getStringValue(si, "general", "ogreLog", "");

		// Video options

		video.renderEngine = getStringValue(si, "graphics", "engine", Direct3D);
		video.windowHeight = getIntValue(si, "graphics", "window.height", "600");
		video.windowWidth = getIntValue(si, "graphics", "window.width", "800");
		video.vsync = getBoolValue(si, "graphics", "vsync", "0");
		video.fullscreen = getBoolValue(si, "graphics", "fullscreen", "0");
		video.fsaa = getIntValue(si, "graphics", "fsaa", "0");
		
		// additional video setting.

		if( video.renderEngine == Direct3D )
		{
			video.renderEngineFullName = G_RENDER_ENGINE_D3D_LONG_NAME;
		}
		else if( video.renderEngine == OpenGL )
		{
			video.renderEngineFullName = G_RENDER_ENGINE_OGL_LONG_NAME;
		}

		// Resource options

		res.pluginDir = getStringValue(si, "plugins", "pluginDir", "");
		res.plugins = getIndexedList(si, "plugins", "plugin", 1);
		res.preload = getIndexedList(si, "resources", "preload", 1);
		res.resources = getIndexedList(si, "resources", "folder", 1);

		// Sound options

		sound.masterVolume = getFloatValue(si, "sound", "masterVolume", "0");
		sound.fxVolume = getFloatValue(si, "sound", "fxVolume", "0");
		sound.musicVolume = getFloatValue(si, "sound", "musicVolume", "0");
		sound.voiceVolume = getFloatValue(si, "sound", "voiceVolume", "0");

		// Performance options

		perf.delayBetweenRenders = getIntValue(si, "general", "delayBetweenRenders", "10");
		perf.useShadows = getBoolValue(si, "general", "useShadows", "0");
		perf.shadowType = getIntValue(si, "general", "shadowType", "1");
		perf.showFPS = getBoolValue(si, "general", "showFPS", "0");
		perf.meshQuality = getIntValue(si, "general", "meshQual", "0");
		perf.cpuSleep = getIntValue(si, "general", "cpuSleep", "10");
		perf.postProcessing = getBoolValue(si, "general", "postProcessing", "0");
		perf.compositorList = getIndexedList(si, "general", "compositors", 1);

		// Camera options

		cam.cam1Pos = getVector3(si, "camera", "camera1Pos", Ogre::Vector3::ZERO);
		cam.cam1LookAt = getVector3(si, "camera", "camera1LookAt", Ogre::Vector3::ZERO);
		cam.cam2Pos = getVector3(si, "camera", "camera2Pos", Ogre::Vector3::ZERO);
		cam.cam2LookAt = getVector3(si, "camera", "camera2LookAt", Ogre::Vector3::ZERO);

		cam.zoomLevel = getFloatValue(si, "camera", "zoomLevel", "0");
		cam.zoomStep = getFloatValue(si, "camera", "zoomStep", "0.2");
		cam.zoomMax = getFloatValue(si, "camera", "zoomMax", "0.8");
		cam.defaultMode = getIntValue(si, "camera", "cameraMode", "1");
		cam.fov = getFloatValue(si, "camera", "fov", "50");

		// Stencil options (render group)

		renderg.firstID = (Ogre::uint8)getIntValue(si, "renderGroup", "firstStencilOpRenderQueue", "0");
		renderg.lastID = getIntValue(si, "renderGroup", "lastStencilOpRenderQueue", "0");

		bool bCanContinue = true;
		for( int i=0; i<100 && bCanContinue; i++ )
		{
			std::stringstream ss;
			ss<<"renderGroup."<<i;
			std::string s = ss.str();
			if( si->GetSection(s.c_str()) != NULL )
			{
				GBaseRenderGroupEntitySettings grp;
				int temp;
				grp.id = getIntValue(si, s, "id", "0");
				temp = getIntValue(si, s, "compareFunc", "0");
				switch( temp )
				{
				case 1:
					grp.compareFunc = Ogre::CMPF_ALWAYS_PASS;
					break;
				case 2:
					grp.compareFunc = Ogre::CMPF_LESS;
					break;
				case 3:
					grp.compareFunc = Ogre::CMPF_LESS_EQUAL;
					break;
				case 4:
					grp.compareFunc = Ogre::CMPF_EQUAL;
					break;
				case 5:
					grp.compareFunc = Ogre::CMPF_NOT_EQUAL;
					break;
				case 6:
					grp.compareFunc = Ogre::CMPF_GREATER_EQUAL;
					break;
				case 7:
					grp.compareFunc = Ogre::CMPF_GREATER;
					break;
				case 0:
				default:
					grp.compareFunc = Ogre::CMPF_ALWAYS_FAIL;
					break;
				}
				temp = getIntValue(si, s, "stencilFailOp", "0");
				switch(temp)
				{
				case 1:
					grp.stencilFailOp = Ogre::SOP_ZERO;
					break;
				case 2:
					grp.stencilFailOp = Ogre::SOP_REPLACE;
					break;
				case 3:
					grp.stencilFailOp = Ogre::SOP_INCREMENT;
					break;
				case 4:
					grp.stencilFailOp = Ogre::SOP_DECREMENT;
					break;
				case 5:
					grp.stencilFailOp = Ogre::SOP_INCREMENT_WRAP;
					break;
				case 6:
					grp.stencilFailOp = Ogre::SOP_DECREMENT_WRAP;
					break;
				case 7:
					grp.stencilFailOp = Ogre::SOP_INVERT;
					break;
				case 0:
				default:
					grp.stencilFailOp = Ogre::SOP_KEEP;
					break;
				}
				temp = getIntValue(si, s, "depthFailOp", "0");
				switch(temp)
				{
				case 1:
					grp.depthFailOp = Ogre::SOP_ZERO;
					break;
				case 2:
					grp.depthFailOp = Ogre::SOP_REPLACE;
					break;
				case 3:
					grp.depthFailOp = Ogre::SOP_INCREMENT;
					break;
				case 4:
					grp.depthFailOp = Ogre::SOP_DECREMENT;
					break;
				case 5:
					grp.depthFailOp = Ogre::SOP_INCREMENT_WRAP;
					break;
				case 6:
					grp.depthFailOp = Ogre::SOP_DECREMENT_WRAP;
					break;
				case 7:
					grp.depthFailOp = Ogre::SOP_INVERT;
					break;
				case 0:
				default:
					grp.depthFailOp = Ogre::SOP_KEEP;
					break;
				}
				temp = getIntValue(si, s, "passOp", "0");
				switch(temp)
				{
				case 1:
					grp.passOp = Ogre::SOP_ZERO;
					break;
				case 2:
					grp.passOp = Ogre::SOP_REPLACE;
					break;
				case 3:
					grp.passOp = Ogre::SOP_INCREMENT;
					break;
				case 4:
					grp.passOp = Ogre::SOP_DECREMENT;
					break;
				case 5:
					grp.passOp = Ogre::SOP_INCREMENT_WRAP;
					break;
				case 6:
					grp.passOp = Ogre::SOP_DECREMENT_WRAP;
					break;
				case 7:
					grp.passOp = Ogre::SOP_INVERT;
					break;
				case 0:
				default:
					grp.passOp = Ogre::SOP_KEEP;
					break;
				}
				temp = getIntValue(si, s, "value", "1");
				grp.value = temp;
				if( grp.id != 0 )
				{
					renderg.groups.push_back(grp);
				}
				//defs.push_back( GLevelDefinition() );
				//++ numLvls;
			}
		}

	}

	void GXAppSettingHolder::save( CSimpleIniA *si )
	{
		if( !si )
			return;
		GBaseAppSettingHolder::save( si );

		// Tracing & Logging options - not saveable

		// Video options

		setStringValue(si, "graphics", "engine", video.renderEngine);
		setIntValue(si, "graphics", "window.height", video.windowHeight);
		setIntValue(si, "graphics", "window.width", video.windowWidth);
		setBoolValue(si, "graphics", "vsync", video.vsync);
		setBoolValue(si, "graphics", "fullscreen", video.fullscreen);
		setIntValue(si, "graphics", "fsaa", video.fsaa);

		// Resource options - not saveable

		// Sound options

		setFloatValue(si, "sound", "masterVolume", sound.masterVolume);
		setFloatValue(si, "sound", "fxVolume", sound.fxVolume);
		setFloatValue(si, "sound", "musicVolume", sound.musicVolume);
		setFloatValue(si, "sound", "voiceVolume", sound.voiceVolume);

		// Performance options

		setIntValue(si, "general", "delayBetweenRenders", perf.delayBetweenRenders);
		setBoolValue(si, "general", "useShadows", perf.useShadows);
		setBoolValue(si, "general", "showFPS", perf.showFPS);
		setIntValue(si, "general", "meshQual", perf.meshQuality);
		setIntValue(si, "general", "cpuSleep", perf.cpuSleep);
		setBoolValue(si, "general", "postProcessing", perf.postProcessing);
	}

	// ---------------

	GLevelConfigHolder::GLevelConfigHolder()
		:singleDefinitionsLoaded(false)
	{
	}

	GLevelConfigHolder::~GLevelConfigHolder()
	{
	}


	void GLevelConfigHolder::loadSingleDefs( CSimpleIniA *si )
	{
		if( !si )
			return;
		//defs
		int numLvls = 0;
		bool bCanContinue = true;
		for( int i=0; i<100 && bCanContinue; i++ )
		{
			std::stringstream ss;
			ss<<"level"<<i;
			std::string s = ss.str();
			if( si->GetSection(s.c_str()) != NULL )
			{
				GLevelDefinition str;
				defs.push_back( GLevelDefinition() );
				defs[numLvls].fileOfLevel = getStringValue(si, s, "level", "");
				defs[numLvls].folder = getStringValue(si, s, "folder", "");
				defs[numLvls].id = getStringValue(si, s, "id", "");
				defs[numLvls].hi = getStringValue(si, s, "hi", "");

				++ numLvls;
			}
		}
	}



	void GLevelConfigHolder::load( CSimpleIniA *si )
	{
		if( !si )
			return;
		GBaseAppSettingHolder::load( si );

		// get the general settings

		lvl.skyBoxName = getStringValue(si, "general", "skyBoxName", "");
		lvl.skyBoxDist = getFloatValue(si, "general", "skyBoxDist", "100.0");

		lvl.worldSize = getVector2(si, "general", "worldSize", Ogre::Vector2::UNIT_SCALE);
		lvl.worldCells = getVector2(si, "general", "worldCells", Ogre::Vector2::UNIT_SCALE);

		lvl.playerSpawn = getVector2(si, "general", "worldCells", Ogre::Vector2::UNIT_SCALE);
		lvl.playerSpawnDirection = getFloatValue(si, "general", "playerSpawnDirection", "0");

		lvl.terrainCfg = getStringValue(si, "general", "terrain", "");
		lvl.levelFolder = getStringValue(si, "general", "levelFolder", "");

		lvl.dropZoneFrom = getVector2(si, "general", "dropZoneFrom", Ogre::Vector2::UNIT_SCALE);
		lvl.dropZoneTo = getVector2(si, "general", "dropZoneTo", Ogre::Vector2::UNIT_SCALE);

		lvl.safeZoneFrom = getVector2(si, "general", "safeZoneFrom", Ogre::Vector2::UNIT_SCALE);
		lvl.safeZoneTo = getVector2(si, "general", "safeZoneTo", Ogre::Vector2::UNIT_SCALE);

		lvl.damageZoneFrom= getVector2(si, "general", "damageZoneFrom", Ogre::Vector2::UNIT_SCALE);
		lvl.damageZoneTo = getVector2(si, "general", "damageZoneTo", Ogre::Vector2::UNIT_SCALE);

		lvl.damageZoneAltitude = getFloatValue(si, "general", "damageZoneAltitude", "0");
		lvl.damageZoneDamage = getFloatValue(si, "general", "damageZoneDamage", "0");

		lvl.maxBonusesOnScreen = getIntValue(si, "general", "maxBonusesOnScreen", "2"); // 2 at a time
		lvl.maxBonusLifetime = getIntValue(si, "general", "maxBonusLifetime", "30000"); // 30 seconds
		lvl.bonusSpawnOdds = getIntValue(si, "general", "bonusSpawnOdds", "1");
		lvl.bonusSpawnOddsMax = getIntValue(si, "general", "bonusSpawnOddsMax", "100");
		// get the map objects

		int numWobs = 0;
		bool bCanContinue = true;
		for( int i=0; i<100 && bCanContinue; i++ )
		{
			std::stringstream ss;
			ss<<"worldObject."<<i;
			std::string s = ss.str();
			if( si->GetSection(s.c_str()) != NULL )
			{
				lvl.wobs.push_back(GBaseWorldObjectSettings());
				lvl.wobs[numWobs].material = getStringValue(si, s, "material", "");
				lvl.wobs[numWobs].type = getIntValue(si, s, "type", "0");
				lvl.wobs[numWobs].position = getVector3(si, s, "position", Ogre::Vector3::ZERO);
				lvl.wobs[numWobs].size = getVector3(si, s, "size", Ogre::Vector3::ZERO);
				lvl.wobs[numWobs].seg = getVector3(si, s, "seg", Ogre::Vector3::ZERO);
				lvl.wobs[numWobs].renderGroup = getIntValue(si, s, "renderGroup", "0");
				lvl.wobs[numWobs].name = s;
				numWobs++;
			}
		}

		// get the fog options

		lvl.fogType = getIntValue(si, "fog", "type", "0");
		lvl.fogColor = getVector3(si, "fog", "color", Ogre::Vector3::UNIT_SCALE);
		lvl.fogExpDensity = getFloatValue(si, "fog", "expDensity", "0");
		lvl.fogLinStart = getFloatValue(si, "fog", "linearStart", "0");
		lvl.fogLinStop = getFloatValue(si, "fog", "linearEnd", "0");

		// get the light options

		lvl.ambientLight = getVector3(si, "lighting", "ambientColor", Ogre::Vector3::UNIT_SCALE);
		lvl.shadowColor = getVector3(si, "lighting", "shadowColor", Ogre::Vector3::UNIT_SCALE);
		lvl.shadowAlpha = getFloatValue(si, "lighting", "shadowAlpha", "1");

		// get all lights

		lvl.numLights = 0;
		bCanContinue = true;
		for( int i=0; i<100 && bCanContinue; i++ )
		{
			std::stringstream ss;
			ss<<"light."<<i;
			std::string s = ss.str();
			if( si->GetSection(s.c_str()) != NULL )
			{
				lvl.lights.push_back(GBaseLightSettings());
				lvl.lights[lvl.numLights].castShadows = getBoolValue(si, s, "castShadows", "1");
				lvl.lights[lvl.numLights].type = getIntValue(si, s, "type", "0");
				lvl.lights[lvl.numLights].diffuseColor = getVector3(si, s, "diffuseColor", Ogre::Vector3::UNIT_SCALE);
				lvl.lights[lvl.numLights].specularColor = getVector3(si, s, "specularColor", Ogre::Vector3::UNIT_SCALE);
				lvl.lights[lvl.numLights].position = getVector3(si, s, "position", Ogre::Vector3::UNIT_SCALE);
				lvl.lights[lvl.numLights].direction = getVector3(si, s, "direction", Ogre::Vector3::ZERO);
				lvl.lights[lvl.numLights].range = getVector2(si, s, "range", Ogre::Vector2::ZERO);
				lvl.lights[lvl.numLights].name = s;
				lvl.numLights ++;
			}
		}
	}

	void GLevelConfigHolder::clear()
	{
		lvl.numLights = 0;
		lvl.wobs.clear();
		lvl.lights.clear();
	}


	// --------------------------- GModelConfigHolder -------------------

	GModelConfigHolder::GModelConfigHolder()
	{
	}

	GModelConfigHolder::~GModelConfigHolder()
	{
	}

	void GModelConfigHolder::load( CSimpleIniA *si )
	{
		if( !si )
			return;
		GBaseAppSettingHolder::load( si );

		// Other stuff

		healthIconDivision = getFloatValue(si, "general", "healthIconDiv", "100");
		healthIconOffset = getFloatValue(si, "general", "healthIconOffset", "0");

		// get the general settings

		srphead.nameOfMesh = getStringValue(si, "head1.model", "mesh", "");
		srphead.nameOfMaterial = getStringValue(si, "head1.model", "material", "");
		srphead.stencilMaterial = getStringValue(si, "head1.model", "stencil", "");
		srphead.renderGroup = getIntValue(si, "head1.model", "renderGroup", "0");
		srphead.stencilGroup = getIntValue(si, "head1.model", "stencilGroup", "0");
		srphead.scalingVector = getVector3(si, "head1.model", "scale", Ogre::Vector3::UNIT_SCALE);
		srphead.rotationVector = getVector3(si, "head1.model", "rotation", Ogre::Vector3::ZERO);
		srphead.translationVector = getVector3(si, "head1.model", "translation", Ogre::Vector3::ZERO);

		srpbody.nameOfMesh = getStringValue(si, "bodypart1.model", "mesh", "");
		srpbody.nameOfMaterial = getStringValue(si, "bodypart1.model", "material", "");
		srpbody.stencilMaterial = getStringValue(si, "bodypart1.model", "stencil", "");
		srpbody.renderGroup = getIntValue(si, "bodypart1.model", "renderGroup", "0");
		srpbody.stencilGroup = getIntValue(si, "bodypart1.model", "stencilGroup", "0");
		srpbody.scalingVector = getVector3(si, "bodypart1.model", "scale", Ogre::Vector3::UNIT_SCALE);
		srpbody.rotationVector = getVector3(si, "bodypart1.model", "rotation", Ogre::Vector3::ZERO);
		srpbody.translationVector = getVector3(si, "bodypart1.model", "translation", Ogre::Vector3::ZERO);

		bool bCanContinue = true;
		int numBonuses = 0;
		for( int i=0; i<100 && bCanContinue; i++ )
		{
			std::stringstream ss;
			ss<<"bonus"<<i;
			std::string s = ss.str();
			if( si->GetSection(s.c_str()) != NULL )
			{
				bonus.push_back(BonusConfig());
				bonus[numBonuses].bonusType = getStringValue(si, s, "type", "");
				bonus[numBonuses].odds = getIntValue(si, s, "odds", "1");
				bonus[numBonuses].numProps = getIntValue(si, s, "numprops", "0");
				for( int j=0; j<bonus[numBonuses].numProps; j++ )
				{
					bonus[numBonuses].properties.push_back( PropertyConfig() );
					std::stringstream myss;
					myss<<"prop"<<j<<".type";
					std::string mys = myss.str();
					bonus[numBonuses].properties[j].type = getStringValue(si, s, mys, "");
					myss.str("");
					myss<<"prop"<<j<<".value";
					mys = myss.str();
					bonus[numBonuses].properties[j].value = getFloatValue(si, s, mys, "");
				}

				bonus[numBonuses].nameOfMesh = getStringValue(si, s, "mesh", "");
				bonus[numBonuses].nameOfMaterial = getStringValue(si, s, "material", "");
				bonus[numBonuses].stencilMaterial = getStringValue(si, s, "stencil", "");
				bonus[numBonuses].renderGroup = getIntValue(si, s, "renderGroup", "0");
				bonus[numBonuses].stencilGroup = getIntValue(si, s, "stencilGroup", "0");
				bonus[numBonuses].scalingVector = getVector3(si, s, "scale", Ogre::Vector3::UNIT_SCALE);
				bonus[numBonuses].rotationVector = getVector3(si, s, "rotation", Ogre::Vector3::UNIT_SCALE);
				bonus[numBonuses].translationVector = getVector3(si, s, "translation", Ogre::Vector3::UNIT_SCALE);

				numBonuses++;
			}
		}
	}

	void GModelConfigHolder::clear()
	{
	}


	// --------------------------- GMusicConfigHolder -------------------

	GMusicConfigHolder::GMusicConfigHolder()
	{
	}

	GMusicConfigHolder::~GMusicConfigHolder()
	{
	}


	std::vector<std::string> GBaseAppSettingsUtil::getRegulatedList( const std::vector<std::string> &inputList )
	{
		std::vector<std::string> stuffToAdd;
		stuffToAdd.clear();
		if( inputList.empty() )
		{
			return stuffToAdd;
		}
		for( std::vector<std::string>::const_iterator it = inputList.begin();
			 it!= inputList.end(); ++it )
		{
			std::string temp = *it;
			int myPos = (int) temp.find("*");
			if( myPos != std::string::npos )
			{
				// found it;
				std::string path = temp.substr(0, myPos);
				std::string extension = temp.substr(myPos+1);
#ifdef G_USE_BOOST_PATH
				boost::filesystem::path usedPath(path);
				if( boost::filesystem::exists(usedPath) && boost::filesystem::is_directory(usedPath) )
				{
					for( boost::filesystem::directory_iterator it2(usedPath), ender; it2!= ender; ++it2 )
					{
						std::stringstream ss;
						ss<< *it2;
						std::string nameOfFile = ss.str();
						if( extension.length() > 0 )
						{
							if( nameOfFile.find( extension ) != std::string::npos )
								stuffToAdd.push_back( nameOfFile );
						}
						else
						{
							stuffToAdd.push_back( nameOfFile );
						}
					}
				}
#endif//G_USE_BOOST_PATH
			}
			else
			{
				// no wildcard found; add in current form
				stuffToAdd.push_back( std::string( *it ) );
			}
		}

		return stuffToAdd;
	}

	void GMusicConfigHolder::load( CSimpleIniA *si )
	{
		if( !si )
			return;
		GBaseAppSettingHolder::load( si );

		// get the general settings
		menu.playlist = getIndexedList( si, "menu", "track", 0 );

		// get the general settings
		game.playlist = getIndexedList( si, "game", "track", 0 );

		// parse the lists;		any lists containing wildcards
		menu.playlist = GBaseAppSettingsUtil::getRegulatedList( menu.playlist );
		game.playlist = GBaseAppSettingsUtil::getRegulatedList( game.playlist );

		copyrightNotice = getStringValue( si, "copyright", "notice", "" );
	}

	void GMusicConfigHolder::clear()
	{
	}


}