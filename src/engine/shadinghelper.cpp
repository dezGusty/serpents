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
// Own header.
//
#include "engine/shadinghelper.h"

//
// C++ system headers.
//

#include <map>
#include <string>

//
// Other libraries' headers
//
#include "OgreRTShaderSystem.h"
#include "OgreShadowCameraSetupPSSM.h"

#include "guslib/trace/trace.h"

namespace Serpents
{
  //
  //  --------------------------------------- Shading Helper Internals ------------------------------------------------
  //

  class ShadingHelper::Impl
  {
  public:
    std::map<Ogre::ShadowTechnique, std::string> techniqueNames_;
    std::map<int, Ogre::ShadowTechnique> techniqueIds_;

    ShadingHelper::Impl()
    {
      techniqueNames_.insert(std::make_pair(Ogre::SHADOWTYPE_NONE, "NONE"));
      techniqueNames_.insert(std::make_pair(Ogre::SHADOWTYPE_STENCIL_MODULATIVE, "STENCIL_MODULATIVE"));
      techniqueNames_.insert(std::make_pair(Ogre::SHADOWTYPE_STENCIL_ADDITIVE, "STENCIL_ADDITIVE"));
      techniqueNames_.insert(std::make_pair(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE, "TEXTURE_ADDITIVE"));
      techniqueNames_.insert(std::make_pair(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE, "TEXTURE_MODULATIVE"));
      techniqueNames_.insert(std::make_pair(
        Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED,
        "TEXTURE_ADDITIVE_INTEGRATED"));
      techniqueNames_.insert(std::make_pair(
        Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED,
        "TEXTURE_MODULATIVE_INTEGRATED"));

      techniqueIds_.insert(std::make_pair(0, Ogre::SHADOWTYPE_NONE));
      techniqueIds_.insert(std::make_pair(1, Ogre::SHADOWTYPE_STENCIL_MODULATIVE));
      techniqueIds_.insert(std::make_pair(2, Ogre::SHADOWTYPE_STENCIL_ADDITIVE));
      techniqueIds_.insert(std::make_pair(3, Ogre::SHADOWTYPE_TEXTURE_ADDITIVE));
      techniqueIds_.insert(std::make_pair(4, Ogre::SHADOWTYPE_TEXTURE_MODULATIVE));
      techniqueIds_.insert(std::make_pair(5, Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED));
      techniqueIds_.insert(std::make_pair(6, Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED));
    }
  };

  //
  //  ---------------------------------------------- Shading Helper ---------------------------------------------------
  //

  /**
    Constructor.
  */
  ShadingHelper::ShadingHelper()
    : shaderGeneratorPtr_(NULL),
      usedSceneManager_(NULL),
      impl_(new ShadingHelper::Impl())
  {
  }

  /**
    Destructor.
  */
  ShadingHelper::~ShadingHelper()
  {
    delete impl_;
  }

  /**
    Getter for the shader generator to use.
  */
  Ogre::RTShader::ShaderGenerator* ShadingHelper::getShaderGenerator()
  {
    return shaderGeneratorPtr_;
  }

  /**
    Setter for the shader generator to use.
  */
  void ShadingHelper::setShaderGenerator(Ogre::RTShader::ShaderGenerator* shaderGenerator)
  {
    shaderGeneratorPtr_ = shaderGenerator;
  };

  bool ShadingHelper::initializeRTShaderSystem(Ogre::SceneManager* sceneMgr)
  {
    ShadingSettings dummy;
    dummy.preferredShadingLanguage = "cg";
    dummy.storeRtssCacheInMemory = true;

    return initializeRTShaderSystem(sceneMgr, dummy);
  }


  bool ShadingHelper::initializeRTShaderSystem(
      Ogre::SceneManager* sceneMgr,
      const ShadingSettings& startupSettings)
  {
    startupSettingsCopy_ = startupSettings;

    if (Ogre::RTShader::ShaderGenerator::initialize())
    {
      shaderGeneratorPtr_ = Ogre::RTShader::ShaderGenerator::getSingletonPtr();

      shaderGeneratorPtr_->addSceneManager(sceneMgr);
      shaderGeneratorPtr_->setTargetLanguage(startupSettings.preferredShadingLanguage);
      GTRACE(2, "stored rtss singleton and added the scene manager to it");
      // Invalidate the scheme in order to re-generate all shaders based technique related to this scheme.
      shaderGeneratorPtr_->invalidateScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
    }
    else
    {
      GTRACE(2, "ERROR initializing the RTSS!!!");
    }

    // Setup core libraries and shader cache path.
    Ogre::StringVector groupVector = Ogre::ResourceGroupManager::getSingleton().getResourceGroups();
    Ogre::StringVector::iterator itGroup = groupVector.begin();
    Ogre::StringVector::iterator itGroupEnd = groupVector.end();
    Ogre::String shaderCoreLibsPath;
    Ogre::String shaderCachePath;
    bool rtssUsesMemoryCache = startupSettings.storeRtssCacheInMemory;
    bool coreLibsFound = false;
    for (; itGroup != itGroupEnd; ++itGroup)
    {
      Ogre::ResourceGroupManager::LocationList resLocationsList =
          Ogre::ResourceGroupManager::getSingleton().getResourceLocationList(*itGroup);

      Ogre::ResourceGroupManager::LocationList::iterator it = resLocationsList.begin();
      Ogre::ResourceGroupManager::LocationList::iterator itEnd = resLocationsList.end();

      // Try to find the location of the core shader lib functions and use it
      // as shader cache path as well - this will reduce the number of generated files
      // when running from different directories.
      for (; it != itEnd; ++it)
      {
        GTRACE(4, "Looking into: [" <<(*it)->archive->getName() << "] for [" << "RTShaderLib" << "]");
        if ((*it)->archive->getName().find("RTShaderLib") != Ogre::String::npos)
        {
          shaderCoreLibsPath =(*it)->archive->getName() + "/cache/";
          shaderCachePath = shaderCoreLibsPath;
          coreLibsFound = true;
          break;
        }
      }

      // Core libs path found in the current group.
      if (coreLibsFound)
      {
        break;
      }
    }

    if (rtssUsesMemoryCache)
    {
      // empty path => generate directly from memory.
      shaderGeneratorPtr_->setShaderCachePath(Ogre::StringUtil::BLANK);
    }
    else
    {
      shaderGeneratorPtr_->setShaderCachePath(shaderCachePath);
    }

    GTRACE(3, "The entry [RTShaderLib] was " <<(coreLibsFound ? "" : "NOT ") << "found in the resources list");

    // Core shader libs not found -> shader generating will fail.
    if (shaderCoreLibsPath.empty())
    {
      return false;
    }

    usedSceneManager_ = sceneMgr;

    // TODO(Augustin Preda, 2014.06.28): Create and register the material manager listener.
#if 0
    mMaterialMgrListener = new ShaderGeneratorTechniqueResolverListener(shaderGeneratorPtr_);
    Ogre::MaterialManager::getSingleton().addListener(mMaterialMgrListener);
#endif

    return true;
  }

  /**
    Perform a cleanup of the RTSS group resources.
  */
  void ShadingHelper::cleanupRTSGResources()
  {
    GTRACE(2, "TODO:(APreda) you are leaving the RTSS files and scripts, but you are deleting other resources!!!");
    GTRACE(2, "Other resources which are used by the RTSS, such as level shaders, shadow casters, textures, etc.");

    shaderGeneratorPtr_->removeSceneManager(usedSceneManager_);
    shaderGeneratorPtr_->flushShaderCache();
    Ogre::RTShader::ShaderGenerator::destroy();
    this->setShaderGenerator(0);
    {
    //  enginePtr_->getShaderGenerator()->finalize();
    //  mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();

    //  mShaderGenerator->addSceneManager(sceneMgr);
    //  mShaderGenerator->setTargetLanguage(startupSettings.preferredShadingLanguage);
    //  GTRACE(2, "stored rtss singleton and added the scene manager to it");
    //  // Invalidate the scheme in order to re-generate all shaders based technique related to this scheme.
    //  mShaderGenerator->invalidateScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
    }
  }


  /**
    Reload the resources part of the RTSS group.
    This may be a simple cleanup and re-initialization.
  */
  void ShadingHelper::reloadRTSGResources()
  {
    bool bResult = this->initializeRTShaderSystem(usedSceneManager_, startupSettingsCopy_);

    if (false == bResult)
    {
      GTRACE(2, "Failed to initialize the RTSS");
    }
  }

  /**
    Getter for the default schema name.
  */
  std::string ShadingHelper::getDefaultSchemaName()
  {
    return std::string(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME.c_str());
  }

  /**
    Invalidate the active RTSS scheme. Will most likely be the same as the default scheme.
  */
  void ShadingHelper::invalidateActiveScheme()
  {
    shaderGeneratorPtr_->invalidateScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
  }

  /**
    Grab the active scheme render state and apply a SSLM_PerVertexLighting lighting model.
  */
  void ShadingHelper::addPerVertexLightingToActiveScheme()
  {
    // Grab the scheme render state.
    Ogre::RTShader::RenderState* schemRenderState = shaderGeneratorPtr_->getRenderState(
        Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

    // Assume a lighting model of SSLM_PerVertexLighting
    Ogre::RTShader::SubRenderState* perPerVertexLightModel =
        shaderGeneratorPtr_->createSubRenderState(Ogre::RTShader::FFPLighting::Type);

    schemRenderState->addTemplateSubRenderState(perPerVertexLightModel);
  }


  /**
    Add PSSM specific render state for RTSS.
  */
  void ShadingHelper::addIntegratedPSSMRenderState(Ogre::PSSMShadowCameraSetup* cameraSetup)
  {
    // Grab the scheme render state.
    Ogre::RTShader::RenderState* schemRenderState = shaderGeneratorPtr_->getRenderState(
        Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

    Ogre::RTShader::SubRenderState* subRenderState =
                  shaderGeneratorPtr_->createSubRenderState(Ogre::RTShader::IntegratedPSSM3::Type);

    Ogre::RTShader::IntegratedPSSM3* pssm3SubRenderState =
        static_cast<Ogre::RTShader::IntegratedPSSM3*>(subRenderState);

    if (cameraSetup)
    {
      const Ogre::PSSMShadowCameraSetup::SplitPointList& srcSplitPoints = cameraSetup->getSplitPoints();
      Ogre::RTShader::IntegratedPSSM3::SplitPointList dstSplitPoints;

      for (unsigned int i = 0; i < srcSplitPoints.size(); ++i)
      {
        dstSplitPoints.push_back(srcSplitPoints[i]);
      }

      pssm3SubRenderState->setSplitPoints(dstSplitPoints);
      schemRenderState->addTemplateSubRenderState(subRenderState);
    }
  }

  /**
    Remove all lighting information from the active
  */
  void ShadingHelper::removeActiveLightingSettings()
  {
    // Grab the scheme render state.
    Ogre::RTShader::RenderState* schemRenderState = shaderGeneratorPtr_->getRenderState(
        Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

    Ogre::RTShader::SubRenderStateList myList = schemRenderState->getTemplateSubRenderStateList();
    for (Ogre::RTShader::SubRenderStateList::iterator it = myList.begin(); it != myList.end(); ++it)
    {
      schemRenderState->removeTemplateSubRenderState(*it);
    }
  }

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
  void ShadingHelper::setShadowTechnique(int shadowTechniqueId)
  {
    Ogre::ShadowTechnique tech = Ogre::SHADOWTYPE_NONE;
    std::map<int, Ogre::ShadowTechnique>::iterator itTech = impl_->techniqueIds_.find(shadowTechniqueId);
    if (itTech != impl_->techniqueIds_.end())
    {
      tech = itTech->second;
    }

    usedSceneManager_->setShadowTechnique(tech);
    GTRACE(4, "[shading] Set shadow tec to id: " << shadowTechniqueId);
  }

  /**
    Get the name of a used shadow techniqueId.
    Note: this is meant to be used mostly for debug info.
  */
  std::string ShadingHelper::getShadowTechniqueName(int shadowTechniqueId) const
  {
    Ogre::ShadowTechnique tech = Ogre::SHADOWTYPE_NONE;
    std::map<int, Ogre::ShadowTechnique>::iterator itTech = impl_->techniqueIds_.find(shadowTechniqueId);
    if (itTech != impl_->techniqueIds_.end())
    {
      tech = itTech->second;
    }

    // Get the name from the technique ID.
    std::map<Ogre::ShadowTechnique, std::string>::iterator it = impl_->techniqueNames_.find(tech);
    if (it != impl_->techniqueNames_.end())
    {
      return it->second;
    }

    return std::string();
  }
}
