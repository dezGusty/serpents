//   This file is part of Gusty's Serpents, licensed under the terms of the MIT License
//   (further described below).
//
//   Copyright (C) 2009-2014  Augustin Preda (thegusty999@gmail.com)
//
//   Permission is hereby granted, free of charge, to any person obtaining a copy
//   of this software and associated documentation files (the "Software"), to deal
//   in the Software without restriction, including without limitation the rights
//   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//   copies of the Software, and to permit persons to whom the Software is
//   furnished to do so, subject to the following conditions:
//
//   The above copyright notice and this permission notice shall be included in
//   all copies or substantial portions of the Software.
//
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//   THE SOFTWARE.
//
//    Last change:  $LastChangedDate: 2014-07-04 08:56:45 +0300 (V, 04 iul. 2014) $
//    Revision:    $Revision: 663 $

//
// Includes
//

//
// Own header.
//
#include "engine/serpentscamera.h"

//
// C++ system headers
//

// none

//
// Other libraries' headers
//

// OGRE headers
#include "OgreCamera.h"
#include "OgreCompositorManager.h"
#include "OgreViewport.h"

#include "guslib/trace/trace.h"

namespace Serpents
{
  //
  //  ------------------------------------------- Camera Internals ----------------------------------------------------
  //

  /**
    Opaque pointer: private implementation of camera.
  */
  class SerpentsCamera::Impl
  {
  public:
    /**
      Constructor.
    */
    SerpentsCamera::Impl()
    {
    }
  };

  //
  //  ---------------------------------------------- Camera ---------------------------------------------------
  //

  /**
    Constructor.
    Requires an Ogre camera to operate.
  */
  SerpentsCamera::SerpentsCamera(Ogre::Camera * aCamera)
    : camera_(aCamera),
      impl_(new SerpentsCamera::Impl()),
      zoomLevel(0),  //  0 = no zoom; 1 = max zoom
      zoomStep(0.1),
      zoomMax(1.0)
  {
    originalOrientation = camera_->getOrientation();
    originalPos = camera_->getPosition();
  }


  /**
    Destructor.
  */
  SerpentsCamera::~SerpentsCamera(void)
  {
    delete impl_;
  }


  /**
    Similar to the standard camera lookAt, also taking the zoom level into account.
  */
  void SerpentsCamera::lookAt(const Ogre::Vector3 & lookTarget)
  {
    if (NULL == camera_)
    {
      return;
    }

    Ogre::Vector3 actualVector = lookTarget;
    if (zoomLevel >0 && zoomLevel <1)
    {
      actualVector *= (1-zoomLevel);
    }

    camera_->lookAt(actualVector);
  }

  /**
    Reset the camera orientation to its initial value
  */
  void SerpentsCamera::resetOrientation()
  {
    camera_->setOrientation(originalOrientation);
  }


  /**
    Utility function to allow easier sliding, keeping the same orientation.
    This will be most likely used in classic 3rd person games, where a constant
    position relative to the character should be kept.
    @param newPosition The position to slide to.
  */
  void SerpentsCamera::slideToLocation(const Ogre::Vector3& newPosition)
  {
    Ogre::Vector3 camPos = this->originalPos;
    Ogre::Vector3 moveVector = newPosition;
    moveVector.y = 0;
    if (zoomLevel > 0 && zoomLevel < 1)
    {
      camPos *= 1 - zoomLevel;
    }

    Ogre::Vector3 newPos = camPos + moveVector;
    this->camera_->setOrientation(originalOrientation);
    this->camera_->setPosition(newPos);
  }

  /**
    Allow the user to override the default Field Of View.
  */
  void SerpentsCamera::setFOV(const Ogre::Real & fov)
  {
    if (this->camera_ != NULL)
    {
      this->camera_->setFOVy(Ogre::Degree(fov));
    }
  }

  /**
    Getter for the camera.
  */
  Ogre::Camera* SerpentsCamera::getCamera() const
  {
    return camera_;
  }


  /**
    Getter for the target point of the camera.
  */
  Ogre::Vector3 SerpentsCamera::getTargetPos() const
  {
    return targetPos_;
  }

  /**
    Setter for the target point of the camera.
  */
  void SerpentsCamera::setTargetPos(const Ogre::Vector3 &newTarget)
  {
    this->targetPos_ = newTarget;
  }

  //
  // ---------------- Zoom functions ---------------------
  //

  /**
    Getter for the zoom step.
  */
  Ogre::Real SerpentsCamera::getZoomStep() const
  {
    return zoomStep;
  }

  /**
    Setter for the zoom step.
  */
  void SerpentsCamera::setZoomStep(const Ogre::Real &value)
  {
    zoomStep = value;
  }

  /**
    Get the current zoom level
  */
  Ogre::Real SerpentsCamera::getZoomLevel() const
  {
    return this->zoomLevel;
  }

  /**
    Set the zoom level to use.
  */
  void SerpentsCamera::setZoomLevel(const Ogre::Real & zoom)
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

  /**
    Getter for the maximum allowed zoom level.
  */
  Ogre::Real SerpentsCamera::getZoomMax() const
  {
    return zoomMax;
  }

  /**
    Setter for the maximum allowed zoom level.
  */
  void SerpentsCamera::setZoomMax(const Ogre::Real & zoom)
  {
    if (zoom >= 0 && zoom <= 1)
    {
      this->zoomMax = zoom;
    }

    setZoomLevel(getZoomLevel());
  }

  //
  // -------------- Compositor operations --------------------------
  //

  /**
    Add a compositor to the camera's viewport.
    @param compositorName The name of the compositor to use.

    @author Augustin Preda
  */
  void SerpentsCamera::addCompositorByName(const char* compositorName)
  {
    addCompositorToViewportByName(compositorName, getCamera()->getViewport());
  }

  /**
    Remove a compositor from the camera's viewport.
    @param compositorName The name of the compositor to use.

    @author Augustin Preda
  */
  void SerpentsCamera::removeCompositorByName(const char* compositorName)
  {
    removeCompositorFromViewportByName(compositorName, getCamera()->getViewport());
  }

  /**
    Remove a chain of compositors for the camera's viewport.

    @author Augustin Preda
  */
  void SerpentsCamera::removeCompositorChain()
  {
    removeCompositorChainFromViewport(getCamera()->getViewport());
  }

  /**
    Set the enabled or disabled status of a compositor for the camera's viewport.
    @param compositorName The name of the compositor to use.
    @param status  On/Off flag.

    @author Augustin Preda
  */
  void SerpentsCamera::setCompositorEnabledByName(const char* compositorName, bool status)
  {
    setCompositorEnabledForViewportByName(compositorName, getCamera()->getViewport(), status);
  }

  /**
    Add a compositor to a viewport.
    @param compositorName The name of the compositor to use.
    @param vp The viewport object to add the compositor to.
  */
  void SerpentsCamera::addCompositorToViewportByName(const char* compositorName, Ogre::Viewport* vp)
  {
    GTRACE(5, "Adding compositor [" << compositorName << "](or at least trying to)");
    try
    {
      Ogre::CompositorManager::getSingleton().addCompositor(vp, compositorName);
      Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, compositorName, true);
    }
    catch (const std::exception& e)
    {
      GTRACE(1, "Caught exception: " << e.what());
    }
  }

  /**
    Remove a compositor from a viewport.
    @param compositorName The name of the compositor to use.
    @param vp The viewport object to remove the compositor from.
  */
  void SerpentsCamera::removeCompositorFromViewportByName(const char* compositorName, Ogre::Viewport* vp)
  {
    GTRACE(5, "Removing compositor [" << compositorName << "]");
    try
    {
      Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, compositorName, false);
      Ogre::CompositorManager::getSingleton().removeCompositor(vp, compositorName);
    }
    catch (const std::exception& e)
    {
      GTRACE(1, "Caught exception: " << e.what());
    }
  }


    /**
      Remove a chain of compositors for a viewport.
      @param vp The viewport object to remove the compositor from.
    */
    void SerpentsCamera::removeCompositorChainFromViewport(Ogre::Viewport* vp)
    {
      Ogre::CompositorManager::getSingletonPtr()->removeCompositorChain(vp);
    }

    /**
      Set the enabled or disabled status of a compositor for a viewport.
      @param compositorName The name of the compositor to use.
      @param vp The viewport object to set the compositor status for.
      @param status  On/Off flag.
    */
    void SerpentsCamera::setCompositorEnabledForViewportByName(
        const char* compositorName,
        Ogre::Viewport* vp,
        bool status)
    {
      try
      {
        Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, compositorName, status);
      }
      catch (const std::exception& e)
      {
        GTRACE(1, "Caught exception: " << e.what());
      }
    }
}


