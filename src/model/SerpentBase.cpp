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


#include <model/SerpentBase.h>
#include <guslib/trace/trace.h>

#include <stdio.h>
#include <math.h>

using namespace GUtils;

namespace Serpents
{
#if 0
	SerpentBase::SerpentBase( )
		: fractionOfLength(0.0)
		, hungerLevel(0.0)
	{
		path = new SerpentPath();
		this->turnSpeed = 1;
		this->speed = 1;
		this->speedStep = 1;
		this->timeSinceLastBonusPicked = 0;
		this->hungerGrowthRatio = 1;
		this->hungerDamagingLimit = 1;
		bodyParts.clear();
	}

	SerpentBase::~SerpentBase(void)
	{
		GTRACE(4, "SerpentBase destructr: Removing serpent & all "<< getLength() << " bodyparts");
		for( unsigned i=0; i<bodyParts.size(); i++ )
		{
			delete bodyParts[i];
		}
		bodyParts.clear();
	}


	void SerpentBase::addPart(Serpents::BodyPartBase *partToAdd)
	{
		BodyPartBase * myBodyPart = partToAdd;
		myBodyPart->setSize( spawnBodyPartSize );
		GTRACE(4, "Adding bodypart; size=" <<spawnBodyPartSize.x<<","<<spawnBodyPartSize.y);
		if( this->getLength() <= 0 )
		{
			myBodyPart->angleInRads = this->spawnAngle;
		}
		bodyParts.push_back( myBodyPart );

		this->onAddedBodyPart();
	}


	void SerpentBase::addParts( double numParts )
	{
		GTRACE(4, "SerpentBase: adding " << numParts<< " bodyparts");
		if( numParts <= 0 )
		{
			// must use removeparts function, instead of supplying a negative number.
			return;
		}
		bool canStillAdd = true;
		while( canStillAdd )
		{
			if( numParts >= 1 )
			{
				// need to add a bodypart;
				// set the currently last bp to full
				if( getLength() > 1 )
				{
					this->getBodyPart(this->getLength()-1)->setFraction( 1.0 );
				}
				BodyPartBase * myBodyPart = this->createBodyPart( bodyParts.size() == 0);
				if( myBodyPart )
				{
					myBodyPart->setSize( spawnBodyPartSize );
					GTRACE(4, "Adding bodypart; size=" <<spawnBodyPartSize.x<<","<<spawnBodyPartSize.y);
					if( this->getLength() <= 0 )
					{
						myBodyPart->setAngleInRadians(this->spawnAngle );
					}
					myBodyPart->setFraction( 1.0 + fractionOfLength );
					bodyParts.push_back( myBodyPart );
					this->onAddedBodyPart();
				}
				numParts--;
			}
			else
			{
				fractionOfLength = fractionOfLength + numParts;
				if( fractionOfLength > 0 )
				{
					// set the currently last bp to full
					if( getLength() > 1 )
					{
						this->getBodyPart(this->getLength()-1)->setFraction( 1.0 );
					}
					BodyPartBase * myBodyPart = this->createBodyPart( bodyParts.size() == 0);
					if( myBodyPart )
					{
						myBodyPart->setSize( spawnBodyPartSize );
						GTRACE(4, "Adding bodypart; size=" <<spawnBodyPartSize.x<<","<<spawnBodyPartSize.y);
						if( this->getLength() <= 0 )
						{
							myBodyPart->setAngleInRadians(this->spawnAngle );
						}
						fractionOfLength -= 1.0;
						myBodyPart->setFraction( 1.0 + fractionOfLength );
						bodyParts.push_back( myBodyPart );
						this->onAddedBodyPart();
					}
				}
				this->getBodyPart(this->getLength()-1)->setFraction( 1.0 + fractionOfLength );
				canStillAdd = false;
			}
		}
	}


	void SerpentBase::initialize( double initialLength )
	{
		maxlength = 100;
		minlength = MIN_SERP_LEN;
		spawnBodyPartSize.x = 15;
		spawnBodyPartSize.y = 15;
		addParts(initialLength);
		if( bodyParts.size()> 0 )
		{
			DPOINT hsz = {20,20};
			this->bodyParts[0]->setSize( hsz );
		}

		minSpeed = DEFAULT_MIN_SPEED;
		maxSpeed = DEFAULT_MAX_SPEED;
		setSpeed(1);
		setSpeedInfluenceOnTurn(0);
		setSpeedStep(0.001);
		fractionOfLength = 0;
	}


	void SerpentBase::onAddedBodyPart()
	{
		path->addStrip();
	}


	void SerpentBase::onRemovedBodyPart()
	{
		path->removeStrip();
	}


	void SerpentBase::removeParts( double numParts )
	{
		GTRACE(4, "SerpentBase: removing " << numParts << " bodyparts");
		bool canRemoveParts = true;
		double partsStillToRemove = numParts;
		while( canRemoveParts )
		{
			if( partsStillToRemove >= 1 )
			{
				if( getLength() - 1 <= minlength )
					canRemoveParts = false;
				if( getLength() - 1 <= MIN_SERP_LEN )
					canRemoveParts = false;
				if( canRemoveParts )
				{
					delete bodyParts[bodyParts.size()-1];
					bodyParts.pop_back();
					this->onRemovedBodyPart();
					if( fractionOfLength >= 0 )
						bodyParts[getLength()-1]->setFraction( fractionOfLength );
					else
						bodyParts[getLength()-1]->setFraction( 1.0 + fractionOfLength );
					-- partsStillToRemove;
				}
			}
			else
			{
				fractionOfLength -= partsStillToRemove;
				if( fractionOfLength <= -1 )
				{
					if( getLength() > 1 )
					{
						delete bodyParts[getLength()-1];
						bodyParts.pop_back();
						this->onRemovedBodyPart();
					}
					fractionOfLength ++;
					bodyParts[getLength()-1]->setFraction( 1.0 + fractionOfLength );
				}
				canRemoveParts = false;
			}

		}
	}


	void SerpentBase::setBodyPartSpawnSize( double width, double height )
	{
		this->spawnBodyPartSize.x = width;
		this->spawnBodyPartSize.y = height;
	}


	void SerpentBase::setSpawnPosition(double x, double y)
	{
		this->spawnPosition.x = x;
		this->spawnPosition.y = y;
	}


	void SerpentBase::spawn(double x, double y)
	{
		for( int i = 0; i< this->getLength(); i++ )
		{
			this->bodyParts[i]->setLocation(x,y);
			if( i==0)
			{
				bodyParts[i]->spawned = true;
			}
			else
			{
				bodyParts[i]->spawned = false;
			}
		}
	}


	void SerpentBase::spawn()
	{
		this->spawn( spawnPosition.x, spawnPosition.y );
	}


	
	void SerpentBase::setAngleInRadians(double angle, bool affectAll )
	{
		int maxIter = (affectAll ? 1 : this->getLength() );

		for( int i=0; i< maxIter && i< this->getLength(); i++  )
		{
			this->bodyParts[i]->angleInRads = angle;
		}
		spawnAngle = angle;
	}




	void SerpentBase::setSpeed( double  newSpeed )
	{
		this->speed = newSpeed;
		if( newSpeed < minSpeed )
		{
			this->speed = minSpeed;
		}
		if( newSpeed > maxSpeed )
		{
			this->speed = maxSpeed;
		}
		if( newSpeed < DEFAULT_MIN_SPEED )
		{
			this->speed = DEFAULT_MIN_SPEED;
		}
		if( newSpeed > DEFAULT_MAX_SPEED )
		{
			this->speed = DEFAULT_MAX_SPEED;
		}
	}


	void SerpentBase::setTurnSpeed(double newTurnSpeed)
	{
		if( newTurnSpeed > MIN_TURN_SPEED && newTurnSpeed < MAX_TURN_SPEED )
		{
			this->turnSpeed = newTurnSpeed;
		}
	}


	void SerpentBase::move( double timeUnits )
	{
		GTRACE(5, "*** SerpentBase::move");
		if( getLength() <= 0 )
			return;
		if( timeUnits <=0 )
		{
			timeUnits = DEFAULT_TIME_INTERVAL;
		}
		GTRACE(5, "*** SerpentBase::move, len="<<getLength());

		if( getLength() <= minlength )
		{
			return;
		}

		// spawn the parts that are not yet visible.

		int lastNonSpawn = getLength();
		for(int i=getLength()-1;i>=0;i-- )
		{
			if( ! bodyParts[i]->spawned )
			{
				lastNonSpawn = i;
			}
		}

		if( lastNonSpawn > 0 )
		{
			for( int i= lastNonSpawn; i < getLength(); i++ )
			{
				if( bodyParts[i]->collidesWith( bodyParts[i-1] ))
				{
					break;
				}
				else
				{
					bodyParts[i]->spawned = bodyParts[i-1]->spawned;
				}
			}
		}

		// Calculate the new location of the head.
		SPoint<double> newLocation = bodyParts[0]->getLocation();
		newLocation.x += GusUtils::Cos( bodyParts[0]->getAngleInRadians() ) * this->speed * timeUnits;
		newLocation.y -= GusUtils::Sin( bodyParts[0]->getAngleInRadians() ) * this->speed * timeUnits;

		// Distance between the path elements.
		// The head is (by standard at least) a bit larger than the rest of the bodyparts. So the distance between it
		// and another bodypart is the radius of the head + the radius of a regular bodypart.
		double xsz = bodyParts[0]->getSizeFromCenter() + (bodyParts[1] ? bodyParts[1]->getSizeFromCenter() : 0);

		// Move the head strip.
		path->moveHeadStrip( newLocation.x, newLocation.y,  xsz);

		// Move all strips, and set the bodyparts according to their location.
		path->getAndSetPositionOfAllParts( bodyParts );

	}


	//
	//	get the bodypart at a certain index
	//
	BodyPartBase* SerpentBase::getBodyPart(int index)
	{
		if( index >= (int)bodyParts.size() )
			throw std::exception("Tried to get bodypart, but index out of range!");
		return bodyParts[index];
	}


	//
	//	turn the snake left or right
	//
	void SerpentBase::turn( double angle, bool useRadians )
	{
		double angleRad = angle;
		if( ! useRadians )
		{
			angleRad = angle * GusUtils::PI() / 180 ;
		}

		BodyPartBase * temp = getBodyPart(0);
		if( temp )
		{
			temp->setAngleInRadians( temp->getAngleInRadians() + 
				angleRad * turnSpeed * (1 + speedInfluenceOnTurn * sqrt(speed)) );
		}
	}


	void SerpentBase::setFractionOfLength( double value )
	{
		if( value >=0 && value <=1 )
		{
			fractionOfLength = value;
		}
		else
		{
			if( value > 1 )
				fractionOfLength = 1;
			else // value <0
				fractionOfLength = 0;
		}
	}


	void SerpentBase::setMinAllowedSpeed( double value )
	{ 
		minSpeed = value; 
		setSpeed(getSpeed()); 
	}


	void SerpentBase::setMaxAllowedSpeed( double value )
	{
		maxSpeed = value; 
		setSpeed(getSpeed());
	}


	void SerpentBase::setSpeedStep( double value )
	{ 
		speedStep = value; 
	}


	void SerpentBase::setHungerLevel( double value )
	{
		if( value <= 0 )
		{
			hungerLevel = 0;
		}
		else
		{
			hungerLevel = value;
		}
	}

	void SerpentBase::setAlive( bool bAlive )
	{
		this->alive = bAlive;
	}

#endif
} // namespace Serpents
