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
//    Game Bonus handling classes.



#include <model/Bonus.h>
#include <guslib/trace/trace.h>
#include <guslib/system/random.h>
using namespace GUtils;

namespace Serpents
{
#if 0
	BonusReward::BonusReward(BonusRewardType brt, std::string sId, double aValue)
		: type( brt )
		, id( sId )
		, value( aValue )
	{
	}

	BonusReward::~BonusReward()
	{
	}


	//
	//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ BonusRoot ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//

	BonusRoot::BonusRoot()
	{
	}

	BonusRoot::~BonusRoot()
	{
		rewards.clear();
	}

	void BonusRoot::addReward( BonusReward &reward )
	{
		rewards.push_back(reward);
	}


	BonusRewardList BonusRoot::getRewardsCopy() const
	{ 
		return rewards;
	}

	void BonusRoot::setRewards(BonusRewardList r)
	{
		rewards.clear();
		rewards = r;
	}



	//
	//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ BonusPrototypeBase ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//

	BonusPrototypeBase::BonusPrototypeBase(std::string nameToUse)
		: BonusRoot()
		, internalName( nameToUse )
		, generationOddWeight( 1 )
	{
	}


	BonusPrototypeBase::~BonusPrototypeBase()
	{
	}



	//
	//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ BonusBase ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//

	BonusBase::BonusBase( const BonusPrototypeBase * prototype, long initialTimeToLive )
		: timeToLive( initialTimeToLive )
		, picked( false )
	{
		BonusPrototypeBase * tempBonus = (BonusPrototypeBase*) prototype;
		linkToPrototype = tempBonus;
		setShape(MapObject::ObjectShapeCircular);
		if( tempBonus )
		{
			//	copy the values of the prototype
			setRewards( tempBonus->getRewardsCopy() );
		}
	}

	BonusBase::~BonusBase(void)
	{
	}



	//
	//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ BonusManagerBase ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//

	BonusManagerBase::BonusManagerBase()
		: maximumAllowedBonuses( 15 )
		, defaultTimeToLive( 30000 )
		, maxValueForRandom( 0 ) // the randomizer 
		, minimumBonusesToKeepOnScreen( 2 )
		, maxValueForSpawnRandom( 100 )
		, genValueForSpawnRandom( 1 )
	{
		defaultSize.x = defaultSize.y = 15;
	}

	BonusManagerBase::~BonusManagerBase()
	{
		GTRACE(5, "BonusManagerBase destructor entered");
		for( PrototypeBaseList::iterator it = prototypes.begin(); it!=prototypes.end(); ++it )
		{
			delete (*it);
		}
		prototypes.clear();
		GTRACE(5, "cleared proros, clearing bonuses");
		for( BonusBaseList::iterator it = bonuses.begin(); it!=bonuses.end(); ++it )
		{
			delete (*it);
		}
		bonuses.clear();
		GTRACE(5, "BonusManagerBase destructor exiting");
	}


	bool BonusManagerBase::addBonusPrototype( BonusPrototypeBase * prototype )
	{
		if( !prototype )
			return false;
		bool nameAlreadyInUse = false;
		for( PrototypeBaseList::iterator it = prototypes.begin(); it!=prototypes.end(); ++it)
		{
			if( (*it)->getInternalName() == prototype->getInternalName() )
			{
				nameAlreadyInUse = true;
			}
		}
		if( nameAlreadyInUse )
		{
			GTRACE(1, "SEVERE: addBonusPrototype with non-unique name (in use): " << prototype->getInternalName() );
			return false;
		}
		GTRACE(4, "Prototype added with name : " <<prototype->getInternalName());
		prototypes.push_back( prototype );
		maxValueForRandom += prototype->getGenerationOddsWeight();
		return true;
	}


	
	BonusBase * BonusManagerBase::createBonusFromPrototype( const std::string & prototypeName )
	{
		return createBonusFromPrototype( getPrototypeForName( prototypeName ) );
	}



	BonusBase* BonusManagerBase::getItemAt( int index )
	{
		if( (int)bonuses.size() <= index )
			return NULL;
		return bonuses[ index ];
	}


	int BonusManagerBase::getNumberOfItems() const
	{
		return (int)bonuses.size();
	}


	BonusPrototypeBase * BonusManagerBase::getRandomBonusType()
	{
		GTRACE(5, "BonusManagerBase::getRandomBonusType()");
		int i=0; 
		maxValueForRandom = 0;
		for( PrototypeBaseList::iterator it = prototypes.begin(); it!=prototypes.end(); ++it )
		{
			maxValueForRandom += (*it)->getGenerationOddsWeight();
		}
		int x = guslib::Random::getPtr()->getValueUpTo(maxValueForRandom);

		for( i=0; i<(int)prototypes.size() && x>=0; i++ )
		{
			x -= prototypes[i]->getGenerationOddsWeight();
		}
		GTRACE(4, "Randomed to bonus type " << (i-1));
		return prototypes[i-1];
	}


	BonusPrototypeBase * BonusManagerBase::getPrototypeForName( const std::string & name )
	{
		for( PrototypeBaseList::iterator it = prototypes.begin(); it!= prototypes.end(); ++it )
		{
			if( (*it)->getInternalName() == name )
			{
				return (*it);
			}
		}
		return NULL;
	}

	void BonusManagerBase::onTimeGoesBy( double timeUnits )
	{
		GUS_LOCK_MUTEX ( listAccessMutex );
		if( bonuses.empty() )
			return;
		GTRACE(5, "BonusManagerBase::onTimeGoesBy:"<<timeUnits<<", numbonuses="<< bonuses.size() );
		for( BonusBaseList::iterator it = bonuses.begin(); it!=bonuses.end(); ++it )
		{
			if( ! *it )
				continue;
			(*it)->setTimeToLive( (*it)->getTimeToLive() - (long) timeUnits );
			if( (*it)->getTimeToLive() < 0 )
			{
				if( (*it)->getLinkToPrototype() )
				{
					GTRACE(3, "A bonus is being removed :"<< (*it)->getLinkToPrototype()->getInternalName());
				}
				delete (*it);
				bonuses.erase(it);
				if( bonuses.size() == 0 )
				{
					GTRACE(4, "eraseop caused empty vect. exiting func.");
					return;
				}
			}
		}
	}


	void BonusManagerBase::pickBonusAt( int indexInBonusList )
	{
		GTRACE(3, "Picking bonus at index " <<indexInBonusList);
		getItemAt(indexInBonusList)->setPicked();
		getItemAt(indexInBonusList)->setTimeToLive( -1 );
	}

	void BonusManagerBase::pickBonus( BonusBaseList::iterator &it )
	{
		delete (*it);
		bonuses.erase(it);
	}

	bool BonusManagerBase::removeAllBonusPrototypes()
	{
		GUS_LOCK_MUTEX( listAccessMutex );
		bool ret = true;
		for( PrototypeBaseList::iterator it = prototypes.begin(); it!=prototypes.end(); ++it )
		{
			bool canRemove = true;

			//	prototypes in use cannot be removed.
			for( BonusBaseList::iterator it2 = bonuses.begin(); it2!=bonuses.end(); ++it2 )
			{
				if( (*it2)->getLinkToPrototype() == (*it) )
				{
					GTRACE(1, "ERROR! Unable to remove prototype ["<<(*it)->getInternalName()<<"] because it is used by a bonus");
					canRemove = false;
				}
			}
			if( canRemove )
			{
				maxValueForRandom -= (*it)->getGenerationOddsWeight();
				GTRACE(5, "Removing prototype " <<(*it)->getInternalName());
				prototypes.erase( it );
				GTRACE(5, "Removed prototype.");
			}
			else
			{
				ret = false;
			}
		}
		return ret;
	}


	bool BonusManagerBase::removeBonusPrototype( BonusPrototypeBase * prototype )
	{
		if( prototype == NULL )
			return false;

		//	if the current prototype is used by any bonuses, removal is not allowed!
		for( BonusBaseList::iterator it = bonuses.begin(); it!=bonuses.end(); ++it )
		{
			if( (*it)->getLinkToPrototype() == prototype )
			{
				GTRACE(1, "ERROR! Unable to remove prototype [" << prototype->getInternalName() << "] because it is used by a bonus");
				return false;
			}
		}
		maxValueForRandom -= prototype->getGenerationOddsWeight();
		for( PrototypeBaseList::iterator pit = prototypes.begin(); pit!=prototypes.end(); ++pit )
		{
			if( (*pit) == prototype )
			{
				delete (*pit);
				prototypes.erase(pit);
			}
		}
		GTRACE(5, "removing bonus prototype...")
		return true;
	}


	bool BonusManagerBase::spawnNewBonus( DPOINT point, const BonusPrototypeBase * prototype )
	{
		if( prototype == NULL )
		{
			GTRACE(1, "ERROR: NULL prototype specified on new bonus spawn");
			return false;
		}

		GTRACE(3, "Spawning new bonus, type=[" << prototype->getInternalName() << "]");
		BonusBase * myBonus = createBonusFromPrototype(prototype);
		if( myBonus == NULL )
			return false;

		myBonus->setTimeToLive( defaultTimeToLive );
		myBonus->setPivot( MapObject::CenterPivot );
		myBonus->setSize( defaultSize );
		myBonus->setLocation( point );
		myBonus->setRewards( prototype->getRewardsCopy() );
		bonuses.push_back( myBonus );

		return true;
	}

#endif
} // namespace Serpents