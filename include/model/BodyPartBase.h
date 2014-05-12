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
//    Base class for serpent bodyparts.

//
//	Forward class def for a serpent.
//

class Serpent;

#include "utils/GGeneralUtils.h"

#include "MapObject.h"

namespace Serpents
{
#if 0
	class SerpentBase;

//	Value to show an inherited speed.
#define SPEED_INHERITED -1

	///
	///	Class to describe a single segment from the body of a serpent.
	///
	class BodyPartBase 
		: public MapObject
	{
	public:
		///	Constructor.
		BodyPartBase(void);

		///	Copy constructor.
		BodyPartBase( BodyPartBase & );

		///	Destructor.
		virtual ~BodyPartBase();

		///
		///	Get the angle describind the direction in which the object is moving.
		///	@return The value in degrees (0-360) of the angle.
		///

		virtual double getAngleInDegrees() const;

		///
		///	Get the angle describind the direction in which the object is moving.
		/// Please note that this is just a value, it is not taken as a
		///	direction relative to anything, you will need to choose a base direction for yourself.
		///	@return The value in radians of the angle.
		///

		virtual double getAngleInRadians() const;

		///
		///	Get the fraction of the bodypart that was filled so far. This will be 1 for normal bodyparts, and a number between
		///	0 and 1 for incomplete bodyparts. (0 may have the logical meaning as that of a not-yet-spawned bodypart, but
		/// beware of the fact that the engine may handle the similar notions a bit differently).
		///

		virtual double getFraction() const { return fraction; }

		/// Get the health of the bodypart.
		virtual double getHealth() const { return health; }

		///
		///	Get the speed with which the current part is moving. For the value of "SPEED_INHERITED", it means that
		///	the speed is inherited from the serpent this object is part of.
		///	@return The speed in game units per time units.
		///

		virtual double getSpeed() const;

		///
		///	Tell wether the bodypart was spawned on the map. A non-spawned bodypart in one that is not yet visible. It has been
		///	created, but not made visible yet.
		///

		virtual bool isSpawned() const;
		
		///
		///	Check to see if a bodypart collides with another bodypart. This makes the assumption that the bodyparts have a
		///	circle-shape when projected onto a plane.
		///	@param rhs The bodypart with which collision detection is performed.
		///

		virtual bool collidesWith( const BodyPartBase * rhs ) const;


		//
		virtual void setAngleInRadians( double angle ){ angleInRads = angle; }

		///
		///	Set the fraction to use.
		///	@param value The fraction to set for the bodypart. This is a value between 0 and 1. Values outside this range will be brought
		///	to the nearest correct value.
		///

		virtual void setFraction( double value );

		/// Set the health for the current bodypart;
		virtual void setHealth( double value );

	protected:

		//	The angle coresponding to the bodypart's direction of movement. Please note that this is just a value, it is not
		//	taken as a direction relative to anything, you will need to choose a base direction for yourself.
		double angleInRads;

		//	The speed the bodypart is moving at. SPEED_INHERITED means it is ignored, (serpent speed is constant for all
		//	bodyparts).
		double speed;

		//	Flag to tell if the bodypart was spawned on the map. This will be used for collision detection and very likely
		//	for the display of the viewable subclass implementation
		bool spawned;

		//  (Param added 2008.02.24)
		/// The bodypart may be allowed, depending on the implementation in the engine, to be considered as fraction of a regular bodypart.
		/// It is up to the viewable implementation of the engine to decide wether this will be drawn differently, or even drawn at all.
		double fraction;

		//  (Param added 2008.03.21)
		/// Health for the bodyparts. The health is a fraction (values between 0 and 1, with 0 being basically no health, and 1 full health).
		double health;

		friend class Serpents::SerpentBase;
	};

	typedef std::vector<BodyPartBase *> BodyPartPtrList;

#endif
} // namespace Serpents