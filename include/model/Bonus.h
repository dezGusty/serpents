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
//    Base class for bonuses.

#include "MapObject.h"
#include "utils/GGeneralUtils.h"

#include <guslib/system/thread.h>
namespace Serpents
{
#if 0
	typedef enum
	{
		BRT_UserDefined=-1, // a user defined reward.
		BRT_LengthModifier=0, //	value +/- add/substract parts
		BRT_ScoreModifier,	//	value +/- add/substract score
		BRT_TurnSpeedModifier,	//	value +/- add/substract to the current turn speed
		BRT_MoveSpeedUpperLimitModifier,	//	changes the maximum value to which a player can manually alter his move speed.
		BRT_MoveSpeedLowerLimitModifier, //	changes the minimum value to which a player can manually alter his move speed.
		BRT_HungerModifier,
		BRT_StaminaModifier,
		BRT_HealthModifier,
		BRT_InvertControlForSeconds,
		BRT_NumPropertyTypes
	}BonusRewardType;


	static BonusRewardType getRewardTypeFromString( std::string name )
	{
		BonusRewardType btt = BRT_UserDefined;
		if( name == std::string("length") ) btt = BRT_LengthModifier;
		if( name == std::string("points") ) btt = BRT_ScoreModifier;
		if( name == std::string("lowspeedlimit") ) btt = BRT_MoveSpeedLowerLimitModifier;
		if( name == std::string("upperspeedlimit") ) btt = BRT_MoveSpeedUpperLimitModifier;
		if( name == std::string("health") ) btt = BRT_HealthModifier;
		if( name == std::string("hunger") ) btt = BRT_HungerModifier;
		if( name == std::string("stamina") ) btt = BRT_StaminaModifier;
		if( name == std::string("turnspeed") ) btt = BRT_TurnSpeedModifier;
		if( name == std::string("userdefined") ) btt = BRT_UserDefined;
		return btt;
	};


	static std::string getNameFromRewardType( BonusRewardType rtype )
	{
		switch( rtype )
		{
		case BRT_LengthModifier:
			return std::string("length");
		case BRT_ScoreModifier:
			return std::string("points");
		case BRT_MoveSpeedLowerLimitModifier:
			return std::string("lowspeedlimit");
		case BRT_MoveSpeedUpperLimitModifier:
			return std::string("upperspeedlimit");
		case BRT_HealthModifier:
			return std::string("health");
		case BRT_HungerModifier:
			return std::string("hunger");
		case BRT_StaminaModifier:
			return std::string("stamina");
		case BRT_TurnSpeedModifier:
			return std::string("turnspeed");
		case BRT_UserDefined:
		default:
			return std::string("userdefined");
		}
	}


	/**
		A bonus reward is a reward contained within a bonus. 
		It will be applied to the picker of the bonus at the pick event.
		The picker class should implement the interface function.
	*/
	class BonusReward
	{
	protected:
		BonusRewardType type;
		double value;
		std::string id;
	public:
		BonusReward(BonusRewardType brt, std::string sId, double aValue=1);
		virtual ~BonusReward();

		std::string getId() const { return id; }
		BonusRewardType getType() const { return type; }
		double getValue() const { return value; }
		void setId( std::string s ){ id=s; }
		void setType( BonusRewardType aType ){ type=aType; }
		void setValue( double v ){ value=v; }
	};



	/**
		Abstract class to identify a target that can handle a certain bonus property
	*/
	class BonusRewardConsumer
	{
	public:
		virtual void handleReward( BonusReward * r )=0;
	};



	/**
		Management class for the bonus rewards.
	*/
	class BonusRewardManager
	{
	protected:
	public:
		static BonusReward * createReward( BonusRewardType brt, std::string sId, 
			double lowerLimit, double upperLimit, double step=1 );
	};


	class BonusProperty;
	class BonusBase;
	class BonusPrototypeBase;
	class BonusPropertyTargetInterface;




	typedef std::vector<BonusReward> BonusRewardList;



	/**
		The class that acts like a root to both actual bonuses as well as bonus prototypes.
		It contains a list of bonus properties.
	*/
	class BonusRoot
	{
	protected:
		BonusRewardList rewards; /// rewards for picking this bonus.

	public:
		///	Simple constructor.
		BonusRoot();
		///	Simple destructor. Removes the properties.
		virtual ~BonusRoot();

		/// Add a reward to a bonus.
		virtual void addReward( BonusReward &reward );

		///	Get the list of properties (as a pointer) for a given bonus
		virtual BonusRewardList getRewardsCopy() const;

		///	Set the list of properties this bonus has. A bonus may contain a combination of different properties.
		virtual void setRewards( BonusRewardList r );
	};



	/**	
		The class that defines bonus prototypes. A bonus prototype serves the purpose of prototyping real bonuses to 
		different configurations. Bonuses will be created with a BonusPrototype given, from which various data will be
		copied.
	*/
	class BonusPrototypeBase
		: public BonusRoot
	{
	protected:
		int generationOddWeight;	//	the odds (weight) this bonus prototype has to be generated from all possible types.
		std::string internalName;	//	the name by which the prototype will be addressed.
	public:

		///	Constuctor. Each prototype should have a name, and the name should be unique.
		BonusPrototypeBase( std::string nameToUse );

		virtual ~BonusPrototypeBase();

		/**
		Get the odds of the bonus being created. This is a weight actually, and is dependent of the overall weight
		of all the prototipes combined.
		*/
		virtual int getGenerationOddsWeight() const { return generationOddWeight; }

		virtual std::string getType() const { return std::string("base"); }

		///	Get the internal name.
		virtual std::string getInternalName() const {return internalName;}

		///	Set the odds of the bonus being created.
		virtual void setGenerationOddsWeight( int rhs ){ generationOddWeight = rhs; }
	};



	/**
		The class to define the basic bonus object. Besides the root properties, the basic bonus can tell if it 
		is picked (meaning it is to be removed soon from its manager list) and how much time it has left until it
		dissappears
	*/
	class BonusBase :
		public MapObject,
		public BonusRoot
	{
	public:
		///	Constructor.
		///	@param time The life time of the bonus
		BonusBase( const BonusPrototypeBase * prototype, long time = 30000 );

		///	Destructor.
		virtual ~BonusBase();

		///	Get the link to the prototype this bonus is based upon.
		virtual BonusPrototypeBase * getLinkToPrototype(){ return linkToPrototype;}

		///	Get the picked status for the bonus. The bonuses are marked as picked by outer classes, and at the next iterations
		///	of the manager's action functions, they will be removed.
		virtual bool getPicked(){ return picked; }

		///	Get the time this bonus still has left before it vanishes.
		virtual long getTimeToLive(){ return timeToLive; }

		///	Set the time (in time units) this bonus has before it vanishes and is removed.
		virtual void setTimeToLive( long time ){ this->timeToLive = time; }

		///	Set the picked status for the current bonus. After the bonus is set as picked, it will be removed at the next
		///	iterations of the manager's action functions.
		virtual void setPicked( bool picked = true ){ this->picked = picked; }
		
	protected:
		BonusPrototypeBase * linkToPrototype;	//	the link to the prototype of the bonus
		long timeToLive;	//	the time this bonus has to live.
		bool picked;	//	the picked status of the bonus.

		friend class BonusManagerBase;
	};



	typedef std::vector<BonusBase *> BonusBaseList;
	typedef std::vector<BonusPrototypeBase *> PrototypeBaseList;



	///
	///	Class meant to easen the access to the available bonuses, by providing an unified location to access them from.
	///	This is the main entry point in the bonus handling side of the engine.
	///
	class BonusManagerBase
	{
	protected:
		BonusBaseList bonuses;	//	list of bonus objects
		PrototypeBaseList prototypes;	//	list of prototypes
		int maxValueForRandom;	//	internal value. the odds of bonuses spawning (from prototypes) are added into this value;
		long defaultTimeToLive;	//	by default, each new bonus will be given this time to live value.
		GUtils::DPOINT defaultSize;		//	the default planar size of the bonus (normally all bonuses will have the same size).
		int maximumAllowedBonuses;	//	the maximum number of bonuses this manager is allowed to control( the manager is not allowed to have more than this number of bonuses on screen)
		int minimumBonusesToKeepOnScreen;	//	the manager is not allowed to have less than this number of bonuses on screen).

		int maxValueForSpawnRandom;
		int genValueForSpawnRandom;

		GUS_MUTEX (listAccessMutex);

	public:
		BonusManagerBase();

		virtual ~BonusManagerBase();

		///	Add a prototype to the list of bonus prototypes.
		///	@return true if the prototype was added successfully, false otherwise (maybe the name of the prototype already exists).
		virtual bool addBonusPrototype( BonusPrototypeBase * prototype );

		///	Create a bonus using a given prototype. Please override this function to handle a different type of bonus.
		virtual BonusBase * createBonusFromPrototype( const BonusPrototypeBase * prototype )=0;

		///
		///	Create a bonus using a prototype given by name. Do not override this function since all it does is to
		///	call the "createBonus" function with the prototype parameter.
		///
		virtual BonusBase * createBonusFromPrototype( const std::string & prototypeName );

		///
		///	Create a bonus prototype using the name provided. Please override this function in child classes to handle 
		///	different types of prototypes.
		///
		virtual BonusPrototypeBase * createPrototype( const std::string & prototypeName ) = 0;

		/// Get the bonuses
		virtual BonusBaseList &getBonuses(){ return bonuses; }

		///	Getter for the BONUS item at a given position.
		virtual BonusBase * getItemAt( int index );

		//virtual guslib::GMutex & getListAccessMutex(){ return listAccessMutex;}

		///
		///	Getter for the default size to be used for each of the bonuses. Please note that this size does not imply that the
		///	same size will be used for the presentation level. It will only be used for determining collision detection between
		///	entities on the map.
		///
		virtual GUtils::DPOINT getDefaultSize(){ return defaultSize; }

		///	Getter for the default time a bonus will be present on the map.
		virtual long getDefaultTimeToLive(){ return defaultTimeToLive; }

		///	Getter fot the maximum number of bonuses that are allowed to be present on the screen at a certain point.
		virtual int getMaximumAllowedBonuses(){ return maximumAllowedBonuses; }

		virtual int getMaxValueForSpawnRandom(){ return maxValueForSpawnRandom ; }
		virtual int getGenValueForSpawnRandom(){ return genValueForSpawnRandom ; }

		///	Get the actual number of BONUS items in the list of bonuses. (no prototypes are used for the calculation)
		virtual int getNumberOfItems() const;

		///	Convenience function to retrieve a random type of bonus to be placed on the map. 
		virtual BonusPrototypeBase * getRandomBonusType();

		///	Get the actual number of items in the list of bonuses. (no prototypes are used for the calculation).
		int getLength() const { return getNumberOfItems(); }

		///	Access a bonus prototype with a given name. Returns NULL for invalid names.
		virtual BonusPrototypeBase * getPrototypeForName( const std::string & name );

		///	Utility function to tell if the number of minimum bonuses to keep on screen is below the required amount.
		virtual bool mustCreateABonusNow(){ return minimumBonusesToKeepOnScreen > getNumberOfItems();}

		
		/// React to a certain amount of time having passed.
		/// @param timeUnits Game time units used (most likely milliseconds)
		virtual void onTimeGoesBy( double timeUnits );

		///
		///	Mark one of the bonuses as picked up by the players on the map. This will cause the bonus to be removed as soon as
		///	possible.
		///
		virtual void pickBonusAt( int indexInBonusList );

		virtual void pickBonus( BonusBaseList::iterator &it );

		///
		///	Remove all prototypes. Please note that any prototype in use of different bonuses in the list will not be removable.
		///
		virtual bool removeAllBonusPrototypes();

		///
		///	Remove a certain bonus prototype. This will fail if the given prototype is in use.
		///
		virtual bool removeBonusPrototype( BonusPrototypeBase * prototype );

		///
		///	Setter for the default size of the bonuses.Please note that this size does not imply that the
		///	same size will be used for the presentation level. It will only be used for determining collision detection between
		///	entities on the map.
		///
		virtual void setDefaultSize( GUtils::DPOINT sz ){ defaultSize = sz; }

		///
		///	Setter for the default time a bonus will be present on the map.
		///
		virtual void setDefaultTimeToLive( long rhs ){ defaultTimeToLive = rhs; }

		///	Setter fot the maximum number of bonuses that are allowed to be present on the screen at a certain point.
		virtual void setMaximumAllowedBonuses( int rhs ){ maximumAllowedBonuses = rhs; }

		virtual void setMaxValueForSpawnRandom( int value ){ maxValueForSpawnRandom = value; }
		virtual void setGenValueForSpawnRandom( int value ){ genValueForSpawnRandom = value; }

		///	Setter for the minimum number of bonuses to try to keep on screen.
		virtual void setMinimumBonusCount( int rhs ){ minimumBonusesToKeepOnScreen = rhs; }

		///
		///	Spawn (Create) a new bonus at a given location. The bonus is added to the list of bonuses present on the map.
		///	This allows the creation of different types of bonuses.
		///
		virtual bool spawnNewBonus( GUtils::DPOINT location, const BonusPrototypeBase * prototype );

		///
		///	Spawn (Create) a new bonus at a given location. The bonus is added to the list of bonuses present on the map.
		///	This allows the creation of different types of bonuses.
		///
		virtual bool spawnNewBonus( GUtils::DPOINT location, std::string prototypeName ){ return spawnNewBonus(location,getPrototypeForName(prototypeName));}
	};

#endif

} // namespace Serpents