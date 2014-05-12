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
//    Implementation of the base bonus.

#include "Bonus.h"
#include <Ogre.h>
#include "GOgreRenderable.h"

#include "GBaseEngine.h"

namespace Serpents
{

	class VisualBonusPrototype:
		public BonusPrototypeBase,
		public GOgreRenderable
	{
	public:
		VisualBonusPrototype( Ogre::SceneManager * sceneManager, std::string nameToUse );
		virtual ~VisualBonusPrototype();
		virtual std::string getType() const { return std::string("visual"); }
	};


	class VisualBonus:
		public BonusBase,
		public GOgreRenderable
	{
	public:
		VisualBonus( Ogre::SceneManager * sceneManager, const BonusPrototypeBase * prototype, long time = 30000 );
		virtual ~VisualBonus();

		/// overrride for location setting.
		virtual void setLocation( GUtils::DFLOAT x, GUtils::DFLOAT y );
	};


	class VisualBonusManager:
		public BonusManagerBase
	{
		Ogre::SceneManager * sceneManager;
		GBase::GBaseEngine *engine;
	public:
		VisualBonusManager( Ogre::SceneManager * sceneManager, GBase::GBaseEngine *someEngine );

		virtual ~VisualBonusManager();

		///
		///	Create a bonus using a given prototype. Please override this function to handle a different type of bonus.
		///	@override
		///
		virtual BonusBase * createBonusFromPrototype( const BonusPrototypeBase * prototype );

		///
		///	Create a bonus prototype using the name provided. Please override this function in child classes to handle 
		///	different types of prototypes.
		///	@override
		///
		virtual BonusPrototypeBase * createPrototype( const std::string & prototypeName );


		/// React to a certain amount of time having passed.
		/// @param timeUnits Game time units used (most likely milliseconds)
		virtual void onTimeGoesBy( double timeUnits );

	};


}