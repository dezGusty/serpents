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


#include "TopSideMessage.h"
#include <Ogre.h>
#include <OgreTextAreaOverlayElement.h>
#include <OgreOverlayManager.h>

namespace Serpents
{
	FadingMessage::FadingMessage( const std::string & nameOfOgreTextElement
		, double fadeDuration )
		: timeToShowMax( fadeDuration )
		, ogreTextElement( nameOfOgreTextElement )
	{
	}

	FadingMessage::FadingMessage()
		: timeToShowMax( defaultTimeToShowFadingMessage )
		, ogreTextElement( defaultOgreTextElementForFadingMessage )
	{
	}

	FadingMessage::FadingMessage( const FadingMessage & cpyCtr )
	{
		message = cpyCtr.message;
		ogreTextElement = cpyCtr.ogreTextElement;
		timeToShowLeft = cpyCtr.timeToShowLeft;
		timeToShowMax = cpyCtr.timeToShowMax;
	}


	FadingMessage::~FadingMessage()
	{
	}


	double FadingMessage::getDisplayPercentageLeft()
	{
		return getDisplayLeft() / getMaxDisplayDuration();
	}

	double FadingMessage::getDisplayLeft()
	{
		return timeToShowLeft;
	}

	void FadingMessage::onTimeGoesBy( double timeUnits )
	{
		this->timeToShowLeft -= timeUnits;
		if( timeToShowLeft < 0 )
			timeToShowLeft = 0;
	}

	void FadingMessage::setDefaultDisplayDuration( double timeUnits )
	{
		if( timeUnits > 0 )
			this->timeToShowMax = timeUnits;
	}

	void FadingMessage::setMessage( const std::string & aMessage, double timeUnits )
	{
        guslib::GScopedLock myLock(messageAccessMutex);
		if( timeUnits == -1 )// default value
		{
			timeUnits = timeToShowMax;
		}
		timeToShowLeft = timeUnits;
		message = aMessage;
	}

	void FadingMessage::setNameOfOgreTextElement( const std::string &name )
	{
		guslib::GScopedLock myLock(messageAccessMutex);
		ogreTextElement = name;
	}


	void FadingMessage::render()
	{
		if( ogreTextElement == defaultOgreTextElementForFadingMessage  )
		{
			return;
		}
		if( !messageAccessMutex.try_lock() )
		{
			return;
		}

		using namespace Ogre;
		if( getDisplayLeft() > 0 )
		{
			OverlayManager::getSingleton().getOverlayElement(ogreTextElement)->show();
			Ogre::ColourValue activeSelectionColorTop(1.0, 1.0, 0.5, getDisplayPercentageLeft());
			Ogre::ColourValue activeSelectionColorBtm(1.0, 0.0, 0.0, getDisplayPercentageLeft());
			Ogre::OverlayManager::getSingleton().getOverlayElement(ogreTextElement)->setCaption( getMessage() );
			((TextAreaOverlayElement*)OverlayManager::getSingleton().getOverlayElement(ogreTextElement))->
				setColourBottom( activeSelectionColorBtm );
			((TextAreaOverlayElement*)OverlayManager::getSingleton().getOverlayElement(ogreTextElement))->
				setColourTop( activeSelectionColorTop );
		}
		else
		{
			OverlayManager::getSingleton().getOverlayElement(ogreTextElement)->setCaption( "" );
			OverlayManager::getSingleton().getOverlayElement(ogreTextElement)->hide();
		}
		messageAccessMutex.unlock();
	}

	// -------------------------------------------------------
	//TopSideMessage::TopSideMessage()
	//	: FadingMessage()
	//{
	//}

	//TopSideMessage::TopSideMessage( const std::string & nameOfOgreTextElement )
	//	: FadingMessage(nameOfOgreTextElement)
	//{
	//}

}
