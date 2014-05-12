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
//    Interface for renderable map objects.

#include <Ogre.h>
#include "GRenderable.h"

namespace Serpents
{
	///
	///	Class to implement Ogre related data for the representation of game objects.
	///
	class GOgreRenderable
		: public GMovableRenderable
	{
	protected:
		Ogre::String meshName;
		Ogre::String materialName;
		Ogre::String stencilMaterialName;
		Ogre::String entName;
		Ogre::String stencilEntName;

		Ogre::uint8 renderGroup;
		Ogre::uint8 stencilGroup;

		Ogre::RaySceneQuery * raySceneQuery;

		//	flag that specifies wether the Ogre scene node and entity associated with this class object 
		//	will be removed. If the scene node and entity are used for multiple objects, then they should not be removed from the scene.
		//	(default is true).
		bool removeOgreEntitiesAtUnload;

		//	by default, renderable objects are created from a game-logic handling thread and no loading is performed until
		//	either the rendering thread goes to action or the background resource loading thread (if existing) handles the
		//	load operation.
		//	Note (2009.02.11) : No background resource loading thread exists at the moment.
		bool loadingWasPerformed;

		bool clonedObject;

		bool temporaryObject; /// specified if this object is temporary, meaning it won't release resources upon its destruction.

		bool usesStencil; /// specify whether to use a stencil drawing as well. The stencils will be generally used for object outlining.

	public:
		///	Constructor.
		GOgreRenderable( Ogre::SceneManager * sceneManager );

		virtual GOgreRenderable* clonePtr();
		virtual GOgreRenderable* createTemporaryClone();

		///	Destructor. This frees up the node and entity.
		virtual ~GOgreRenderable();

		/// Say whether the loading of the ogre object was performed.
		bool loaded(){ return loadingWasPerformed; }
		bool cloned(){ return clonedObject; }

		///	Load the 3d data of the object. This will create the scene node and entity objects.
		/// The loading will be performed at the exact point of the function call.
		///	@param meshName The name of the 3d mesh to be used.
		virtual void loadObject( const Ogre::String meshName, const Ogre::String materialName = "" );

		/// Ask for the loading process to commence. This will cause the loading to commence in a separate
		/// thread, so be sure not to use the resources immediately afterwards.
		virtual void requestLoading( const Ogre::String &meshName, const Ogre::String &materialName = "", const Ogre::String &stencilMat = "",
			Ogre::uint8 rGroup=0, Ogre::uint8 stencilRGroup=0);

		virtual void requestUnloading();


		virtual Ogre::Vector3 getMasterTranslationVector(){ return masterTranslationVector; }
		virtual void setMasterTranslationVector( Ogre::Vector3 tVector ){ masterTranslationVector = tVector; }

		virtual Ogre::String getEntName(){ return entName; }
		virtual Ogre::String getMeshName(){ return meshName; }
		virtual Ogre::String getMaterialName(){ return materialName; }
		virtual Ogre::String getStencilEntName(){ return stencilEntName; }
		virtual Ogre::String getStencilMaterialName(){ return stencilMaterialName; }
		virtual Ogre::uint8 getRenderGroup(){ return renderGroup; }
		virtual Ogre::uint8 getStencilGroup(){ return stencilGroup; }

		virtual Ogre::RaySceneQuery * getRaySceneQuery(){ return raySceneQuery; }
		virtual void setRaySceneQuery( Ogre::RaySceneQuery *query ){ raySceneQuery = query; }
		virtual void setRenderGroup( Ogre::uint8 value ){ renderGroup = value; }
		virtual void setStencilGroup( Ogre::uint8 value ){ stencilGroup = value; }

		virtual void render();

		virtual void unload();

		virtual void setVisible( bool value );
	};


	typedef struct 
	{
		std::vector<GOgreRenderable> data;
	} GRenderSnapshotView;
}