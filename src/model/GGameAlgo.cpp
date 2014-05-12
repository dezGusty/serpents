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


#include <model/GGameAlgo.h>
#include "GBaseSetting.h"
#include <guslib/trace/trace.h>
#include <guslib/system/random.h>

using namespace GBase;
using namespace GUtils;

namespace Serpents
{
	GGameSession::GGameSession()
	{
	}



	GameSessionSettings GGameSession::getSettings()
	{
		return settings;
	}


	void GGameSession::setSettings( GameSessionSettings someSettings )
	{
		settings = someSettings;
	}




	// -------------------------------- GGameAlgo ----------------------------------


	GGameAlgo::GGameAlgo()
		: serpent( NULL )
		, bonusManager( NULL )
		, gameMap( NULL )
		, started( false )
		, paused( false )
		, engine(NULL)
		, scoringTable( NULL )
		, standardHungerGrowthRate( 0.003 )
		, standardHungerQuench( 10 )
		, standardHealthDropInterval( 2000 ) // millis for health decrease when hunger at 100%
		, standardHealthDropRate( 0.1 ) // fraction of health decrease
		, standardHealthDropTimer( 0.0 )
		, standardHungerDamagingLimit( 100 )
	{
	}


	GGameAlgo::~GGameAlgo()
	{
	}


	bool GGameAlgo::start()
	{
		if( !bonusManager || !gameMap || !serpent)
		{
			return false;
		}

		serpent->initialize();
		DFLOAT myX = GBaseAppSettings::getPtr ()->level.lvl.playerSpawn.x;
		DFLOAT myY = GBaseAppSettings::getPtr ()->level.lvl.playerSpawn.y;
		serpent->setSpawnPosition(myX, myY);

		DFLOAT myRad = GBaseAppSettings::getPtr ()->level.lvl.playerSpawnDirection;
		serpent->setAngleInRadians( myRad );

		serpent->setTurnSpeed( 1.7 );
		serpent->setSpeedInfluenceOnTurn( 1 );
		serpent->setMinAllowedSpeed( 0.05 );
		serpent->setMaxAllowedSpeed( 0.20 );
		serpent->setSpeed( 0.05 );
		serpent->setSpeedStep( 0.005 );
		serpent->addParts(5);
		serpent->setAlive( true );
		if( listener ) listener->OnPlayerSpeedChange( serpent->getSpeed() );

		GTRACE(4, "Spawning serpent...");
		serpent->spawn();

		if( scoringTable )
		{
			scoringTable->createScoreEntryForCurrentGame();
		}
		started = true;
		paused = false;
		if( listener ) listener->OnPlayerHealthChange( serpent->getBodyPart(0)->getHealth() );
		GTRACE(4, "GGameAlgo::started");
		return true;
	}


	void GGameAlgo::actionLoop(guslib::TimeUnits elapsedTimeInMillis)
	{
		GTRACE(5, "GGameAlgo actionLoop, time passed : " << elapsedTimeInMillis);
		if( elapsedTimeInMillis <= 0 )
			return;
		if( !started )
			return;

		reactToInput(elapsedTimeInMillis);

		if( ! bonusManager || !gameMap || !serpent )
		{
			return;
		}

		GTRACE(5, "GGameAlgo::actionLoop doing serpent movement");
		this->serpent->move( elapsedTimeInMillis );
		this->serpent->setTimeSinceLastBonusPicked( serpent->getTimeSinceLastBonusPicked() + elapsedTimeInMillis );
		this->serpent->setHungerLevel( this->serpent->getHungerLevel() 
			+ standardHungerGrowthRate * this->serpent->getHungerGrowthRatio() * elapsedTimeInMillis );

		// Limit to the maximum hunger decrease.
		if( serpent->getLength() > 0 )
		{
			if( this->serpent->getHungerLevel() >=
				this->serpent->getHungerDamagingLimit() * this->standardHungerDamagingLimit )
			{
				serpent->setHungerLevel( this->serpent->getHungerDamagingLimit() * this->standardHungerDamagingLimit );
				this->standardHealthDropTimer += elapsedTimeInMillis;
				if( standardHealthDropTimer >= standardHealthDropInterval )
				{
					standardHealthDropTimer -= standardHealthDropInterval;
					serpent->getBodyPart(0)->setHealth( serpent->getBodyPart(0)->getHealth() - standardHealthDropRate );
					if( listener ) listener->OnPlayerHealthChange( serpent->getBodyPart(0)->getHealth() );
				}
			}
		}
		if( listener )	listener->OnPlayerHungerChange( serpent->getHungerLevel() );

		GTRACE(5, "GGameAlgo::actionLoop doing bonusMgr action");
		if( bonusManager!= NULL )
		{
			GTRACE(5, "GGameAlgo::actionLoop calling onTimeGoesBy");
			bonusManager->onTimeGoesBy( elapsedTimeInMillis );
			GTRACE(5, "GGameAlgo::actionLoop called onTimeGoesBy");

			if( bonusManager->getNumberOfItems() < bonusManager->getMaximumAllowedBonuses() )
			{
				long r= bonusManager->getGenValueForSpawnRandom() + 1;
				static long myX;
				myX += (long)elapsedTimeInMillis;
				if( myX > 50 )
				{
					myX-=50;
					r= guslib::Random::getPtr()->getValueUpTo( bonusManager->getMaxValueForSpawnRandom() );
				}

				if( r < bonusManager->getGenValueForSpawnRandom() || bonusManager->mustCreateABonusNow() )
				{
					if( gameMap != NULL )
					{
						if( bonusManager->mustCreateABonusNow() )
						{
							GTRACE(4, "Engine Base: must spawn bonus");
						}

						//	try to spawn the bonus

						//	first, clear the old collision map.
						gameMap->clearCollisionMap();

						//	now build the new collision map adding the already existing entities (bonuses, serpent bodyparts, etc.)
						for( int i=0; i< bonusManager->getLength(); i++ )
						{
							if( bonusManager->getItemAt(i)->getTimeToLive() > 0 )
							{
								this->gameMap->addObjectToCollisionMap( bonusManager->getItemAt(i));
							}
						}
						for( int j=0; j< serpent->getLength(); j++ )
						{
							this->gameMap->addObjectToCollisionMap( serpent->getBodyPart(j) );
						}


						//	get a free position to set the new object.
						DPOINT point;
						if( gameMap->getFreeSpotForSize( bonusManager->getDefaultSize(), &point ) )
						{
							GTRACE(4, "Engine base: Creating new bonus.");
							BonusPrototypeBase * bonusProt = bonusManager->getRandomBonusType();
							GTRACE(4, "Engine base: Spawning new bonus at ["<<point.x<<","<<point.y<<"]");
							if( ! bonusManager->spawnNewBonus( point, bonusProt ) )
							{
								GTRACE(1, "ERROR! Unable to spawn bonus.");
							}
							else
							{
								GTRACE(3, "Engine base: Spawned new bonus of type: " <<bonusProt->getInternalName());
								if( listener ) listener->OnBonusSpawned( bonusManager->getBonuses().back() );
							}
						}
					}
				}
			}
		}

		//GTRACE(4, "GGameAlgo::actionLoop doing serpent damage");

		if( serpent!= NULL && serpent->getAlive() )
		{
			// check to see if the player is outside the "safe zone" and in the "damage area"
			if( serpent->getLength() > 0 && serpent->getBodyPart(0)->isSpawned() )
			{
				DRECT r1 = serpent->getBodyPart(0)->getBounds();
				DRECT r2 = gameMap->getSafeArea();
				if( r1.left < r2.left || r1.right > r2.right || r1.top < r2.top || r1.bottom > r2.bottom )
				{
					DRECT r3 = gameMap->getDamageArea();
					DFLOAT distx=0;
					DFLOAT disty=0;
					DFLOAT width=0;
					DFLOAT height=0;

					// serpent takes damage in these 4 cases
					if( r1.left < r2.left )
					{
						distx = r2.left-r1.left;
						width = r2.left-r3.left;
					}
					if( r1.right > r2.right )
					{
						distx = r1.right-r2.right;
						width = r3.right-r2.right;
					}
					if( r1.top < r2.top )
					{
						disty = r2.top - r1.top;
						height = r2.top - r3.top;
					}
					if( r1.bottom > r2.bottom )
					{
						disty = r1.bottom-r2.bottom;
						height = r3.bottom-r2.bottom;
					}

					DFLOAT xRatio = 0;
					DFLOAT yRatio = 0;
					if( width !=0 ) xRatio =distx/width;
					if( height !=0) yRatio = disty/height;

					// only apply the damage for the higher ratio;
					DFLOAT damage = (xRatio > yRatio) ? xRatio : yRatio;
					GTRACE(5, "safe area (x1,y1,x2,y2) "<<r2.left<<","<<r2.top<<","<<r2.right<<","<<r2.bottom);
					GTRACE(5, "dmg area (x1,y1,x2,y2) "<<r3.left<<","<<r3.top<<","<<r3.right<<","<<r3.bottom);
					GTRACE(5, "ratio (x, y)=" <<xRatio <<","<<yRatio<<"; dist=(x,y) "<<distx<<" , "<<disty <<"; size(x,y)=" << width <<","<<height);
					GTRACE(4, "Serpent is in damage area, applying damage of "<<damage);
					damage *= gameMap->getDamageAreaDamageRatio() * elapsedTimeInMillis/1000;
					serpent->getBodyPart(0)->setHealth( serpent->getBodyPart(0)->getHealth() - damage );
					if( listener ) listener->OnPlayerHealthChange( serpent->getBodyPart(0)->getHealth() );
				}
			}
		}


		GTRACE(5, "checking collisions and bonus capture");
		verifyCollisions();

		verifyBonusCapture();

		//	verify end-game conditions
		//GTRACE(4, "checking end-game conditions...");

		if( serpent && serpent->getLength() > 0 && this->serpent->getBodyPart(0)->getHealth() <= 0 )
		{
			this->serpent->setAlive(false);
			GTRACE(4, "Head health ="<<serpent->getBodyPart(0)->getHealth());
		}

		if( ! this->serpent->getAlive() )
		{
			stopGameSession();
			if( listener )
			{
				listener->OnGameFinished();
			}
		}
		GTRACE(5, "GGameAlgo actionLoop, done");
	}


	void GGameAlgo::pause()
	{
		paused = true;
	}

	void GGameAlgo::resume()
	{
		paused = false;
	}

	void GGameAlgo::lapseTime()
	{
		guslib::TimeUnits duration = timer.renew();

		if( ! isPaused() )
		{
			actionLoop( duration );
		}
	}



	void GGameAlgo::reactToInput( guslib::TimeUnits elapsedTimeInMillis )
	{
	}



	void GGameAlgo::setStateListener( GGameAlgoStateListener * lis )
	{
		listener = lis;
	}


	void GGameAlgo::startGameSession( GGameSession aSession )
	{
	}

	void GGameAlgo::setBonusManager(BonusManagerBase *mgr)
	{
		bonusManager = mgr;
	}

	void GGameAlgo::setMap(Map *aMap)
	{
		gameMap = aMap;
	}

	void GGameAlgo::setEngine( GBaseEngine * anEngine )
	{
		engine = anEngine;
	}

	void GGameAlgo::setSerpent( SerpentBase * srp )
	{
		serpent = srp;
	}

	void GGameAlgo::setScoringTable(GScoringTable *aTable)
	{
		scoringTable = aTable;
	}




	void GGameAlgo::stopGameSession()
	{
		started = false;
	}

	void GGameAlgo::verifyBonusCapture()
	{
		guslib::GScopedLock myLock (bonusManager->getListAccessMutex() );
		if( bonusManager == NULL || serpent == NULL || serpent->getLength() <= 0 )
			return;
		for( BonusBaseList::iterator it = bonusManager->getBonuses().begin();
			it!= bonusManager->getBonuses().end(); ++it )
		{
			MapObject * comp = NULL;
			if( serpent && serpent->getLength() > 0 )
				comp = serpent->getBodyPart(0);
			if( !comp )
				return;
			if( (*it)->intersectsWith( comp ) )
			{
				if( (*it)->getLinkToPrototype() )
				{
					GTRACE(4, "A bonus intersects with the serpent head: "<<(*it)->getLinkToPrototype()->getInternalName() );
				}
				for( unsigned k=0; k<(*it)->getRewardsCopy().size(); k++ )
				{
					BonusReward rew = (*it)->getRewardsCopy()[k];
					GTRACE(4, "bonus reward="<<getNameFromRewardType(rew.getType()));
					switch( rew.getType() )
					{
					case BRT_LengthModifier:
						if( rew.getValue() >= 0 )
						{
							GTRACE(4, "calling serpent add parts : "<<rew.getValue());
							serpent->addParts( rew.getValue() );
							GTRACE(4, "added parts... len="<< serpent->getLength() <<"; partial len="<<serpent->getFractionOfLength());
							if( listener )
							{
								listener->OnPlayerLengthChange( serpent->getLength() );
							}
						}
						else
						{
							GTRACE(4, "calling serpent remove parts : "<<rew.getValue());
							serpent->removeParts( - rew.getValue() );
							GTRACE(4, "removed parts...");
							if( listener )
							{
								listener->OnPlayerLengthChange( serpent->getLength() );
							}
						}
						break;
					case BRT_ScoreModifier:
						if( scoringTable )
						{
							GTRACE(4, "applying score.");
							long newVal = (long) scoringTable->getCurrentEntry().get()->getValue();
							newVal += (long)rew.getValue();
							scoringTable->updateCurrentScore( newVal );
							GTRACE(4, "applied score.");
							if( listener )
							{
								listener->OnPlayerScoreChange( newVal );
							}
						}
						break;
					case BRT_HealthModifier:
						GTRACE(4, "Setting health...");
						serpent->getBodyPart(0)->setHealth( serpent->getBodyPart(0)->getHealth() + rew.getValue() );
						if( serpent->getBodyPart(0)->getHealth() > 1 )
						{
							serpent->getBodyPart(0)->setHealth(1);
						}
						if( listener ) listener->OnPlayerHealthChange( serpent->getBodyPart(0)->getHealth() );
						GTRACE(4, "Set health...");
						break;
					case BRT_HungerModifier:
						if( serpent )
						{
							GTRACE(4, "Setting hunger...");
							serpent->setHungerLevel( serpent->getHungerLevel() + rew.getValue() );
							if( listener )	listener->OnPlayerHungerChange( serpent->getHungerLevel() );
							GTRACE(4, "Set. hunger...");
						}
						break;
					case BRT_MoveSpeedUpperLimitModifier:
						GTRACE(4, "Setting setMaxAllowedSpeed...");
						serpent->setMaxAllowedSpeed( serpent->getMaxAllowedSpeed() + rew.getValue()* serpent->getSpeedStep() );
						GTRACE(4, "Set setMaxAllowedSpeed...");
						break;
					case BRT_MoveSpeedLowerLimitModifier:
						GTRACE(4, "Setting setMinAllowedSpeed...");
						serpent->setMinAllowedSpeed( serpent->getMinAllowedSpeed() + rew.getValue()* serpent->getSpeedStep() );
						GTRACE(4, "Set setMinAllowedSpeed...");
						break;
					}// end switch
				}//endfor
				if( listener ) listener->OnBonusPicked( *it );
				// NOTE: pickbonus does not lock the bonuses stl container
				bonusManager->pickBonus( it );

				// force exit at next step;
				it= bonusManager->getBonuses().end();

				//GTRACE(4, "resetting bonus pickup timer...");
				serpent->setTimeSinceLastBonusPicked( 0 );
				serpent->setHungerLevel( serpent->getHungerLevel() - this->standardHungerQuench );
				if( listener )	listener->OnPlayerHungerChange( serpent->getHungerLevel() );
				standardHealthDropTimer = 0;
				//TODO: sent callback to a listener that a bonus was picked?
				GTRACE(4, "done reward apply...");
				it = bonusManager->getBonuses().begin();
			}//endif intersects
		}//endfor
	}

	void GGameAlgo::verifyCollisions()
	{
		// self- collisions

		//GusLogger::getInstancePtr()->writeLineF("verifycollisions for  serpent %d (len=%d)", i, serpent->getLength() );
		for( int j=2; j< serpent->getLength(); j++ )
		{
			if( serpent->getBodyPart(j)->isSpawned() )//only spawned parts count for collision.
			{
				if( serpent->getBodyPart(0)->intersectsWith( serpent->getBodyPart(j) ) )
				{
					serpent->getBodyPart(0)->setHealth(0.0);
					serpent->getBodyPart(j)->setHealth(0.0);
					if( listener ) listener->OnPlayerHealthChange( serpent->getBodyPart(0)->getHealth() );
					GTRACE(4, "Head of serpent collided with bodypart="<<j);
					serpent->setAlive( false );
					j = serpent->getLength();
				}
			}
		}

		//	collisions with the border of the map

		if( serpent->getLength() > 0 && serpent->getBodyPart(0)->isSpawned() )
		{
			DRECT r1 = serpent->getBodyPart(0)->getBounds();
			DRECT r2 = gameMap->getDamageArea();
			if( r1.left < r2.left || r1.right > r2.right || r1.top < r2.top || r1.bottom > r2.bottom )
			{
				GTRACE(4, "Head of serpent outside damage area. head="<<GusUtils::rectToString(r1)
					<< ", damage area="<<GusUtils::rectToString(r2) );
				serpent->setAlive( false );
			}
		}

		//	collisions with other objects on the map
	}


}