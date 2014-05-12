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


#include "VisualBodyPart.h"
#include <guslib/trace/trace.h>
#include "GResourceManager.h"

using namespace GUtils;

namespace Serpents
{
	VisualBodyPart::VisualBodyPart( Ogre::SceneManager * sceneManager )
		: BodyPartBase(),
		GOgreRenderable( sceneManager )
	{
	}

	VisualBodyPart::~VisualBodyPart()
	{
		GBase::GResourceManager::getPtr()->cancelLoadRequest( this );
	}
	void VisualBodyPart::setLocation( DFLOAT x, DFLOAT y )
	{
		GTRACE(5, "VisualBodyPart_settingloc");
		BodyPartBase::setLocation( x, y );
		translationVector.x = x;
		translationVector.z = y;
	}

	void VisualBodyPart::setAngleInRadians( double angle )
	{
		BodyPartBase::setAngleInRadians( angle );

		rotationVector.y = angle;
	}


	void VisualBodyPart::setFraction( double value )
	{
		GTRACE(4, "VisualBodyPart::setFraction to "<<value );
		double myValue = value;
		if( value >=0 && value <=1 )
		{
			myValue = value;
		}
		else
		{
			if( value > 1 )
				myValue = 1;
			else // value <0
				myValue = 0;
		}
		fraction = myValue;
		// make the object a bit more visible.
		animScalingVector = Ogre::Vector3::UNIT_SCALE * (fraction*0.5 + 0.5);
	}
}