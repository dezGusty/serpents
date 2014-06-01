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

//
// Includes
//

//
// Own header.
//
#include "engine/guscamera.h"

namespace Serpents
{
  GusCamera::GusCamera(Ogre::Camera * aCamera)
    : camera(aCamera),
      zoomLevel(0),  //  0 = no zoom; 1 = max zoom
      zoomStep(0.1),
      zoomMax(1.0)
  {
    originalOrientation = camera->getOrientation();
    originalPos = camera->getPosition();
  }


  GusCamera::~GusCamera(void)
  {
  }


  void GusCamera::lookAt(const Ogre::Vector3 & lookTarget)
  {
    if (NULL == camera)
    {
      return;
    }

    Ogre::Vector3 actualVector = lookTarget;
    if (zoomLevel >0 && zoomLevel <1)
    {
      actualVector *= (1-zoomLevel);
    }

    camera->lookAt(actualVector);
  }

  void GusCamera::setZoomLevel(const Ogre::Real & zoom)
  {
    if (zoom >= 0 && zoom <= 1)
    {
      this->zoomLevel = zoom;
    }

    if (zoomLevel > zoomMax)
    {
      zoomLevel = zoomMax;
    }
  }


  void GusCamera::setZoomMax(const Ogre::Real & zoom)
  {
    if (zoom >= 0 && zoom <= 1)
    {
      this->zoomMax = zoom;
    }

    setZoomLevel(getZoomLevel());
  }


  void GusCamera::slideToLocation(Ogre::Vector3 newPosition)
  {
    Ogre::Vector3 camPos = this->originalPos;
    Ogre::Vector3 moveVector = newPosition;
    moveVector.y = 0;
    if (zoomLevel > 0 && zoomLevel < 1)
    {
      camPos *= 1 - zoomLevel;
    }

    Ogre::Vector3 newPos = camPos + moveVector;
    this->camera->setOrientation(originalOrientation);
    this->camera->setPosition(newPos);
  }


  void GusCamera::setFOV(const Ogre::Real & fov)
  {
    if (this->camera != NULL)
    {
      this->camera->setFOVy(Ogre::Degree(fov));
    }
  }
}
