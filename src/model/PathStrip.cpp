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
//    Class to represent paths on a planar surface.


#include <model/PathStrip.h>
#include <model/BodyPartBase.h>
#include <math.h>
#include <stdio.h>
using namespace GUtils;

namespace Serpents
{
#if 0
	PathStrip::PathStrip()
	{
		headLocation.x = 0;
		headLocation.y = 0;

		tailLocation.x = 0;
		tailLocation.y = 0;
	}


	PathStrip::PathStrip( double xhead, double yhead, double xtail, double ytail )
	{
		this->headLocation.x = xhead;
		this->headLocation.y = yhead;

		this->tailLocation.x = xtail;
		this->tailLocation.y = ytail;
	}


	PathStrip::PathStrip( PathStrip & rhs )
	{
		this->headLocation.x = rhs.headLocation.x;
		this->headLocation.y = rhs.headLocation.y;

		this->tailLocation.x = rhs.tailLocation.x;
		this->tailLocation.y = rhs.tailLocation.y;
	}


	PathStrip::~PathStrip(void)
	{
	}


	double PathStrip::getLength()
	{
		return GUtils::GetSegmentLength( tailLocation.x, tailLocation.y, headLocation.x, headLocation.y );
	}


	DPOINT PathStrip::getLocationFromHead( double partialSegmentSize, bool allowGreaterSize )
	{
		DPOINT ret = this->headLocation;

		double len = getLength();
		if( len > 0 )
		{
			if( partialSegmentSize > len && !allowGreaterSize )
			{
				partialSegmentSize = len;
			}
			double ratio = partialSegmentSize / len;
			ret.x = headLocation.x - (headLocation.x - tailLocation.x) * ratio;
			ret.y = headLocation.y - (headLocation.y - tailLocation.y) * ratio;

			if( ratio >= 1 )
			{
				// This is not yet handled.
			}
		}
		return ret;
	}



	//
	// ------------------------------------------------------------------------------
	//


	SerpentPath::SerpentPath()
	{
		partArrangementWasCompleted = true;
	}


	SerpentPath::~SerpentPath()
	{
		for( int i=0; i< (int) pathStrips.size(); i++ )
		{
			delete pathStrips[i];
		}
	}

	void SerpentPath::addStrip( int numParts )
	{
		int partsToAdd = numParts;
		if( partsToAdd <= 0 )
		{
			return;
		}
		for( int i=0; i<partsToAdd; i++ )
		{
			PathStrip * strip = new PathStrip();

			// Allocate if required.

			if( getLength() > 0 )
			{
				strip->headLocation.x = pathStrips[ getLength() - 1 ]->tailLocation.x;
				strip->headLocation.y = pathStrips[ getLength() - 1 ]->tailLocation.y;
			}

			pathStrips.push_back( strip );
		}
	}


	//
	void SerpentPath::getAndSetPositionOfAllParts( BodyPartPtrList &parts/*, int len*/ )
	{
		double extraDistance = 0;
		int tempIndex = 0;
		int latestSetIndex = 0;

		// set the serpent's head on the first strip's central location.

		int lastSpawned = 0;
		parts[0]->setLocation( pathStrips[0]->headLocation );

		// process all entries that are spawned.
		for( int i=1; i < (int)parts.size() && tempIndex < getLength() ; i++ )
		{
			if( parts[i]->isSpawned() )
			{
				// move along the path with the radius of the current and previous entries.
				extraDistance += parts[i]->getSizeFromCenter() * parts[i]->getFraction() 
					+ parts[i-1]->getSizeFromCenter() * parts[i-1]->getFraction();

				// trim the extra-locations when the distance is very small. This should occur very, very rarely.
				// the only time it would be encountered would be in the case of a strong acceleration/deceleration.
				while( extraDistance >= pathStrips[ tempIndex ]->getLength() && tempIndex < getLength() )
				{
					extraDistance -= pathStrips[ tempIndex ]->getLength();
					tempIndex ++;
				}

				parts[i]->setLocation( pathStrips[ tempIndex ]->getLocationFromHead( extraDistance ) );
				latestSetIndex = i;
				lastSpawned = i;
			}
			else
			{
				break;
			}
		}
	}



	DPOINT SerpentPath::getCoords(int index)
	{
		DPOINT ret = {0,0};
		if( index < getLength() )
		{
			ret.x = pathStrips[index]->headLocation.x;
			ret.y = pathStrips[index]->headLocation.y;
		}
		return ret;
	}


	void SerpentPath::moveHeadStrip( double x, double y, double minLength )
	{
		if( getLength() > 0  )
		{
			if( pathStrips[0]->getLength() > minLength && minLength != -1 )
			{
				for( int i = getLength()-1 ; i>0 ; i-- )
				{
					pathStrips[i]->headLocation.x = pathStrips[i-1]->headLocation.x;
					pathStrips[i]->headLocation.y = pathStrips[i-1]->headLocation.y;
					pathStrips[i]->tailLocation.x = pathStrips[i-1]->tailLocation.x;
					pathStrips[i]->tailLocation.y = pathStrips[i-1]->tailLocation.y;
				}
				if( getLength() >= 1 )
				{
					pathStrips[0]->headLocation.x = x;
					pathStrips[0]->headLocation.y = y;
					if( getLength() > 1 )
					{
						pathStrips[0]->tailLocation.x = pathStrips[1]->headLocation.x;
						pathStrips[0]->tailLocation.y = pathStrips[1]->headLocation.y;
					}
				}
			}
			else
			{
					pathStrips[0]->headLocation.x = x;
					pathStrips[0]->headLocation.y = y;
			}
		}
	}


	void SerpentPath::removeStrip()
	{
		pathStrips.pop_back();
	}

#endif
} // namespace Serpents