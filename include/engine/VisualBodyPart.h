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
//    Implementation of the base bodypart.

#include "../model/BodyPartBase.h"
#include "GOgreRenderable.h"

namespace Serpents
{
	class VisualBodyPart 
		: public BodyPartBase
		, public GOgreRenderable
	{
	public:
		///
		///	Constructor.
		///

		VisualBodyPart( Ogre::SceneManager * sceneManager );

		virtual ~VisualBodyPart();

		/// overrride for location setting.
		virtual void setLocation( GUtils::DFLOAT x, GUtils::DFLOAT y );

		/// override for angle setting.
		virtual void setAngleInRadians( double angle );

		virtual void setFraction( double value );
	};
}