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
//    Utility to display messages to the user.

// Using the std::string
#include <string>
#include <map>
#include <guslib/system/thread.h>

namespace Serpents
{
	const double defaultTimeToShowFadingMessage = 3000;
	const std::string defaultOgreTextElementForFadingMessage = "";


	class FadingMessage
	{
	private:
        guslib::GMutex messageAccessMutex;
	protected:
		std::string message;
		std::string ogreTextElement;

		double timeToShowLeft;
		double timeToShowMax;
	public:
		FadingMessage( const std::string & nameOfOgreTextElement, double fadeDuration = defaultTimeToShowFadingMessage );
		FadingMessage( const FadingMessage & cpyCtr );
		FadingMessage();
		virtual ~FadingMessage();

		virtual double getDisplayPercentageLeft();
		virtual double getDisplayLeft();
		virtual double getMaxDisplayDuration(){ return timeToShowMax; }
		virtual const std::string & getMessage() const { return message; }
		virtual void onTimeGoesBy( double timeUnits );
		virtual void setDefaultDisplayDuration( double timeUnits );
		virtual void setMessage( const std::string & message, double timeUnits = -1);
		virtual void setNameOfOgreTextElement( const std::string &name );

		virtual void render();
	};

	typedef std::map<std::string, FadingMessage> FadingMessageMap;
}