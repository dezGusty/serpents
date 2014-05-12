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
//    General utilities.

#include <stdio.h>
// Using the std::string
#include <string>
// Using the std::stringstream
#include <sstream>
// Using the std::vector
#include <vector>

#include "GCommonDef.h"

// defines for windows file locations.
#ifdef _WINDOWS
	#define G_CURRENT_FOLDER 1
	#define G_WINDOWS_FOLDER G_CURRENT_FOLDER<<1
	#define G_WINDOWS_SYS32_FOLDER G_WINDOWS_FOLDER<<1
	#define G_ALL_LOCATIONS 11111111
#endif

///
/// @brief Utilities namespace.
/// The utilities provided are re-usable (or at least they should be) in other projects as well.
/// @details
/// The utilities that are available may depend on additional flags (macro definitions).
/// Some of the utilities require linking additional libraries, such as <b>STL</b> and <b>boost</b>
///
namespace GUtils
{
	typedef double DFLOAT;

	///
	/// Point representation structure.
	///

	template <class T> struct SPoint
	{
		T x;
		T y;
	};

	/// Definitions for the most used point structures.

	typedef SPoint<DFLOAT> DPOINT;

	typedef SPoint<int> IPOINT;

#ifndef SAFE_DELETE
#define SAFE_DELETE(pointer) if(pointer!=NULL){delete pointer;}
#endif


	///
	/// Rectangle representation structure.
	///

	template <class T> struct SRectangle
	{
		T left;
		T right;
		T top;
		T bottom;
	};

	typedef SRectangle<DFLOAT> DRECT;

	/// Function that checks if 2 rectangles collide. The rectangles are given through their 4 points coordinates
	template <class T> static bool RectanglesCollide( T left1, T right1, T top1, T bottom1, 
											T left2, T right2, T top2, T bottom2 );

	/// Get the length of a segment between two points (of double format)
	DFLOAT GetSegmentLength( DFLOAT x1, DFLOAT y1, DFLOAT x2, DFLOAT y2 );

	/// Function that checks if 2 rectangles (of double) collide.
	bool RectanglesCollideD( DFLOAT left1, DFLOAT right1, DFLOAT top1, DFLOAT bottom1, 
											DFLOAT left2, DFLOAT right2, DFLOAT top2, DFLOAT bottom2 );


	///
	///	General utilities class.
	///

	class GusUtils
	{
	protected:
		GusUtils(void);
	public:
		~GusUtils(void);

	public:
		template <class T> static bool RectanglesCollide( T left1, T right1, T top1, T bottom1, 
											T left2, T right2, T top2, T bottom2 );

		///
		///	Transformation from radians to degrees.
		///
		static double getAngleInDegreesFromRadians( double angleInRadians );

		///
		///	Transformation from radians to degrees.
		///
		static double getAngleInDegreesFromRadiansSq( double angleInRadians );

		///
		///	Getter for the value of PI
		///
		static double PI();

		///
		///	Mapping for math functions.
		///
		static double Cos( double angle );

		static double Sin( double angle );

		static double Acos( double angle );

		static double Asin( double angle );
		
		static double Tan( double angle );

		static double Atan( double angle );

		static double Pow( double num, double pw );

		static int * numberToCharBasisIntArray( long number, int *returnedLen );

		static char * numberToCharBasisCharArray( long number, int *returnedLen );

		static long charBasisIntArrayToNumber( int * theArray, int len );
		
		static long charBasisCharArrayToNumber( const char * theArray, int len );

		static bool fileExists( const char * fileName );

		static std::string rectToString( DRECT myRect );
	};

}