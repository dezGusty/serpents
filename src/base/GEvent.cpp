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

#include <base/GEvent.h>
#include <guslib/trace/trace.h>

namespace GBase
{
	GEventParam::GEventParam()
	{
	}


	GEventParam::~GEventParam()
	{
	}



	GBaseEvent::GBaseEvent(GBaseEventType evtType, std::string descr, GEventParam * param)
		: eventType( evtType )
		, description( descr )
		, parameters( param )
	{
	}



	GBaseEvent::~GBaseEvent()
	{
		if( parameters != NULL )
			delete parameters;
	}


	GNumericalFloatEventParam::~GNumericalFloatEventParam()
	{
	}

	const std::string GNumericalFloatEventParam::getValue() const
	{ 
		double myVal = value_;
		if( subdigits > 0 )
		{
			myVal *= GUtils::GusUtils::Pow(10, subdigits); // 0.98332 with 2 digits becomes 9833.2
		}
		long truncated = (long)myVal; // 0.98332 with 2 digits becomes 9833.2
		myVal = truncated;
		if( subdigits > 0 )
		{
			myVal = myVal / GUtils::GusUtils::Pow(10, subdigits);
		}
		std::stringstream ss;
		ss<<myVal;
		return ss.str();
	}

	const std::string GPairFloatEventParam::getValue() const
	{ 
		std::stringstream ss;
		ss<<x_<<","<<y_;
		return ss.str();
	}

	GPercentageEventParam::~GPercentageEventParam()
	{
	}

	const std::string GPercentageEventParam::getValue() const
	{
		double myVal = value_ * 100; // 1.0 becomes 100.0 ; 0.98332 becomes 98.332
		if( subdigits > 0 )
		{
			myVal *= GUtils::GusUtils::Pow(10, subdigits); // 0.98332 with 2 digits becomes 9833.2
		}
		long truncated = (long)myVal; // 0.98332 with 2 digits becomes 9833.2
		myVal = truncated;
		if( subdigits > 0 )
		{
			myVal = myVal / GUtils::GusUtils::Pow(10, subdigits);
		}
		std::stringstream ss;
		ss<<myVal;
		return ss.str();
	}


	GSimpleEvent::GSimpleEvent( int evtType, std::string desc, double param)
		: eventType(evtType)
		, description(desc)
		, parameter(param)
	{
	}
}