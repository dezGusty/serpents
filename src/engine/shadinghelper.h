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
//   Helper class for shader based operations.
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

// Forward declaration to avoid inclusion of header.
namespace Ogre
{
  namespace RTShader
  {
    class ShaderGenerator;
  }

  class SceneManager;
  class PSSMShadowCameraSetup;
}

namespace Serpents
{
  /**
    A structure that stores settings to be used for shading control.
  */
  struct ShadingSettings
  {
  public:
    std::string preferredShadingLanguage;
    bool storeRtssCacheInMemory;

    ShadingSettings()
      : preferredShadingLanguage("cg"),
        storeRtssCacheInMemory(false)
    {
    }
  };

  /**
    Shading helper class. Dependent on OGRE.
    Will use the OGRE RTSS component.
  */
  class ShadingHelper
  {
  private:
    Ogre::RTShader::ShaderGenerator* shaderGeneratorPtr_;      // The Shader generator instance.
    ShadingSettings startupSettingsCopy_;
    Ogre::SceneManager* usedSceneManager_;

    class Impl;
    Impl* impl_;

  public:
    /**
      Constructor.
    */
    ShadingHelper();

    /**
      Destructor.
    */
    virtual ~ShadingHelper();

    /**
      Initialize the RTSS component, using the default settings.
    */
    virtual bool initializeRTShaderSystem(Ogre::SceneManager* sceneMgr);

    /**
      Initialize the RTSS component using custom start-up settings.
    */
    virtual bool initializeRTShaderSystem(
        Ogre::SceneManager* sceneMgr,
        const ShadingSettings& startupSettings);

    /**
      Setter for the shader generator to use.
    */
    void setShaderGenerator(Ogre::RTShader::ShaderGenerator* shaderGenerator);

    /**
      Getter for the shader generator to use.
    */
    Ogre::RTShader::ShaderGenerator* getShaderGenerator();

    /**
      Perform a cleanup of the RTSS group resources.
    */
    void cleanupRTSGResources();

    /**
      Reload the resources part of the RTSS group.
      This may be a simple cleanup and re-initialization.
    */
    void reloadRTSGResources();

    /**
      Getter for the default schema name.
    */
    std::string getDefaultSchemaName();

    /**
      Invalidate the active RTSS scheme. Will most likely be the same as the default scheme.
    */
    void invalidateActiveScheme();

    /**
      Grab the active scheme render state and apply a SSLM_PerVertexLighting lighting model.
    */
    void addPerVertexLightingToActiveScheme();

    /**
      Add PSSM specific render state for RTSS.
    */
    void addIntegratedPSSMRenderState(Ogre::PSSMShadowCameraSetup* cameraSetup);

    /**
      Remove all lighting information from the active
    */
    void removeActiveLightingSettings();

    /**
      Apply a shadow technique to use.
      @param shadowTechniqueId The identifier of the shadow technique. Possible values include, but are not limited to:
        0 = SHADOWTYPE_NONE
        1 = SHADOWTYPE_STENCIL_MODULATIVE
        2 = SHADOWTYPE_STENCIL_ADDITIVE
        3 = SHADOWTYPE_TEXTURE_ADDITIVE
        4 = SHADOWTYPE_TEXTURE_MODULATIVE
        5 = SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED
        6 = SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED
      @see See the definition of [enum ShadowTechnique] in [OgreCommon.h].
      Note that the values of the technique ID differ.
    */
    void setShadowTechnique(int shadowTechniqueId);

    /**
      Get the name of a used shadow techniqueId.
      Note: this is meant to be used mostly for debug info.
      @param shadowTechniqueId The identifier of the shadow technique. Possible values include, but are not limited to:
        0 = SHADOWTYPE_NONE
        1 = SHADOWTYPE_STENCIL_MODULATIVE
        2 = SHADOWTYPE_STENCIL_ADDITIVE
        3 = SHADOWTYPE_TEXTURE_ADDITIVE
        4 = SHADOWTYPE_TEXTURE_MODULATIVE
        5 = SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED
        6 = SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED
      @see See the definition of [enum ShadowTechnique] in [OgreCommon.h].
      Note that the values of the technique ID differ.
    */
    std::string getShadowTechniqueName(int shadowTechniqueId) const;
  };
}
