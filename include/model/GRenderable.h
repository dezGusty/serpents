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
//    A renderable object's interface.

//
// Includes
//

//
// C++ system headers
//

// none

//
// Other libraries' headers
//
#include "OgrePrerequisites.h"
#include "OgreVector3.h"


// Forwards the inclusion of ogre.h
//#include <config/serpentsogre.h>



namespace Serpents
{
	class GRenderable
	{
	protected:
		virtual ~GRenderable(){}
	public:
		virtual void render() = 0;
	};


	class GMovableRenderable
		:public GRenderable
	{
	protected:
		Ogre::Vector3 animRotationVector;
		Ogre::Vector3 animScalingVector;
		Ogre::Vector3 rotationVector;
		Ogre::Vector3 scalingVector;
		Ogre::Vector3 masterTranslationVector;
		Ogre::Vector3 translationVector;

		//Ogre::SceneManager * sceneManager_;
		Ogre::String sceneNodeName_;

		static long internalCounter;
		long creationIndex;

		virtual ~GMovableRenderable(){}
		GMovableRenderable(Ogre::SceneManager * aSceneManager);

	public:
		virtual void setAnimRotationVector( Ogre::Vector3 vector ){ this->animRotationVector = vector; }
		virtual Ogre::Vector3 getAnimRotationVector(){ return this->animRotationVector; }
		virtual void setAnimScalingVector( Ogre::Vector3 vector ){ this->animScalingVector = vector; }
		virtual Ogre::Vector3 getAnimScalingVector(){ return this->animScalingVector; }
		virtual void setRotationVector( Ogre::Vector3 vector ){ this->rotationVector = vector; }
		virtual Ogre::Vector3 getRotationVector(){ return rotationVector; }
		virtual void setScalingVector( Ogre::Vector3 vector ){ this->scalingVector = vector; }
		virtual Ogre::Vector3 getScalingVector(){ return scalingVector; }
		virtual void setTranslationVector( Ogre::Vector3 vector ){ this->translationVector = vector; }
		virtual Ogre::Vector3 getTranslationVector(){ return translationVector; }

		virtual Ogre::String getSceneNodeName(){ return sceneNodeName_; }
	};
}