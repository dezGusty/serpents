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
//    Interface for renderable map objects.


#include "GOgreRenderable.h"
#include <guslib/system/timer.h>
#include "GResourceManager.h"
#include <guslib/trace/trace.h>
#include "GStencilRenderer.h"

// mainly for c_distanceForRaySceneQueries
#include "SerpConstants.h"

#ifndef OGRE_THREAD_SUPPORT
#pragma message ("Warning! No background loading support included! (see OGRE_THREAD_SUPPORT in OgreConfig.h)")
#else
#if OGRE_THREAD_SUPPORT !=1 && OGRE_THREAD_SUPPORT !=2
#pragma message ("********")
#pragma message ("Warning! No background loading support included! (see OGRE_THREAD_SUPPORT in OgreConfig.h)")
#pragma message ("********")
#endif
#endif


using namespace Ogre;

namespace Serpents
{
	GOgreRenderable::GOgreRenderable( Ogre::SceneManager * aSceneManager )
		: GMovableRenderable( aSceneManager )
		, entName( "" )
		, stencilEntName( "" )
		, meshName( "" )
		, materialName( "" )
		, stencilMaterialName( "" )
		, removeOgreEntitiesAtUnload( false )
		, loadingWasPerformed( false )
		, raySceneQuery( NULL )
		, clonedObject( false )
		, temporaryObject( false )
		, usesStencil( false )
	{
	}


	GOgreRenderable::~GOgreRenderable()
	{
		GTRACE(6, "_GOgreRenderable destructor entry_"<< (cloned() ? "(cloned)" : "(original)") );
		if( loaded() )
			unload();
		GTRACE(5, "_GOgreRenderable destroyed.");
	}


	GOgreRenderable* GOgreRenderable::clonePtr()
	{
		GOgreRenderable * tempPtr = new GOgreRenderable( sceneManager_ );
		tempPtr->sceneNodeName_ = sceneNodeName_;
		tempPtr->entName = entName;
		tempPtr->materialName = materialName;
		tempPtr->stencilEntName = stencilEntName;
		tempPtr->stencilMaterialName = stencilMaterialName;
		tempPtr->creationIndex = creationIndex;
		tempPtr->loadingWasPerformed = loadingWasPerformed;
		tempPtr->clonedObject = true;
		tempPtr->usesStencil = usesStencil;
		return tempPtr;
	}


	GOgreRenderable* GOgreRenderable::createTemporaryClone()
	{
		GOgreRenderable * tempPtr = clonePtr();
		tempPtr->temporaryObject = true;
		tempPtr->animRotationVector = animRotationVector;
		tempPtr->rotationVector = rotationVector;
		tempPtr->scalingVector = scalingVector;
		tempPtr->animScalingVector = animScalingVector;
		tempPtr->masterTranslationVector = masterTranslationVector;
		tempPtr->translationVector = translationVector;
		tempPtr->raySceneQuery = raySceneQuery;
		return tempPtr;
	}


	void GOgreRenderable::unload()
	{
		GTRACE(5, "GOgreRenderable::unload()");

		if( temporaryObject )
		{
			// temporary objects don't release anything, they are temporary copies of the normal objects.
			return;
		}

		if( sceneManager_ == NULL )
		{
			GTRACE(1, "ERROR!: NULL sceneManager in GOgreRenderable destructor");
			return;
		}

		//	if the ogre data doesn't need to be destroyed, then exit.

		if( ! removeOgreEntitiesAtUnload )
		{
			requestUnloading();
			return;
		}

		GTRACE(5, "Detaching data from :" << getSceneNodeName() <<" and "<< getEntName() );

		//	remove the scene node

		Ogre::SceneNode * tempNode = sceneManager_->getSceneNode(getSceneNodeName() );
		if( tempNode )
		{
			sceneManager_->getRootSceneNode()->removeChild( tempNode );	//	the node is removed from the scene, but not destroyed.
			tempNode->removeAllChildren();	//	all other children of the node are removed(detached) (not destroyed).
			tempNode->detachAllObjects();

			GTRACE(5, "Scene node delete:" << tempNode->getName());
			sceneManager_->destroySceneNode( tempNode->getName() );

			//	remove the entity too

			GTRACE(5, "Entity delete:" << getEntName() );
			sceneManager_->destroyEntity( getEntName() );

			if( usesStencil )
			{
				sceneManager_->destroyEntity( getStencilEntName() );
			}
		}
		else
		{
			GTRACE(2, "WARNING: GOgreRenderable::unload tried to remove some missing nodes; maybe removed by scene?");
		}

		raySceneQuery = NULL;
		loadingWasPerformed = false;
		GTRACE(5, "GOgreRenderable::unload done.");
	}



	void GOgreRenderable::loadObject( const Ogre::String aMeshName, const Ogre::String aMaterialName )
	{
		// Load the object related data.
		// This will require a "scene node" and an "entity", each with an unique name.
		// The sceneNode will begin with an "n" character, the entity with an "e".
		// Both will have their names continuing with the current tine, an "_"
		// (underline) and an internal counter.

		// After this point, the scene node should be enough to perform most transformations.

		meshName = aMeshName;
		materialName = aMaterialName;
		GTRACE(5, "GOgreRenderable::loadObject, meshName="<<meshName<<", matname=" <<materialName);

		guslib::Timer timer;
		double myDbl= timer.getCurrentTimeUnits();
		std::stringstream sTemp;
		sTemp <<myDbl;
		std::string myStr = sTemp.str();
		Ogre::String myString = Ogre::String("n") + Ogre::String(myStr.c_str())
			+ Ogre::String("_") + StringConverter::toString((int)creationIndex);

		Ogre::SceneNode * sceneNode;
		Ogre::Entity * entity;
		sceneNode = sceneManager_->getRootSceneNode()->createChildSceneNode(myString);
		sceneNodeName_ = sceneNode->getName();
		sceneNode->setVisible( false );

		GTRACE(5, "Created scene node [" <<myString<<","<<sceneNodeName_<<"]");

		//	make an unique name for the entity.
		myString = Ogre::String("e") + Ogre::String(myStr.c_str())
			+ Ogre::String("_") + StringConverter::toString((int)creationIndex);
		GTRACE(5, "Creating entity for mesh ["<<meshName<<"]");
		entity = sceneManager_->createEntity( myString, meshName );
		if( renderGroup != 0 )
		{
			GTRACE(5, "Set render group to "<<(int)renderGroup);
			entity->setRenderQueueGroup(renderGroup);
			//entity->setRenderQueueGroup(RENDER_QUEUE_OUTLINE_GLOW_OBJECTS);
		}
		entity->setVisible( false );
		entName = entity->getName();
		GTRACE(5, "Created mesh entity [" <<myString<<","<<meshName<<"]");


		sceneNode->attachObject( entity );
		if( materialName != "" )
		{
			entity->setMaterialName( materialName );
		}

		if( usesStencil )
		{
			// add the stencil glow.
			Ogre::Entity* outlineEntity = entity->clone(entName + std::string("_glow"));
			stencilEntName = outlineEntity->getName();
			GTRACE(5, "Created clone for stencil..."<<stencilEntName);
			if( stencilGroup != 0 )
			{
				GTRACE(5, "Set render group to "<<(int)stencilGroup);
				outlineEntity->setRenderQueueGroup(stencilGroup);
			}
			outlineEntity->setMaterialName( stencilMaterialName);
			outlineEntity->setVisible(false);
			sceneNode->attachObject(outlineEntity);
		}
		sceneNode->resetToInitialState();

		loadingWasPerformed = true;
	}


	void GOgreRenderable::requestLoading( const Ogre::String &aMeshName, const Ogre::String &aMaterialName, const Ogre::String &stencilMat,
		Ogre::uint8 rGroup, Ogre::uint8 stencilRGroup )
	{
		meshName = aMeshName;
		materialName = aMaterialName;
		stencilMaterialName = stencilMat;
		renderGroup = rGroup;
		stencilGroup = stencilRGroup;
		usesStencil = (stencilMaterialName != std::string("") );

		GBase::GResourceManager::getPtr()->requestResourceLoad( this );
	}

	void GOgreRenderable::requestUnloading()
	{
		GOgreRenderable * tempPtr = this->clonePtr();
		tempPtr->removeOgreEntitiesAtUnload = true;
		GBase::GResourceManager::getPtr()->requestResourceUnload( tempPtr );
	}




	void GOgreRenderable::render()
	{
		if( !loaded() )
			return;

		SceneNode * myNode = sceneManager_->getSceneNode( getSceneNodeName() );
		if( !myNode )
			return;

		Entity *myEntity = sceneManager_->getEntity( getEntName() );
		myNode->resetToInitialState();

		//	move relative to the mesh model.
		myNode->translate( getTranslationVector() );

		myNode->translate( getMasterTranslationVector() );

		//	rotate relative to the mesh model.
		myNode->rotate( Vector3::UNIT_Y, Ogre::Radian( getRotationVector().y ) );
		myNode->rotate( Vector3::UNIT_X, Ogre::Radian( getRotationVector().x ) );
		myNode->rotate( Vector3::UNIT_Z, Ogre::Radian( getRotationVector().z ) );

		//	rotate with animated angle.
		myNode->rotate( Vector3::UNIT_Y, Ogre::Radian( getAnimRotationVector().y ) );
		myNode->rotate( Vector3::UNIT_X, Ogre::Radian( getAnimRotationVector().x ) );
		myNode->rotate( Vector3::UNIT_Z, Ogre::Radian( getAnimRotationVector().z ) );

		// scale it.
		myNode->scale( getScalingVector() );
		myNode->scale( getAnimScalingVector() );

		if( raySceneQuery )
		{
			static Ray updateRay2;
			updateRay2.setOrigin( Vector3(myNode->getPosition().x, c_distanceForRaySceneQueries, myNode->getPosition().z) );
			updateRay2.setDirection(Vector3::NEGATIVE_UNIT_Y);
			raySceneQuery->setRay(updateRay2);
			RaySceneQueryResult& qryResult = raySceneQuery->execute();
			RaySceneQueryResult::iterator j = qryResult.begin();
			if (j != qryResult.end() && j->worldFragment)
			{
				myNode->setPosition( myNode->getPosition().x,
					j->worldFragment->singleIntersection.y,// + h/2,
					myNode->getPosition().z);
			}
		}

		Ogre::Vector3 correctionVector( GUS_CORRECTION_VECTOR_PARAMS );
		myNode->translate( correctionVector );
	}


	void GOgreRenderable::setVisible( bool value )
	{
		if( !loaded() )
			return;
		SceneNode * myNode = sceneManager_->getSceneNode( getSceneNodeName() );
		myNode->setVisible( value );
	}

}
