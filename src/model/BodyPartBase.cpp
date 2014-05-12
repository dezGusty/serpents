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
//    Base class for serpent bodyparts.



#include <model/BodyPartBase.h>
#include <guslib/trace/trace.h>
using namespace GUtils;

namespace Serpents
{
#if 0
	BodyPartBase::BodyPartBase(void)
	{
		// Set the values inherited from the map object
		setPivot(CenterPivot);
		DRECT myRect={0,0,0,0};
		setBounds(myRect);
		DPOINT mySize = {1,1};
		setSize(mySize);
		setShape(MapObject::ObjectShapeCircular);

		// Initialize own values.
		spawned = false;
		speed = SPEED_INHERITED;
		angleInRads = 0.0 ;
		fraction = 1.0; // by default, working with full values.
		health = 1.0; // by default, parts have full health.
	}


	BodyPartBase::BodyPartBase(BodyPartBase & copy)
	{
		this->speed = copy.speed;
		this->spawned = copy.spawned;
	}


	BodyPartBase::~BodyPartBase()
	{
		GTRACE(5, "bodypart BASE deletion");
	}


	bool BodyPartBase::collidesWith( const BodyPartBase * rhs ) const
	{
		//	Circle shape cillision detection.

		double len1 = GUtils::GetSegmentLength( getLocation().x, getLocation().y, 
			rhs->getLocation().x, rhs->getLocation().y );
		double len2 = this->getSizeFromCenter() + rhs->getSizeFromCenter();

		if( len1 < len2 )
		{
			return true;
		}
		return false;
	}


	double BodyPartBase::getAngleInDegrees() const
	{
		return GusUtils::getAngleInDegreesFromRadians( this->getAngleInRadians() );
	}


	double BodyPartBase::getAngleInRadians() const
	{
		return this->angleInRads;
	}


	double BodyPartBase::getSpeed() const
	{
		return this->speed;
	}


	bool BodyPartBase::isSpawned() const
	{
		return this->spawned;
	}


	void BodyPartBase::setFraction( double value )
	{
		if( value >=0 && value <=1 )
		{
			fraction = value;
		}
		else
		{
			if( value > 1 )
				fraction = 1;
			else // value <0
				fraction = 0;
		}
	}

	void BodyPartBase::setHealth( double value )
	{
		if( value >=0 && value <=1 )
		{
			health = value;
		}
		else
		{
			if( value > 1 )
				health = 1;
			else // value <0
				health = 0;
		}
	}
#endif
} // namespace Serpents