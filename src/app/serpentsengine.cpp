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
// Includes
//

//
// Own header
//
#include "app/serpentsengine.h"

//
// C++ system files
//
#include <string>

//
// Other libraries' headers
//

// Tracing
#include "guslib/trace/trace.h"

//
// This project's headers
//

// Forwards the inclusion of ogre.h
#include "config/serpentsogre.h"

#include "app/serpentsframelistener.h"
#include "app/serpentsgui.h"

#include "utils/CEGUIUtils.h"

namespace app
{
  SerpEngine::SerpEngine(Ogre::SceneManager * aManager, Ogre::Root * rootElement, const std::string & name)
    : exiting_(false),
      rootPtr_(rootElement),
      sceneManagerPtr_(aManager),
      frameListenerPtr_(0),
      cameraPtr_(0),
      cameraSecPtr_(0),
      name_(name)
#if GUS_USE_RTSS
      ,
      mShaderGenerator(0)
#endif
  {
    GTRACE(3, "SerpEngine created.");
  }

  SerpEngine::~SerpEngine()
  {
  }

  bool SerpEngine::isExiting()
  {
    return exiting_;
  }

  void SerpEngine::logicThreadLoop()
  {
    stateManager_.handleLogicThreadLoop();
  }

  bool SerpEngine::renderCompleteScene()
  {
    // This is basically a copy of the standard OGRE root function "renderOneFrame".
    // What it does in addition to the aforementioned function is to add a mutex lock for
    // loading and rendering operations.
    // The function should be called from the rendering thread!

    if (NULL == frameListenerPtr_)
    {
      return false;
    }

    guslib::Timer myTimer;
    myTimer.reset();

    GTRACE(5, "Engine: render complete scene() entered.");
    if (isExiting())
    {
      return false;
    }

    // Trigger the FrameListener's "frameStarted" function to render the 3D scene.
    if (false == rootPtr_->_fireFrameStarted())
    {
      GTRACE(1, "GBaseEngine::renderCompleteScene received error from _fireFrameStarted");
      return false;
    }

    if (isExiting())
    {
      return false;
    }

    try
    {
      if (false == rootPtr_->_updateAllRenderTargets())
      {
        GTRACE(1, "GBaseEngine::renderCompleteScene received error from _updateAllRenderTargets");
        return false;
      }
    }
    catch (Ogre::Exception & e)
    {
      GTRACE(1, "Exception caught! GBaseEngine::renderCompleteScene(): " << e.getFullDescription());
    }

    if (isExiting())
    {
      return false;
    }

    // Trigger the FrameListener's "frameEnded" function.
    bool returnValue = rootPtr_->_fireFrameEnded();

    GTRACE(5, "Engine: render complete scene() exiting. Render took: "<< myTimer.elapsed()<< " time units.");
    GTRACE(6, "GBaseEngine::renderCompleteScene() releasing mutex(loadAndRenderMutex)");
    return returnValue;
  }

  /**
    Scene render function.
    Called(indirectly) from renderCompleteScene(via the frame listener, which is one of the render targets).
  */
  bool SerpEngine::renderScene(Ogre::Real timeSinceLastFrame)
  {
    return stateManager_.renderScene();
  }

  /**
    GUI render function.
    Called(indirectly) from renderCompleteScene(via the frame listener, which is one of the render targets).
  */
  bool SerpEngine::renderGUI(Ogre::Real timeSinceLastFrame)
  {
    guiPtr_->renderGUI(timeSinceLastFrame);
    return stateManager_.renderGUI();
  }


  void SerpEngine::setMainCamera(Ogre::Camera* cam)
  {
    cameraPtr_ = cam;
  }

  void SerpEngine::setSecondaryCamera(Ogre::Camera* cam)
  {
    cameraSecPtr_ = cam;
  }

  void SerpEngine::setConfig(guslib::Configuration& config)
  {
    config_= config;
  }

  void SerpEngine::shutDown()
  {
    GTRACE(3, "SerpEngine: received cmd to shutdown");
    stateManager_.setShutdown();
    exiting_ = true;

    // TODO(Augustin Preda, 2014.06.01): consider the possibility of having to remove the resource groups.
    // Note that if you try to remove the groups during rendering, the application may freeze.
    // (this side effect occured randomly while deleting the General group)
  }


  void SerpEngine::setFrameListener(SerpFrameListener * ptr)
  {
    GTRACE(3, "SerpEngine: received frame listener");
    if (frameListenerPtr_)
    {
      // there is a listener already; replacement required
      rootPtr_->removeFrameListener(frameListenerPtr_);
    }

    frameListenerPtr_ = ptr;
    rootPtr_->addFrameListener(frameListenerPtr_);
  }


  void SerpEngine::releaseFrameListener()
  {
    frameListenerPtr_ = NULL;
  }


  void SerpEngine::setWindowIsActive(bool value)
  {
    GTRACE(3, "SerpEngine: Window active state change. Is active now:" <<value);
    windowIsActive_ = value;
    // TODO: start/stop music according to window active status

    //if (windowIsActive_)
    //{
    //  GSOUND_START_MUSIC();
    //}
    //else
    //{
    //  GSOUND_PAUSE_MUSIC();
    //}
  }


  //overloads + interf.-------------------------------

  bool SerpEngine::keyPressed(const OIS::KeyEvent &arg)
  {
    if (isExiting())
    {
      return true;
    }

    if (arg.key == OIS::KC_ESCAPE)
    {
      // TODO: add exit!
      shutDown();
    }

    // key down
    utils::CEGUIHelperUtil::injectKeyDown(arg.key);

    // now character
    utils::CEGUIHelperUtil::injectChar(arg.text);

    GTRACE(5, "keyPressed, " << arg.key << " thread : " << boost::this_thread::get_id());

    // TODO: this limits the possibilities of subclasses a lot; should consider sending the full event.
    stateManager_.reactToKeyPress(arg.key);
    return true;
  }


  bool SerpEngine::keyReleased(const OIS::KeyEvent &arg)
  {
    GTRACE(5, "keyReleased, " << arg.key);
    if (isExiting())
    {
      return true;
    }
    utils::CEGUIHelperUtil::injectKeyUp(arg.key);

    // TODO: this limits the possibilities of subclasses a lot; should consider sending the full event.
    stateManager_.reactToKeyRelease(arg.key);
    return true;
  }


  bool SerpEngine::mouseMoved(const OIS::MouseEvent &arg)
  {
    utils::CEGUIHelperUtil::injectMousePosition(arg.state.X.abs, arg.state.Y.abs);
    utils::CEGUIHelperUtil::injectMouseWheelChange(arg.state.Z.rel / 120.0f);

    // TODO: this limits the possibilities of subclasses a lot; should consider sending the full event.
    stateManager_.reactToMouseMoved(arg.state.X.abs, arg.state.Y.abs);

    return true;
  }


  bool SerpEngine::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
  {
    GTRACE(4, "SerpEngine: pressed mouse button");
    utils::CEGUIHelperUtil::injectMouseButtonDown(id);

    // TODO: this limits the possibilities of subclasses a lot; should consider sending the full event.
    stateManager_.reactToMousePressed(id, e.state.X.abs, e.state.Y.abs);
    return true;
  }


  bool SerpEngine::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
  {
    utils::CEGUIHelperUtil::injectMouseButtonUp(id);
    // TODO: this limits the possibilities of subclasses a lot; should consider sending the full event.
    stateManager_.reactToMouseReleased(id, e.state.X.abs, e.state.Y.abs);
    return true;
  }


  void SerpEngine::OnStateStackEmpty()
  {
    GTRACE(3, "The state stack is empty. Shut-down is imminent.");
    shutDown();
  }

  //
  // --------------------------------------------- GUS_USE_RTSS section -----------------------------------------------
  //
#if GUS_USE_RTSS
  bool SerpEngine::initializeRTShaderSystem(Ogre::SceneManager* sceneMgr)
  {
    SerpEngineStartupSettings dummy;
    dummy.preferredShadingLanguage = "cg";
    dummy.storeRtssCacheInMemory = true;

    return initializeRTShaderSystem(sceneMgr, dummy);
  }

  bool SerpEngine::initializeRTShaderSystem(
      Ogre::SceneManager* sceneMgr,
      const SerpEngineStartupSettings& startupSettings)
  {
    startupSettingsCopy_ = startupSettings;

    if (Ogre::RTShader::ShaderGenerator::initialize())
    {
      mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();

      mShaderGenerator->addSceneManager(sceneMgr);
      mShaderGenerator->setTargetLanguage(startupSettings.preferredShadingLanguage);
      GTRACE(2, "stored rtss singleton and added the scene manager to it");
      // Invalidate the scheme in order to re-generate all shaders based technique related to this scheme.
      mShaderGenerator->invalidateScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
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
      mShaderGenerator->setShaderCachePath(Ogre::StringUtil::BLANK);
    }
    else
    {
      mShaderGenerator->setShaderCachePath(shaderCachePath);
    }

    GTRACE(3, "The entry [RTShaderLib] was " <<(coreLibsFound ? "" : "NOT ") << "found in the resources list");

    // Core shader libs not found -> shader generating will fail.
    if (shaderCoreLibsPath.empty())
    {
      return false;
    }

    // Create and register the material manager listener.
    //mMaterialMgrListener = new ShaderGeneratorTechniqueResolverListener(mShaderGenerator);
    //Ogre::MaterialManager::getSingleton().addListener(mMaterialMgrListener);
    return true;
  }

  void SerpEngine::cleanupRTSGResources()
  {
    GTRACE(2, "ERROR:(GUS TODO MSG) you are leaving the RTSS files and scripts, but you are deleting other resources!!!");
    GTRACE(2, "Other resources which are used by the RTSS, such as level shaders, shadow casters, textures, etc.");

    mShaderGenerator->removeSceneManager(this->getSceneManagerPtr());
    mShaderGenerator->flushShaderCache();
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

  void SerpEngine::reloadRTSGResources()
  {
    bool bResult = this->initializeRTShaderSystem(this->getSceneManagerPtr(), startupSettingsCopy_);

    if (false == bResult)
    {
      GTRACE(2, "Failed to initialize the RTSS");
    }
  }

#endif  // GUS_USE_RTSS
}
