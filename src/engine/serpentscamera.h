#pragma once

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
//   Camera handling utility.
//
//    Last change:  $LastChangedDate: 2014-07-04 08:56:45 +0300 (V, 04 iul. 2014) $
//    Revision:    $Revision: 663 $

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
  class Camera;
  class Viewport;
};


namespace Serpents
{
  /**
    Helper class, containing an Ogre camera.
    TODO(Augustin Preda, 2014.07.01): improve the camera movement. Currently it is rather ... "blocky". 
    Use quaternion nlerp/slerp and keep a slower camera movement, so that it's not instant.
  */
  class SerpentsCamera
  {
  private:
    Ogre::Camera* camera_;        //  the Ogre camera to do the actual rendering.
    Ogre::Vector3 targetPos_;
    Ogre::Quaternion originalOrientation;  // Original orientation
    Ogre::Vector3 originalPos;    //  original location
    Ogre::Real zoomLevel;      //  zoom level to use. 0 = no zoom; 1 = max zoom
    Ogre::Real zoomStep;            // steps between zooms; (units travelled in 1 sec.)
    Ogre::Real zoomMax;
    
    class Impl;
    SerpentsCamera::Impl* impl_;

  public:
    /**
      Constructor.
      Requires an Ogre camera to operate.
    */
    explicit SerpentsCamera(Ogre::Camera* camera);

    /**
      Destructor.
    */
    virtual ~SerpentsCamera(void);

    /**
      Getter for the camera.
    */
    virtual Ogre::Camera* getCamera() const;

    /**
      Setter for the target point of the camera.
    */
    virtual void setTargetPos(const Ogre::Vector3& newTarget);

    /**
      Getter for the target point of the camera.
    */
    virtual Ogre::Vector3 getTargetPos() const;
    
    /**
      Utility function to allow easier sliding, keeping the same orientation.
      This will be most likely used in classic 3rd person games, where a constant
      position relative to the character should be kept.
      @param newPosition The position to slide to.
    */
    virtual void slideToLocation(const Ogre::Vector3& newPosition);

    /**
      Similar to the standard camera lookAt, also taking the zoom level into account.
    */
    virtual void lookAt(const Ogre::Vector3& lookTarget);

    /**
      Reset the camera orientation to its initial value
    */
    virtual void resetOrientation();


    /**
      Allow the user to override the default Field Of View.
    */
    void setFOV(const Ogre::Real& fov);

    //
    // ---------------- Zoom functions ---------------------
    //

    /**
      Getter for the zoom step.
    */
    virtual Ogre::Real getZoomStep() const;

    /**
      Setter for the zoom step.
    */
    virtual void setZoomStep(const Ogre::Real& value);

    /**
      Getter for the maximum allowed zoom level.
    */
    Ogre::Real getZoomMax() const;

    /**
      Setter for the maximum allowed zoom level.
    */
    void setZoomMax(const Ogre::Real& zoom);

    /**
      Set the zoom level to use.
    */
    void setZoomLevel(const Ogre::Real& zoom);

    /**
      Get the current zoom level
    */
    Ogre::Real getZoomLevel() const;

    //
    // -------------- Compositor operations --------------------------
    //

    /**
      Add a compositor to the camera's viewport.
      @param compositorName The name of the compositor to use.

      @author Augustin Preda
    */
    void addCompositorByName(const char* compositorName);

    /**
      Remove a compositor from the camera's viewport.
      @param compositorName The name of the compositor to use.

      @author Augustin Preda
    */
    void removeCompositorByName(const char* compositorName);

    /**
      Remove a chain of compositors for the camera's viewport.

      @author Augustin Preda
    */
    void removeCompositorChain();

    /**
      Set the enabled or disabled status of a compositor for the camera's viewport.
      @param compositorName The name of the compositor to use.
      @param status  On/Off flag.

      @author Augustin Preda
    */
    void setCompositorEnabledByName(const char* compositorName, bool status);

    //
    // ------------------------------- Static compositor - viewport functions --------------------
    //

    /**
      Add a compositor to a viewport.
      @param compositorName The name of the compositor to use.
      @param vp The viewport object to add the compositor to.
    */
    static void addCompositorToViewportByName(const char* compositorName, Ogre::Viewport* vp);

    /**
      Remove a compositor from a viewport.
      @param compositorName The name of the compositor to use.
      @param vp The viewport object to remove the compositor from.
    */
    static void removeCompositorFromViewportByName(const char* compositorName, Ogre::Viewport* vp);


    /**
      Remove a chain of compositors for a viewport.
      @param vp The viewport object to remove the compositor from.
    */
    static void removeCompositorChainFromViewport(Ogre::Viewport* vp);

    /**
      Set the enabled or disabled status of a compositor for a viewport.
      @param compositorName The name of the compositor to use.
      @param vp The viewport object to set the compositor status for.
      @param status  On/Off flag.
    */
    static void setCompositorEnabledForViewportByName(const char* compositorName, Ogre::Viewport* vp, bool status);
  };
}
