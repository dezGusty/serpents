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
//   Scene handling class.
//
//    Last change:  $LastChangedDate$
//    Revision:    $Revision$

//
// Includes
//

//
// C++ system headers.
//

#include <string>

//
// Other libraries' headers
//

#include "guslib/util/configuration.h"

//
// This project's headers
//


// Forward declaration to avoid inclusion of header.
namespace Ogre
{
//   namespace RTShader
//   {
//     class ShaderGenerator;
//   }
//
//   class SceneManager;
//   class PSSMShadowCameraSetup;
  class Light;
}

namespace app
{
  class SerpEngine;
}

namespace Serpents
{
  /**
    Shading helper class. Dependent on OGRE.
    Will use the OGRE RTSS component.
  */
  class Scene
  {
  private:
    class Impl;
    Scene::Impl* impl_;

  public:
    /**
      Constructor.
    */
    explicit Scene(app::SerpEngine* ptrToEngine);

    /**
      Destructor.
    */
    virtual ~Scene();

    /**
      Allow the specification of skybox details.
      This allows the user to turn the skybox on or off, as well as setting the material to use
      (for the 6 facets of the skybox), distance
    */
    void setSkyBoxData(
      bool enabled,
      const char* materialName,
      double distanceToSkyBox,
      bool drawFirst,
      const char* resourceGroupName);

    /**
      Create scene objects from a given configuration.
      @param objectConfiguration A simple configuration object.
    */
    void createObjectFromConfig(guslib::PropertyGroup& groupEntry, const std::string& resourceGroupName);

    /**
      Create a light configuration from a base configuration entry.
      @param groupEntry The configuration for the light, based on a property group.
      @param createVisibleLightInformation Specify to create additional items to help the user see the light source.
      @param resourceGroupName The resource group to use for creating additional items.
    */
    void createLightFromConfig(
        guslib::PropertyGroup& groupEntry,
        bool createVisibleLightInformation,
        const std::string& resourceGroupName);


    /**
      Apply the light settings to a light entry.
      @param lightObject The light object to which settings are applied.
      @param lightCfgEntry The configuration entry.
    */
    void applyLightSettingsFromCfg(Ogre::Light* lightObject, guslib::PropertyGroup& lightCfgEntry);
  };
}
