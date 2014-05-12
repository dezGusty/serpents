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

#include <queue>
#include "utils/GGeneralUtils.h"

namespace GBase
{
	typedef enum GBaseEventTypeEnum
	{
		GET_Unknown =0,
		GET_Loading,
		GET_StateChange,
		GET_MenuShouldRedraw,
		GET_Exit,
		GET_Hiscore,
		GET_Reboot,
		GET_Quick,
		GET_NormGame,
		GET_ViewHiscore,
		GET_InGameMenu,
		GET_DualStateBack,
		GET_LengthChange,
		GET_HungerChange,
		GET_HealthChange,
		GET_ScoreChange,
		GET_SpeedChange,
		GET_PosChange,
		GET_NumEvents
	}GBaseEventType;


	/// Basic param class. Could be extended for future use.
	class GEventParam
	{
	protected:
	public:
		GEventParam();
		virtual ~GEventParam();

		virtual const std::string getValue() const { return std::string(""); }
	};

	class GPairFloatEventParam : public GEventParam
	{
	protected:
		double x_;
		double y_;
	public:
		GPairFloatEventParam( double x, double y ): x_(x), y_(y){}
		virtual ~GPairFloatEventParam(){}
		virtual const std::string getValue() const;
	};


	//TODO: doesn't it make sense to make this a template?? FFS, then do it!
	class GNumericalFloatEventParam : public GEventParam
	{
	protected:
		double value_;
		int subdigits; // digits after the comma
	public:
		GNumericalFloatEventParam( double value, int numDigits=2 ){ value_ = value; subdigits=numDigits; }
		virtual ~GNumericalFloatEventParam();
		virtual const std::string getValue() const;
	};


	class GPercentageEventParam : public GEventParam
	{
	protected:
		double value_;
		int subdigits; // digits after the comma
	public:
		GPercentageEventParam( double value, int numDigits=2 ){ value_ = value; subdigits=numDigits; }
		virtual ~GPercentageEventParam();
		virtual const std::string getValue() const;
	};


	///
	/// Single event class.
	///
	class GBaseEvent
	{
	protected:
		GBaseEventType eventType;
		std::string description;
		GEventParam * parameters;
	public:
		GBaseEvent( GBaseEventType evtType, std::string description="", GEventParam * param=NULL );
		virtual ~GBaseEvent();
		const std::string& getDescription() const { return description; }
		GEventParam * getParams(){ return parameters; }
		GBaseEventType getType(){ return eventType; }
	};
	typedef std::queue<GBaseEvent*> GBaseEventQueue;

	///
	/// Simple event class.
	///
	class GSimpleEvent
	{
	protected:
		int eventType;
		double parameter;
		std::string description;
	public:
		GSimpleEvent( int evtType, std::string desc="", double param=0.0);
		virtual ~GSimpleEvent(){}
		const std::string& getDescription() const { return description; }
		double getParam(){ return parameter; }
		int getType(){ return eventType; }
	};

	typedef std::queue<GSimpleEvent*> GSimpleEventQueue;
}