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
//    Class to allow manipulation of a serpent.

#include "utils/GGeneralUtils.h"
#include "BodyPartBase.h"
#include "PathStrip.h"

namespace Serpents
{
	//	definitions for limit & default values
#define MIN_TURN_SPEED 0
#define MAX_TURN_SPEED 5
#define DEFAULT_MIN_SPEED 0.003
#define DEFAULT_MAX_SPEED 99
#define MIN_SERP_LEN 3
#define DEFAULT_TIME_INTERVAL 0.03

#if 0
	///
	///	The interface that will be used for callback function control. It will be implemented by all classes that
	///	need to receive messages regarding the serpents state change.
	///

	class SerpentListenerInterface
	{
	public:
		virtual void OnBodyPartSpawned( SerpentBase * serpent, int indexOfBodyPart = 0 ) = 0;
		virtual void OnBodyPartRemoved( SerpentBase * serpent, int indexOfBodyPart ) = 0;
		virtual void OnSpeedChanged( SerpentBase * serpent ) = 0;
		virtual void OnAliveStatusChanged( SerpentBase * serpent ) = 0;
	};

	///
	///	The base class for the serpents. This will provide all the serpent related background-logic support
	///	(movement on the map, adding/removing pieces, 
	///
	class SerpentBase
	{
		public:

		///
		///	Constructor.
		///

		SerpentBase();

		///
		///	Destructor. This will detroy all the contained bodyparts.
		///	NOTE: this means that should there be an animation required where th bodyparts fade after the serpent is
		///	destroyed, you must not delete the serpent before copying/moving the animation related data to another location.
		///

		virtual ~SerpentBase(void);

		///
		///	Add (append) an already created single bodypart to the serpent.
		///

		virtual void addPart( BodyPartBase * partToAdd );

		///
		///	Add a number of body parts to the serpent (default:1). The parts are appended.
		///

		virtual void addParts( double numParts = 1 );

		///
		///	This function should be usually called right after the constructor, or after the callback interface is set.
		///	It handles the initialization process.
		///	@param initialLength The number of bodyparts the serpent will have initially.
		///

		virtual void initialize( double initialLength = 0 );

		///
		///	Remove a number of bodyparts from the serpent. The parts are removed starting with the most trailing one and
		///	ending with the 1st.
		///

		virtual void removeParts( double numParts = 1 );

		///
		///	Set the location where the serpent will be spawned using the spawn() function.
		///

		virtual void setSpawnPosition( double x, double y );

		///
		///	Start the spawning process at a given location.
		///

		virtual void spawn( double x, double y );

		///
		///	Start the spawning process at the location preset with setSpawnPosition.
		///

		virtual void spawn();

		///
		///	Set the serpent's heading direction, by changing the direction of its head. One could also change all other
		///	bodyparts too (this being useful at the moment of a spawn for instance).
		///	@param angle The angle to set (in radians).
		///	@param affectAll Specify wether to set only the head's direction or each bodypart's.
		///

		virtual void setAngleInRadians( double angle, bool affectAll = true );

		///
		///	Move the serpent on a logical level, with its current heading and speed for a certain amount of time.
		///	Note the moving is actually replacing the position. Using too large timeUnits values will result in jumpiness
		///	of the location.
		///	@param timeUnits The number of time units (generally milliseconds) to carry out the move for.
		///

		virtual void move( double timeUnits );

		///
		///	Set the speed of the serpent in game speed units.
		///	@param speedUnits The amount of game speed units: 1 speed unit = 1 distance unit / 1 time unit.
		///

		virtual void setSpeed( double speedUnits );

		///
		///	Turn the serpent left or fight by a certain angle. The specified value can be either in radians
		///	(default), or in 0-360 degrees, changing the flag accordingly
		///	@param anAngle The angle with which the turning will be done. Negative values means the serpent will turn left,
		///	positive values means the serpent will turn right.
		///	@param useRadians true if the angle value is given in radians, false if in degrees.
		///

		virtual void turn( double anAngle, bool useRadians = true );

		///
		///	Getter for the serpent speed.
		///	@return The value measured in game speed units.
		///

		virtual double getSpeed(){ return speed;}

		///
		///	Getter for the serpent's length
		///	@return The number of bodyparts composing the serpent (regardless wether these bodyparts are visible or not).
		///

		int getLength(){ return (int)bodyParts.size(); }

		///
		///	Convenience getter for a bodypart at a certain index. 
		///	@param index The index to get the bodypart for (The head has the index '0').
		///	@return The bodypart for a valid index, NULL for an invalid index.
		///

		virtual BodyPartBase* getBodyPart(int index);

		///
		///	Convenience function to create a bodypart. The bodypart object is created, but not added anywhere automatically.
		///	It is the user's responsability to handle its deletion.
		///	Override this in subclasses to create visible bodyparts!
		///

		virtual BodyPartBase* createBodyPart(int=0)=0;//{return new BodyPartBase();}

		///
		///	Set the turning speed of the serpent.
		///

		void setTurnSpeed( double newTurnSpeed );

		///
		///	Set the (planar) size the bodyparts will spawn with.
		///

		virtual void setBodyPartSpawnSize( double width, double height );

		///
		///	Callback function called when a bodypart has been added.
		///

		virtual void onAddedBodyPart();

		///
		///	Callback function called everytime a bodypart has been removed.
		///

		virtual void onRemovedBodyPart();

		//	These next functions are not that important for the time being and are subject to change.

		virtual double getSpeedInfluenceOnTurn(){ return speedInfluenceOnTurn; }
		virtual void setSpeedInfluenceOnTurn( double value ){ speedInfluenceOnTurn = value; }

		virtual double getFractionOfLength(){ return this->fractionOfLength; }
		virtual double getMaxAllowedSpeed(){ return maxSpeed; }
		virtual double getMinAllowedSpeed(){ return minSpeed; }
		virtual double getSpeedStep(){ return speedStep; }
		virtual double getTurnSpeed(){ return turnSpeed; }
		virtual void setFractionOfLength( double value );
		virtual void setMinAllowedSpeed( double value );
		virtual void setMaxAllowedSpeed( double value );
		virtual void setSpeedStep( double value );
		virtual bool getAlive(){ return this->alive; }
		virtual void setAlive( bool bAlive );

		virtual double getTimeSinceLastBonusPicked(){ return timeSinceLastBonusPicked; }
		virtual void setTimeSinceLastBonusPicked( double value ){ timeSinceLastBonusPicked = value; }
		virtual double getHungerLevel(){ return hungerLevel; }
		virtual double getHungerDamagingLimit(){ return hungerDamagingLimit; }
		virtual double getHungerGrowthRatio(){ return hungerGrowthRatio; }
		virtual void setHungerLevel( double value );
		virtual void setHungerDamagingLimit( double value ){ hungerDamagingLimit = value; }
		virtual void setHungerGrowthRatio( double value ){ hungerGrowthRatio = value; }

	protected:

		//	The saved spawn position for the serpent (set via setSpawnPosition)
		GUtils::DPOINT spawnPosition;

		//	The default dimensions a bodypart will be created with. (useful only for internal game logic collision detection).
		GUtils::DPOINT spawnBodyPartSize;

		bool alive;	//	the alive status
		int maxlength;//	The maximum length this serpent is allowed.
		int minlength;//	The minimum length this serpent is allowed to have. (ignored if smaller than MIN_SERP_LEN)
		double minSpeed;
		double maxSpeed;

		//	This is the amount of bodypart units the serpent has accumulated. When this number reaches or exceedes 1, a new (or more) bodyparts
		//	is (are) created, as required. This is designed to allow the serpent to gain fractional units of length, not just full ones. (e.g.
		//  0.2). Using only integer bodyparts, the behaviour will be that of a classical serpents game.
		double fractionOfLength;

		// The hunger level for the serpent.
		double hungerLevel;

		// The ratio at which the serpent's hunger grows (1 by default; 0 => no growth; 2=> double growth, etc)
		double hungerGrowthRatio;

		// The point at which hunger becomes damaging. This will be combined with an engine similar variable. (use 1 as default)
		double hungerDamagingLimit;

		//	This is the amount the speed will increase/decrease.
		double speedStep;

		//	The internal speed of the serpent, in game units.
		double speed;

		//	The turning speed of the serpent.
		double turnSpeed;

		//	The influence the move speed has on the turning speed. This is built in so that, just like when driving a car, the result will
		//	be different when cornering at 60 KMH compared to 300 KMH, with the serpent following a different according to the two situatios.
		double speedInfluenceOnTurn;

		//	Colectia de bodyparts aferenta serpentului. Sir de pointeri de trebuie dealocat in destructor.
		BodyPartPtrList bodyParts;

		//	Control the angle at which the bodyparts will spawn
		double spawnAngle;

		// The duration (in millis) since the last moment a bonus was picked.
		double timeSinceLastBonusPicked;

	//DECLARE_SINGLE_LISTENER_CONTAINER(SerpentListenerInterface, listeners);
	public:
		//	The collection of planar segments that will compose the path the serpent is following.
		SerpentPath * path;
	};

#endif
} // namespace Serpents