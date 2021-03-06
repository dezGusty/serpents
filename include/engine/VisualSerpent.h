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
//    Implementation of the base serpent.

#include "SerpentBase.h"
#include "GBaseEngine.h"
#include "VisualBodyPart.h"
#include "GBaseSetting.h"
#include <Ogre.h>

namespace Serpents
{
	class VisualSerpent :
		public SerpentBase
	{
	protected:
		Ogre::SceneManager * sceneManager;
		GBase::GBaseEngine * engine;

		virtual VisualBodyPart * createHeadBodyPart();
		virtual VisualBodyPart * createBodyBodyPart();

		GBase::VisibleEntityConfig srphead;
		GBase::VisibleEntityConfig srpbody;
	public:
		VisualSerpent( Ogre::SceneManager * sceneManager, GBase::GBaseEngine * someEngine );
		virtual ~VisualSerpent();
		virtual BodyPartBase* createBodyPart(int partType=0);
		virtual void setHeadConfig( GBase::VisibleEntityConfig cfg );
		virtual void setBodyConfig( GBase::VisibleEntityConfig cfg );
	};
}