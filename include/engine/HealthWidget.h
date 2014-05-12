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
//    Utility widget to graphically display the serpent's health.

// Using the std::string
#include <string>
// Using the std::map
#include <map>

#include <guslib/common/smartptr.h>
#include <guslib/system/thread.h>

namespace Serpents
{
	class CustomWidget
	{
	public:
		virtual void elapseTime(double timeUnits)=0;
		virtual double getValue()=0;
		virtual void setValue(double)=0;
		virtual void render()=0;
	};

	class HealthWidget
		: public CustomWidget
	{
	private:
		guslib::GMutex messageAccessMutex;
	protected:
		std::string textOverlayElement_;
		std::string graphicalOverlayElement_;
		double textValue_;

	public:
		HealthWidget( const std::string & textElem, const std::string & graphElem );
		virtual ~HealthWidget();

		virtual void elapseTime(double /*timeUnits*/) { }

		virtual double getValue(){return textValue_;}
		virtual void setValue(double textValue);

		virtual void render();
	};

	typedef GSharedPtr<CustomWidget> CustomWidgetPtr;

	typedef std::map<std::string, CustomWidgetPtr> CustomWidgetMap;
}