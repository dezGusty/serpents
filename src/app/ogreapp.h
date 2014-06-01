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
#include <vector>

//
// Other libraries' headers
//
#include "guslib/system/thread.h"
#include "guslib/util/configuration.h"

//
// This project's headers
//

#include "app/baseappinterface.h"

// GUI
#include "app/serpentsgui.h"

// TODO(Augustin Preda): move to a proper location.
#include "impl/SerpMaterialListener.h"

// common definitions.

namespace app
{
  // Fwd. class decls.
  class SerpFrameListener;
  class SerpEngine;

  /**
    An implementation of the application using OGRE specific functionality.
  */
  class OgreApp
    : public BaseAppInterface
  {
  protected:
    Ogre::Root* ogreRootPtr_;
    Ogre::RenderWindow* renderWindowPtr_;
    Ogre::SceneManager* sceneManagerPtr;
    Ogre::RenderSystem* renderSysPtr_;
    SerpEngine* enginePtr_;
    SerpFrameListener* frameListenerPtr_;
    SerpGUI* guiPtr_;

    MaterialListener *materialListenerPtr_;

    guslib::Configuration config_;

    bool exitRequired_;
    bool bInitReady_;

    bool canStartLogicThread_;
    bool threadInputFinished_;
    bool threadInputStarted_;
    bool threadLogicFinished_;
    bool threadLogicStarted_;
    bool threadNetworkFinished_;
    bool threadNetworkStarted_;
    bool threadRenderFinished_;
    bool threadRenderStarted_;

    bool useLogicThread_;
    bool useInputThread_;

    // Define a mutex for accessing the state
    GUS_MUTEX(threadStateMutex_);

    /**
      Load a plugin identified by its relative path
    */
    bool LoadPlugin(const std::string& pluginName);

    /**
      Load a plugin identified by its relative path in a given set of possible directories.
    */
    bool LoadPluginFromLocations(const std::string& pluginName, std::vector <std::string>& pluginDirectoryList);

    /**
      Load expected plugins list. (the list is read from configuration).
    */
    void LoadPluginsOrDie();

    /**
      Prepare the initial resource locations to use for the application.
      The resource locations may be altered later on.
    */
    void PrepareInitialResourceLocations();

    /**
      Set the rendering engine automatically, based on the configuration loaded thus far and
      the success of loading various plugins.
      @return True of a rendering engine was set, false otherwise.
    */
    bool AutoSetRenderingEngine();

  public:
    OgreApp();

    virtual ~OgreApp();

    virtual void cleanup();

    virtual void run();

    // Initialize the generic components (logging)
    virtual bool preInit();

    // Initialize the graphical engine
    virtual bool initialize();
    virtual bool initGUI();
    virtual bool initSound();

    virtual bool isExitRequired();
    virtual bool isInitializationReady();
    virtual bool isThreadInputFinished();
    virtual bool isThreadInputStarted();
    virtual bool isThreadLogicFinished();
    virtual bool isThreadLogicStarted();
    virtual bool isThreadNetworkFinished();

    virtual void setExitRequired(bool value, const std::string & description);
    virtual void setInitializationReady(bool value = true);
    virtual void setThreadRenderFinished(bool value = true);
    virtual void setThreadInputFinished(bool value = true);
    virtual void setThreadInputStarted(bool value = true);
    virtual void setThreadNetworkFinished(bool value = true);
    virtual void setThreadLogicFinished(bool value = true);
    virtual void setThreadLogicStarted(bool value = true);


    virtual void threadRenderMain();
    virtual void threadLogicHandlingMain();
    virtual void threadInputMain();
    virtual void threadNetworkingMain();

    virtual void waitForSecondaryThreadsToFinish();

    // Ogre related items

    Ogre::RenderWindow *getRenderWindow()
    {
      return renderWindowPtr_;
    }
  };
}
