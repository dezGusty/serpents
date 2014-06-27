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

//
// Includes
//

//
// C++ system headers
//

// none

//
// Other libraries' headers
//

#include "OgrePrerequisites.h"
#include "OgreVector3.h"

//
// This project's headers.
//


// Forward declaration to avoid inclusion of header.
namespace Ogre
{
  class Root;
  class Camera;
}

namespace Serpents
{
  /// Ogre::Camera container, allowing some additional functionality.
  /// TODO: use quaternion nlerp/slerp and keep a slower camera movement, so that it's not instant... so not blocky.
  class SerpentsCamera
  {
  private:
    Ogre::Camera * camera;        //  the Ogre camera to do the actual rendering.
    Ogre::Vector3 targetPos;
    Ogre::Quaternion originalOrientation;  // Original orientation
    Ogre::Vector3 originalPos;    //  original location
    Ogre::Real zoomLevel;      //  zoom level to use. 0 = no zoom; 1 = max zoom
    Ogre::Real zoomStep;            // steps between zooms; (units travelled in 1 sec.)
    Ogre::Real zoomMax;

  public:
    //  Ctor.
    explicit SerpentsCamera(Ogre::Camera * camera);

    virtual ~SerpentsCamera(void);

    ///  Setter for the target point of the camera.
    virtual void setTargetPos(const Ogre::Vector3 &newTarget)
    {
      this->targetPos = newTarget;
    }

    /// Getter for the target point of the camera.
    virtual Ogre::Vector3 getTargetPos() const
    {
      return targetPos;
    }

    ///  Utility function to allow easier sliding, keeping the same orientation.
    /// This will be most likely used in classic 3rd person games, where a constant
    /// position relative to the character should be kept.
    virtual void slideToLocation(Ogre::Vector3 newPosition);

    ///  Getter for the camera
    virtual Ogre::Camera* getCamera() const
    {
      return camera;
    }

    /// Same as standard camera lookAt, taking the zoom level in addition.
    virtual void lookAt(const Ogre::Vector3& lookTarget);

    ///  Reset the camera orientation to its initial value
    virtual void resetOrientation();

    virtual void setZoomStep(const Ogre::Real &value)
    {
      zoomStep = value;
    }

    virtual Ogre::Real getZoomStep() const
    {
      return zoomStep;
    }

    /// Override the default Field Of View.
    void setFOV(const Ogre::Real &fov);

    Ogre::Real getZoomMax() const
    {
      return zoomMax;
    }

    void setZoomMax(const Ogre::Real & zoom);

    ///  Set the zoom to have
    void setZoomLevel(const Ogre::Real & zoom);

    ///  Getter for the zoom level
    Ogre::Real getZoomLevel() const
    {
      return this->zoomLevel;
    }
  };
}
