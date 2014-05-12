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
//    Implementation of the base bonus.


#include "VisualBonus.h"
#include <guslib/trace/trace.h>
#include "GResourceManager.h"

using namespace GBase;
using namespace GUtils;

namespace Serpents
{
	VisualBonusPrototype::VisualBonusPrototype( Ogre::SceneManager * sceneManager,
		std::string nameToUse)
		:BonusPrototypeBase( nameToUse ),
		GOgreRenderable( sceneManager )
	{
	}

	VisualBonusPrototype::~VisualBonusPrototype()
	{
		GTRACE(4, "VisualBonusPrototype destructor");
	}


	//
	//
	//

	VisualBonus::VisualBonus( Ogre::SceneManager * sceneManager, const BonusPrototypeBase * prototype, long time )
		:BonusBase( (VisualBonusPrototype*)prototype, time ),
		GOgreRenderable( sceneManager )
	{
	}

	VisualBonus::~VisualBonus()
	{
		GTRACE(4, "_VisualBonus destructor entry.");
		GBase::GResourceManager::getPtr()->cancelLoadRequest( this );
	}


	void VisualBonus::setLocation( DFLOAT x, DFLOAT y )
	{
		GTRACE(5, "VisualBonus_settingloc");
		BonusBase::setLocation(x,y);
		translationVector.x = x;
		translationVector.z = y;
	}


	//
	//
	//

	VisualBonusManager::VisualBonusManager( Ogre::SceneManager * aSceneManager, GBaseEngine *someEngine )
		: BonusManagerBase()
		, sceneManager( aSceneManager )
		, engine( someEngine )
	{
	}


	VisualBonusManager::~VisualBonusManager()
	{
		GTRACE(4, "visual bonus manager destructor");
	}


	BonusBase * VisualBonusManager::createBonusFromPrototype( const BonusPrototypeBase * prototype )
	{
		GTRACE(4, "VisualBonusManager::createBonusFromPrototype");
		if( prototype == NULL )
			return NULL;

		if( !engine )
		{
			GTRACE(1, "ERROR! NULL engine for bonus manager (createBonusFromPrototype)!");
			return NULL;
		}
	
		GTRACE(5, "creating new bonus...");

		GTRACE(4, "VisualBonusManager proto="<<prototype->getType());
		VisualBonus * vibonus = new VisualBonus( this->sceneManager, prototype );
		VisualBonusPrototype * viProto = (VisualBonusPrototype *) vibonus->getLinkToPrototype();
		vibonus->requestLoading( viProto->getMeshName(), viProto->getMaterialName(), viProto->getStencilMaterialName(),
			viProto->getRenderGroup(), viProto->getStencilGroup());
		vibonus->setAnimRotationVector( viProto->getAnimRotationVector() );
		vibonus->setMasterTranslationVector( viProto->getMasterTranslationVector() );
		vibonus->setRotationVector( viProto->getRotationVector() );
		vibonus->setScalingVector( viProto->getScalingVector() );
		vibonus->setTranslationVector( viProto->getTranslationVector() );

		GTRACE(4, "VisualBonusManager returning created bonus.");
		return vibonus;
	}


	BonusPrototypeBase * VisualBonusManager::createPrototype( const std::string &prototypeName )
	{
		return new VisualBonusPrototype( this->sceneManager, prototypeName );
	}


	void VisualBonusManager::onTimeGoesBy( double timeUnits )
	{
		BonusManagerBase::onTimeGoesBy( timeUnits );
	}

}