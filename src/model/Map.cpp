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
//    Base class map collision representation.


#include <model/map.h>
#include <guslib/trace/trace.h>
#include <guslib/system/random.h>

using namespace GUtils;
namespace Serpents
{
	Map::Map( double width, double height, const IPOINT numCells )
	{
		cellMatrix = NULL;
		createMap( 0, 0, width, height, numCells );
	}


	Map::Map( double left, double top, double right, double bottom, const IPOINT numCells )
	{
		cellMatrix = NULL;
		createMap( left, top, right, bottom, numCells );
	}


	void Map::createMap( double left, double top, double right, double bottom, const IPOINT mynumCells )
	{
		// by default, there is no damage area section, since it is identical with the safe area, and the safe area has priority

		this->bonusDropArea.left = left;
		this->bonusDropArea.top = top;
		this->bonusDropArea.right = right;
		this->bonusDropArea.bottom = bottom;

		this->damageArea.left = left;
		this->damageArea.top = top;
		this->damageArea.right = right;
		this->damageArea.bottom = bottom;

		this->safeArea.left = left;
		this->safeArea.top = top;
		this->safeArea.right = right;
		this->safeArea.bottom = bottom;

		IPOINT ipnumCells = mynumCells;
		if( ipnumCells.x <= 0 )
			ipnumCells.x = 10;
		if( ipnumCells.y <= 0 )
			ipnumCells.y = 10;

		this->numCells.x = ipnumCells.x;
		this->numCells.y = ipnumCells.y;
		//this->cellSize.x = getWidth()/numCells.x;
		//this->cellSize.y = getHeight()/numCells.y;
		this->cellSize.x = (bonusDropArea.right - bonusDropArea.left)/numCells.x;
		this->cellSize.y = (bonusDropArea.bottom - bonusDropArea.top)/numCells.y;

		cellMatrix = new MapCell**[numCells.x];
		for( int i=0; i<numCells.x; i++ )
		{
			cellMatrix[i] = new MapCell*[numCells.y];
			for( int j=0; j<numCells.y; j++ )
				cellMatrix[i][j] = new MapCell();
		}
	}

	Map::~Map()
	{
		GTRACE(4, "Map: destructor begin");
		int iwidth = this->numCells.x;
		int iheight = this->numCells.y;
		for( int i=0; i< iwidth; i++ )
		{
			for( int j=0; j< iheight; j++ )
			{
				delete cellMatrix[i][j];
				//cellMatrix[i][j].elementArray.removeAll();
			}
			if(cellMatrix[i]!=NULL){delete cellMatrix[i];}
		}
		if(cellMatrix!=NULL){delete cellMatrix;}
		GTRACE(4, "Map: destructor done");
	}


	void Map::addObjectToCollisionMap( MapObject * obj )
	{
		int x1 = (int)(obj->getBounds().left / cellSize.x);
		int x2 = (int)(obj->getBounds().right / cellSize.x);
		int y1 = (int)(obj->getBounds().top / cellSize.y);
		int y2 = (int)(obj->getBounds().bottom / cellSize.y);

		if( x1<0 ) x1=0;
		if( y1<0 ) y1=0;
		for( int i=x1; i<=x2 && i<numCells.x; i++ )
		{
			for( int j=y1; j<=y2 && j<numCells.y; j++ )
			{
				cellMatrix[i][j]->elementArray.push_back( obj );
			}
		}

	}


	bool Map::canPlaceObjectAtLocation( const DPOINT objLocation, const DPOINT objSize )
	{
		GTRACE(5, "> canPlaceObjectAtLocation, location="<<objLocation.x<<","<<objLocation.y<<"; size="<<objSize.x<<","<<objSize.y);
		//	Get the bounds that are affected.

		DRECT rect;
		rect.left = objLocation.x - (objSize.x/2);
		rect.top = objLocation.y - (objSize.y/2);
		rect.right = objLocation.x + (objSize.x/2);
		rect.bottom = objLocation.y + (objSize.y/2);

		GTRACE(5, "rect of location (with borders) (x,y) .. (x,y)");
		GTRACE(5, " "<<rect.left<<","<<rect.top<<","<<rect.right<<","<<rect.bottom);

		int x1 = (int)( rect.left / cellSize.x);
		int x2 = (int)( rect.right/ cellSize.x);
		int y1 = (int)( rect.top / cellSize.y);
		int y2 = (int)( rect.bottom/ cellSize.y);

		GTRACE(5, "rect of cells for object");
		GTRACE(5, " "<<x1<<","<<y1<<","<<x2<<","<<y2);


		//if( x1<bounds.left || x1>bounds.right || y1<bounds.top || y1>bounds.bottom )//WAS WRONG!!! x1, x1, y1, y1?
		//	return false;
		if( rect.left<bonusDropArea.left || rect.right>bonusDropArea.right || rect.top<bonusDropArea.top || rect.bottom>bonusDropArea.bottom )
			return false;

		GTRACE(5, "checking others...");
		for( int i=x1; i<=x2 && i<numCells.x; i++ )
		{
			for( int j=y1; j<=y2 && j<numCells.y; j++ )
			{
				int len = (int)cellMatrix[i][j]->elementArray.size();
				if( len > 0 )
				{
					for( int k=0; k<len; k++ )
						if(  cellMatrix[i][j]->elementArray[k]->intersectsWithArea(rect) )
						{
							return false;
						}
				}
			}
		}
		return true;
	}


	void Map::clearCollisionMap()
	{
		for( int i=0; i< numCells.x; i++ )
			for( int j=0; j<numCells.y; j++ )
			{
				// remove all elems without deleting them.
				this->cellMatrix[i][j]->elementArray.clear();
			}
	}


	bool Map::getFreeSpotForSize(SPoint<double> objSize, DPOINT *returnVal)
	{
		returnVal->x = 0;
		returnVal->y = 0;

		//	First pass. Find the "most free" positions on the map.

		std::vector<IPOINT> possibleLocations;

		bool wasFound = false;
		for( int i=0; i< numCells.x; i++ )
		{
			for( int j=0; j< numCells.y; j++ )
			{
				if( cellMatrix[i][j]->elementArray.size() <= 0 )
				{
					wasFound = true;
					IPOINT iPoint;
					iPoint.x = i;
					iPoint.y = j;
					possibleLocations.push_back( iPoint );
				}
			}
		}

		if( wasFound )
		{
			wasFound = false;
			bool keepTrying = true;
			while( !wasFound && keepTrying )
			{
				int index = guslib::Random::getPtr()->getValueUpTo((long)possibleLocations.size());
				//	try to see if the object collides with any of the neighbours.
				int x = possibleLocations[index].x;
				int y = possibleLocations[index].y;
				DPOINT loc;
				loc.x = x * cellSize.x + cellSize.x/2;
				loc.y = y * cellSize.y + cellSize.y/2;

				if( canPlaceObjectAtLocation( loc, objSize ) )
				{
					wasFound = true;
					returnVal->x = loc.x;
					returnVal->y = loc.y;
				}
				else
				{
					std::vector<IPOINT>::iterator it = possibleLocations.begin();
					for( int i=0; i<index; i++ ){ it++; }
					possibleLocations.erase( it );
					keepTrying = possibleLocations.size() > 0;
				}
			}
		}

		if( ! wasFound )
		{
			//	need second run-through.
		}

		GTRACE(4, "getFreeSpotForSize returning:" << wasFound);
		return wasFound;
	}



	DFLOAT Map::getHeight()
	{
		return damageArea.bottom - damageArea.top;
	}


	DFLOAT Map::getWidth()
	{
		return damageArea.right - damageArea.left;
	}


	//TODO: optimize!!! if an object is removed and placed again on the same location, it's just wasteful.
#if 0
	void Map::moveObjectToLocation( MapObject * obj )
	{
		DRECT oldBounds = obj->getPreviousBounds();
		DRECT newBounds = obj->getBounds();
		int x1,x2,y1,y2;

		x1 = (int)(oldBounds.left / cellSize.x);
		x2 = (int)(oldBounds.right / cellSize.x);
		y1 = (int)(oldBounds.top / cellSize.y);
		y2 = (int)(oldBounds.bottom / cellSize.y);

		if( x1<0 ) x1=0;
		if( y1<0 ) y1=0;
		for( int i=x1; i<=x2 && i<numCells.x; i++ )
		{
			for( int j=y1; j<=y2 && j<numCells.y; j++ )
			{
				cellMatrix[i][j]->elementArray.remove( obj );
			}
		}

		x1 = (int)(newBounds.left / cellSize.x);
		x2 = (int)(newBounds.right / cellSize.x);
		y1 = (int)(newBounds.top / cellSize.y);
		y2 = (int)(newBounds.bottom / cellSize.y);

		if( x1<0 ) x1=0;
		if( y1<0 ) y1=0;
		for( int i=x1; i<=x2 && i<numCells.x; i++ )
		{
			for( int j=y1; j<=y2 && j<numCells.y; j++ )
			{
				cellMatrix[i][j]->elementArray.push_back( obj );
			}
		}
	}
#endif
	

	void Map::setNumCells( IPOINT nCells )
	{
		this->numCells = nCells;
		this->cellSize.x = (bonusDropArea.right - bonusDropArea.left)/numCells.x;
		this->cellSize.y = (bonusDropArea.bottom - bonusDropArea.top)/numCells.y;
		//this->cellSize.x = getWidth() / nCells.x;
		//this->cellSize.y = getHeight() / nCells.y;
	}

#if 0
	void Map::onObjectMoved( MapObject * obj )
	{
		moveObjectToLocation( obj );
	}
#endif
}