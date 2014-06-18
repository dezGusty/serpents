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
// Includes
//

//
// C++ system headers
//
#include <string>

//
// Other libraries' headers
//

#include "guslib/util/configuration.h"
#include "OIS.h"

//
// This project's headers
//

// Include the main configuration file.
#include "config/serpents_config.h"

#include "app/serpentsstate.h"
#include "app/serpentsgui.h"


#define GUS_USE_RTSS 1

#if GUS_USE_RTSS
#include "OgreRTShaderSystem.h"
#endif

// common definitions.

namespace app
{
  // Fwd. class decls.

  class SerpFrameListener;

  //
  // POD structure?
  //
  class SerpEngineStartupSettings
  {
  public:
    std::string preferredShadingLanguage;
    bool storeRtssCacheInMemory;

    SerpEngineStartupSettings()
      : preferredShadingLanguage("cg")
      , storeRtssCacheInMemory(false)
    {
    }
  };


  class SerpEngine
    : public OIS::KeyListener
    , public OIS::MouseListener
    , public SerpStateManagerListener
  {
  protected:
    bool exiting_;
    bool windowIsActive_;
    std::string name_;

    SerpFrameListener * frameListenerPtr_;
    Ogre::Root * rootPtr_;
    Ogre::SceneManager * sceneManagerPtr_;

    Ogre::Camera* cameraPtr_;
    Ogre::Camera* cameraSecPtr_;

    SerpStateManagerUtil stateManager_;
    SerpGUI * guiPtr_;
    guslib::Configuration config_;
    SerpEngineStartupSettings startupSettingsCopy_;
#if GUS_USE_RTSS
  protected:
    Ogre::RTShader::ShaderGenerator*      mShaderGenerator;      // The Shader generator instance.

  public:
    virtual bool initializeRTShaderSystem(Ogre::SceneManager* sceneMgr);
    virtual bool initializeRTShaderSystem(
        Ogre::SceneManager* sceneMgr,
        const SerpEngineStartupSettings& startupSettings);

    void setShaderGenerator(Ogre::RTShader::ShaderGenerator* shaderGenerator)
    {
      mShaderGenerator = shaderGenerator;
    };

    Ogre::RTShader::ShaderGenerator* getShaderGenerator()
    {
      return mShaderGenerator;
    }

    void cleanupRTSGResources();
    void reloadRTSGResources();

#endif  // GUS_USE_RTSS

  public:
    SerpEngine(Ogre::SceneManager * aManager, Ogre::Root * rootElement, const std::string & name);

    virtual ~SerpEngine();

    virtual Ogre::SceneManager * getSceneManagerPtr()
    {
      return sceneManagerPtr_;
    }

    virtual SerpStateManagerUtil& getStateMgr()
    {
      return stateManager_;
    }

    virtual bool isExiting();

    virtual void logicThreadLoop();

    virtual void setConfig(guslib::Configuration& config);

    virtual guslib::Configuration& getConfig()
    {
      return config_;
    }

    void releaseGUI()
    {
      guiPtr_ = 0;
    }

    void setGUIPtr(SerpGUI * ptr)
    {
      guiPtr_ = ptr;
    }

    /**
      This is basically a copy of the standard OGRE root function "renderOneFrame".
      It's added here to make it easier to add a mutex lock for loading and rendering operations.
      The function should be called from the rendering thread!
    */
    virtual bool renderCompleteScene();

    /**
      Scene render function.
      Called (indirectly) from renderCompleteScene (via the frame listener, which is one of the render targets).
    */
    virtual bool renderScene(Ogre::Real timeSinceLastFrame);

    /// The GUI rendering.
    virtual bool renderGUI(Ogre::Real timeSinceLastFrame);

    virtual Ogre::Camera* getMainCamera()
    {
      return cameraPtr_;
    }

    virtual Ogre::Camera* getSecondaryCamera()
    {
      return cameraSecPtr_;
    }

    virtual void setMainCamera(Ogre::Camera* cam);
    virtual void setSecondaryCamera(Ogre::Camera* cam);

    virtual void shutDown();

    virtual SerpFrameListener* getFrameListener()
    {
      return frameListenerPtr_;
    }

    virtual void setFrameListener(SerpFrameListener * ptr);

    /// Release the connection to the frameListener. The frame listener is not owned by the engine,
    /// so it will not be destroyed.
    virtual void releaseFrameListener();

    /// Utility functions to allow other classes to control the frame listener without actually having a pointer to it.
    virtual void setWindowIsActive(bool value);
    virtual bool isWindowActive() const
    {
      return windowIsActive_;
    }


    // OIS::KeyListener overloads
    virtual bool keyPressed(const OIS::KeyEvent &arg);
    virtual bool keyReleased(const OIS::KeyEvent &arg);

    // OIS::MouseListener overloads
    virtual bool mouseMoved(const OIS::MouseEvent &arg);
    virtual bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    virtual bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

    // State manager listener overloads.
    virtual void OnStateStackEmpty();
  };
}

