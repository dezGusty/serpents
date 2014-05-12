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
//    Class to represent paths on a planar surface.

#include "utils/GGeneralUtils.h"
#include "BodyPartBase.h"

namespace Serpents
{
#if 0
	///
	///	Class to define a segment in the path on which a serpent will be placed along.
	///

	class PathStrip
	{
	public:

		/// Constructor (default).
		PathStrip();

		/// Constructor (for a more convenient use).
		PathStrip( double xhead, double yhead, double xtail, double ytail );

		/// Copy constr.
		PathStrip( PathStrip & );

		/// Destructor. No pointers used, to nothing to do.
		~PathStrip(void);

		/// Get the length for the given segment. (Pitagora)
		double getLength();

		///
		///	Get the coordinates of a sub-segment. It is imperative that partialSegmentSize be smaller than
		///	getLength(), otherwise the maximum allowed location will be returned. To retrieve the point on the line, witout being
		///	limited by the length, specify true for allowGreaterSize
		///

		GUtils::DPOINT getLocationFromHead( double partialSegmentSize, bool allowGreaterSize = false );

		GUtils::DPOINT getHeadLocation(){ return headLocation; }
		GUtils::DPOINT getTailLocation(){ return tailLocation; }

	protected:
		GUtils::DPOINT headLocation;
		GUtils::DPOINT tailLocation;

		friend class SerpentPath;
	};

	typedef std::vector<PathStrip*> PathStripList;

	///
	///	Class to define the path on which a serpent will be placed along.
	///

	class SerpentPath
	{
	public:

		SerpentPath();

		~SerpentPath();

		///
		///	Make the calculations for the coordinates for all bodyparts. The result is an array of bodyparts.
		///	This is the place where the unused path strips are eliminated.
		///

		virtual void getAndSetPositionOfAllParts( BodyPartPtrList &parts);

		///	Add a stript to the end of the path. This will be used in general to generate new (unspawned) bodyparts
		virtual void addStrip( int numParts = 1 );

		///
		///	Move the head of the strip list. A pathstrip will be added (the list is automatically extended).
		///	It will be eliminated when the bodyparts are re-placed in their position.
		///

		virtual void moveHeadStrip( double x, double y, double minLength = -1 );

		///	Remove a stript from the end of the path.
		virtual void removeStrip();

		///	Get the coordinates for the entry at a given index.
		virtual GUtils::DPOINT getCoords( int index );

		/// Get a single strip.
		virtual PathStrip* getStrip( int index ){ return pathStrips[index];}

		///	Getter for the length.
		virtual int getLength(){ return (int)pathStrips.size(); }

	protected:
		bool partArrangementWasCompleted;

		PathStripList pathStrips;
		GUtils::DPOINT absoluteTail;
	};
#endif
} // namespace Serpents