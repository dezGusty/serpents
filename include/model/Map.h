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
//    Base class map collision representation.

#include "MapObject.h"
#include "utils/GGeneralUtils.h"

namespace Serpents
{
	typedef std::vector<MapObject*> MapObjectPtrList;

	///
	///	A logical cell for the map. This will be used mainly for collision detection.
	///	Objects that occupy the same cells are candidates for collision. An object is part of one or more cells,
	///	(of each cell its body floats upon).
	///
	class MapCell
	{
	public:
		MapCell(){}
		~MapCell(){}
		MapObjectPtrList elementArray;
	};

	///
	///	Game area representation class, containing physical boundries, obstacles, etc. 
	///	An important addiotion is the ability to calculate the collisions between different objects on the map.
	///

	class Map
	{
	public:

		///
		///	Constructor. This specifies the size of the map. The top-left corner of the map will be used for (0,0) coordinates.
		///	The number of cells is also specified. If -1 is given to the width, or the height, it will be substituted with the
		///	int value of the width or the height.
		///
		Map( GUtils::DFLOAT width, GUtils::DFLOAT height, const GUtils::IPOINT numCells );

		///
		///	Direct coordinate constructor.
		///
		Map( GUtils::DFLOAT left, GUtils::DFLOAT top, GUtils::DFLOAT right, GUtils::DFLOAT bottom, const GUtils::IPOINT numCells );

		///
		///	Initialization function called from the contructors.
		///
		void createMap( double left, double top, double right, double bottom, const GUtils::IPOINT numCells );

		///
		///	Destructor.
		///
		~Map();

		//
		//	Add an object to the matrix used for collision testing.
		//
		virtual void addObjectToCollisionMap( MapObject * obj );

		//
		//	Utility functon used to determine if an object may be placed at a target point.
		//

		virtual bool canPlaceObjectAtLocation( const GUtils::DPOINT objLocation, const GUtils::DPOINT objSize );

		//
		//	Clear the collision map for rebuild pourposes.
		//

		virtual void clearCollisionMap();

		virtual GUtils::DRECT getBonusDropArea(){ return bonusDropArea; }
		virtual GUtils::DRECT getSafeArea(){ return safeArea; }
		virtual GUtils::DRECT getDamageArea(){ return damageArea; }

		virtual void setBonusDropArea( GUtils::DRECT area ){ bonusDropArea = area; }
		virtual void setSafeArea( GUtils::DRECT area ){ safeArea = area; }
		virtual void setDamageArea( GUtils::DRECT area ){ damageArea = area; }


		///	Getter for the size of a cell;
		virtual GUtils::DPOINT getCellSize(){ return cellSize; }

		/// Getter for multiplier of damage received by players inside the "damage area".
		virtual GUtils::DFLOAT getDamageAreaDamageRatio(){ return damageAreaRatio; }

		///
		///	Getter for a place where an object may be placed without colliding with another object.
		///	@param objsize The size of the object (rectangle) to search for placement.
		///	@param returnVal Pointer to the structure this function will return.
		///	@return true if a place was found, false otherwise.
		///
		virtual bool getFreeSpotForSize( GUtils::DPOINT objSize, GUtils::DPOINT *returnVal );

		///	Getter for the map height.
		virtual GUtils::DFLOAT getHeight();

		///	Getter for the number of cells the map uses. (returned as an x, y structure)
		virtual GUtils::IPOINT getNumCells(){ return numCells; }

		///	Getter for the map width;
		virtual GUtils::DFLOAT getWidth();

		/// Setter for the damage ratio
		virtual void setDamageAreaDamageRatio( GUtils::DFLOAT dmg ){ damageAreaRatio = dmg; }

		///
		///	Setter for the cell numbers and sizes.
		///	@param nCells A dual int structure, describing the number of cells for the width and height.
		///
		virtual void setNumCells( GUtils::IPOINT nCells );

	protected:

		// Rectangles delimiting the game surface.
		GUtils::DRECT bonusDropArea;
		GUtils::DRECT safeArea;
		GUtils::DRECT damageArea;

		// Note: easiest hack currently. the base engine requires some data found in the level config, but
		// the original design does not allow it... so provide it via the map. Though it should be placed
		// somewhere different to allow the presence of cheats for instance.
		GUtils::DFLOAT damageAreaRatio;

		GUtils::DPOINT cellSize;
		GUtils::IPOINT numCells;

		MapCell *** cellMatrix;

	};
}