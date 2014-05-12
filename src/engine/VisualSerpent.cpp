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
//    Implementation of the base serpent.


#include "VisualSerpent.h"
#include "GBAseSetting.h"
#include "GResourceManager.h"
#include <guslib/trace/trace.h>

using namespace GBase;
using namespace Ogre;
using namespace GUtils;

namespace Serpents
{

	VisualSerpent::VisualSerpent( SceneManager * aSceneManager, GBaseEngine * someEngine )
		: SerpentBase()
		, sceneManager( aSceneManager )
		, engine( someEngine )

	{
		//initialize(5);
	}

	VisualSerpent::~VisualSerpent()
	{
		GTRACE(4, "_visualserpent destructor_");
	}


	BodyPartBase* VisualSerpent::createBodyPart(int partType)
	{
		VisualBodyPart* ret = NULL;
		try
		{
			if( partType == 0 )
				ret = createBodyBodyPart();
			else
				ret = createHeadBodyPart();
		}
		catch( Exception &e )
		{
			GTRACE(1, "createBonusFromPrototype Exception caught:"<<e.getFullDescription());
		}
		return ret;
	}

	VisualBodyPart * VisualSerpent::createBodyBodyPart()
	{
		GTRACE(4, "creating serpent bodypart...");

		VisualBodyPart * myBodyPart = new Serpents::VisualBodyPart( sceneManager );
		myBodyPart->requestLoading( srpbody.nameOfMesh, srpbody.nameOfMaterial, srpbody.stencilMaterial,
			srpbody.renderGroup, srpbody.stencilGroup);
		myBodyPart->setRotationVector( srpbody.rotationVector );
		myBodyPart->setScalingVector( srpbody.scalingVector );
		myBodyPart->setTranslationVector( srpbody.translationVector );
		myBodyPart->setAnimRotationVector( srpbody.animRotationVector );

		return myBodyPart;
	}

	VisualBodyPart * VisualSerpent::createHeadBodyPart()
	{
		GTRACE(4, "creating serpent head...");

		VisualBodyPart * myBodyPart = new Serpents::VisualBodyPart( sceneManager );
		myBodyPart->requestLoading( srphead.nameOfMesh, srphead.nameOfMaterial, srphead.stencilMaterial,
			srphead.renderGroup, srphead.stencilGroup);
		myBodyPart->setRotationVector( srphead.rotationVector );
		myBodyPart->setScalingVector( srphead.scalingVector );
		myBodyPart->setTranslationVector( srphead.translationVector );
		myBodyPart->setAnimRotationVector( srphead.animRotationVector );

		return myBodyPart;
	}

	void VisualSerpent::setHeadConfig( VisibleEntityConfig cfg )
	{
		srphead = cfg;
	}

	void VisualSerpent::setBodyConfig( VisibleEntityConfig cfg )
	{
		srpbody = cfg;
	}

}