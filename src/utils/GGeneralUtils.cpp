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
//    General utitilies file

//
// Includes
//

//
// Own header
//
#include "utils/ggeneralutils.h"

//
// C++ system files
//
#include <sstream>
#include <string>

namespace GUtils
{

	template <class T> static bool RectanglesCollide( T left1, T right1, T top1, T bottom1, 
											T left2, T right2, T top2, T bottom2 )
	{
		if( right2 < left1 )
			return false;
		if( left2 > right1 )
			return false;
		if( top2 > bottom1 )
			return false;
		if( top1 > bottom2 )
			return false;
		return true;
	}


	bool RectanglesCollideD( double left1, double right1, double top1, double bottom1, 
											double left2, double right2, double top2, double bottom2 )
	{
		if( right2 < left1 )
			return false;
		if( left2 > right1 )
			return false;
		if( top2 > bottom1 )
			return false;
		if( top1 > bottom2 )
			return false;
		return true;
	}


	double GetSegmentLength( double x1, double y1, double x2, double y2 )
	{
		double ret = 0;
		double width = abs( x2 - x1 );
		double height = abs( y2 - y1 );

		ret = sqrt( width * width + height * height );
		return ret;
	}



	//
	//	=========================================== GusUtils =========================================
	//

	GusUtils::GusUtils(void)
	{
	}


	GusUtils::~GusUtils(void)
	{
	}

	template <class T> static bool GusUtils::RectanglesCollide( T left1, T right1, T top1, T bottom1, 
											T left2, T right2, T top2, T bottom2 )
	{
		if( right2 < left1 )
			return false;
		if( left2 > right1 )
			return false;
		if( top2 > bottom1 )
			return false;
		if( top1 > bottom2 )
			return false;
		return true;
	}


	double GusUtils::getAngleInDegreesFromRadians( double angleInRadians )
	{
		return angleInRadians * 180;
	}

	double GusUtils::getAngleInDegreesFromRadiansSq( double angleInRadians )
	{
		return angleInRadians * 180 / PI();
	}


	double GusUtils::PI()
	{
#ifdef M_PI
		return M_PI;
#else
		return 3.14159265358979323846;
#endif
	}


	double GusUtils::Cos( double angle )
	{
		return cos( angle );
	}

	double GusUtils::Sin( double angle )
	{
		return sin( angle );
	}

	double GusUtils::Acos( double angle )
	{
		return acos( angle );
	}

	double GusUtils::Asin( double angle )
	{
		return asin( angle );
	}
		
	double GusUtils::Tan( double angle )
	{
		return tan( angle );
	}

	double GusUtils::Atan( double angle )
	{
		return atan( angle );
	}

	double GusUtils::Pow(double num, double pw)
	{
		return pow(num, pw);
	}


	long GusUtils::charBasisIntArrayToNumber( int * theArray, int len )
	{
		long ret=0;
		for( int i=0; i<len; i++ )
		{
			ret += theArray[i] * (long)Pow(256,i);
		}
		return ret;
	}
		
	long GusUtils::charBasisCharArrayToNumber( const char * theArray, int len )
	{
		long ret=0;
		for( int i=0; i<len; i++ )
		{
			ret += theArray[i] * (long)Pow(256,i);
		}
		return ret;
	}

	

	std::string GusUtils::rectToString( DRECT myRect )
	{
		std::stringstream ss;
		ss<<"("<< myRect.left<<","<< myRect.top<<"," <<myRect.right<<"," <<myRect.bottom <<")";
		return ss.str();
	}

}
