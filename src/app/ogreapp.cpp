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
#include "app/ogreapp.h"

//
// C++ system files
//
#include <string>
#include <vector>

//
// Other libraries' headers
//

// Indirect OGRE inclusion.
#include "config/serpentsogre.h"

// OGRE resource handling.
#include "OgreResourceGroupManager.h"

// miscellaneous guslib utilities
#include "guslib/common/smartPtr.h"
#include "guslib/system/commandline.h"
#include "guslib/system/timer.h"
#include "guslib/trace/trace.h"
#include "guslib/util/config.h"
#include "guslib/util/filehelper.h"

// gussound headers
#include "gussound.h"
#include "gopenal.h"

//
// This project's headers
//

// Resource file needed for icon definition.
#include "../res/resource.h"


#include "config/serpents_version.h"
#include "app/serpentsengine.h"
#include "app/serpentsframelistener.h"

// Game helper
#include "app/SerpentsGameHelper.h"

// TODO: move to another place?
#include "impl/WorkbenchState.h"

// TODO: move to another place?
//#include "impl/MenuState.h"


//// Define a pointer function to the Server.
void tlaunch_renderMain(void* op)
{
  app::OgreApp *castedApp = static_cast<app::OgreApp*>(op);
  castedApp->threadRenderMain();
}
void tlaunch_logicMain(void* op)
{
  app::OgreApp *castedApp = static_cast<app::OgreApp*>(op);
  castedApp->threadLogicHandlingMain();
}

void tlaunch_inputMain(void* op)
{
  app::OgreApp *castedApp = static_cast<app::OgreApp*>(op);
  castedApp->threadInputMain();
}

void tlaunch_networkingMain(void* op)
{
  app::OgreApp *castedApp = static_cast<app::OgreApp*>(op);
  castedApp->threadNetworkingMain();
}

namespace app
{
  OgreApp::OgreApp()
    : BaseAppInterface(),
      enginePtr_(0),
      frameListenerPtr_(0),
      renderWindowPtr_(0),
      materialListenerPtr_(0),
      ogreRootPtr_(0),
      exitRequired_(false),
      bInitReady_(false),
      useLogicThread_(false),
      useInputThread_(false),
      renderSysPtr_(NULL)
  {
    sceneManagerPtr = NULL;
    guiPtr_ = NULL;

    canStartLogicThread_ = false;
    threadInputFinished_ = false;
    threadInputStarted_ = false;
    threadLogicFinished_ = false;
    threadLogicStarted_ = false;
    threadNetworkFinished_ = false;
    threadNetworkStarted_ = false;
    threadRenderFinished_ = false;
    threadRenderStarted_ = false;
  }


  OgreApp::~OgreApp()
  {
    enginePtr_->releaseFrameListener();
    enginePtr_->releaseGUI();
    frameListenerPtr_->releaseEngine();

    delete frameListenerPtr_;
    frameListenerPtr_ = 0;

    delete materialListenerPtr_;
    materialListenerPtr_ = 0;

    delete enginePtr_;
    enginePtr_ = 0;

    delete guiPtr_;
    guiPtr_ = 0;
  }


  //
  // ---------------------------- Pre - initialization -------------------------------------------
  // Load config file and start the standard tracing.
  //

  bool OgreApp::preInit()
  {
    // Use a different settings file for the debug scenario and the release scenario(s).
    // This is required as the plugin list is defined in the settings file, and different plugins are used
    // for the debug and the release modes.
    // load the config.
    try
    {
      // Try to load the config file. Allow several locations to be used.
      std::vector <std::string> allowedIniFileLocations;

      // The following locations will be used in the order in which they are given here.
      // As soon as the first one is found, it is loaded. All others are ignored afterwards.
#if _DEBUG
      std::string cfgFileName("settings_dbg.ini");
#else
      std::string cfgFileName("settings.ini");
#endif
      allowedIniFileLocations.push_back("data/");
      allowedIniFileLocations.push_back("../Game/data/");
      allowedIniFileLocations.push_back("Game/data/");
      allowedIniFileLocations.push_back("../../Game/data/");

      for (std::vector <std::string>::iterator it = allowedIniFileLocations.begin();
          it != allowedIniFileLocations.end(); ++it)
      {
        std::string targetFile(*it);
        targetFile.append(cfgFileName);
        if (guslib::filehelper::IsFileAccessible(targetFile))
        {
          config_.load(targetFile);
          std::string dataFolder = guslib::filehelper::RemoveLastFolderFromName(*it, true);
          // Store the data folder. Strip the final part from it.
          // We'll be assuming the path starts from the parent dir of "data".
          config_["_auto_"]["dataFolder"].setAsString(dataFolder);

          // This group (_auto_) is used to store temporary configurations only.
          // Make sure these are not saved to the disk.
          config_["_auto_"].setSavable(false);
          break;
        }
      }

      // Ok, what if no config file was found? Throw an exception.
      if (config_.getGroups().size() == 0)
      {
        std::string errorMessage =("Could not locate settings.ini file. Tried:");
        for (std::vector <std::string>::iterator it = allowedIniFileLocations.begin();
          it != allowedIniFileLocations.end(); ++it)
        {
          errorMessage.append("\n");
          errorMessage.append(*it);
        }
        throw guslib::SimpleException(errorMessage.c_str());
      }
    }
    catch (const std::exception &e)
    {
      // TODO(Augustin Preda, 2014.04.26): make cross platform.
      MessageBoxA(NULL, (LPCSTR)e.what(), (LPCSTR)"some error for settings!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
      return false;
    }

    app::SerpentsGameHelper::getPtr()->init();

    // Obtain the tracing file and level.
    // Please keep in mind that the trace level must be higher than 0 to allow tracing.

    std::string traceFile(config_["logging"]["traceFile"].getAsStringOrDefaultVal("zserptrace.log"));
    int traceLevel(config_["logging"]["traceLevel"].getAsIntOrDefaultVal(3));

    if (traceLevel > 0 && traceFile.length() > 0)
    {
      GSTARTTRACING(traceFile.c_str(), traceLevel);
    }

    // Write header to trace file.
    // TODO(Augustin Preda, 2014.04.26): replace this header with a separate class dedicated to identifying
    // relevant information to place in the log file.
    GTRACE(3, "Arguments: [" << guslib::CommandLine::getProgramArguments() << "]");
    GTRACE(1, "Trace file for " << "Serpents" << " v. "<< SERPENTS_STRING_VERSION);
    GTRACE(1, "Expecting data folder location: " << config_["_auto_"]["dataFolder"].getAsStringOrDefaultVal("unknown!"));
    GTRACE(1, "-------------------------------------------------------------");

    // Renew the application clock, so that when it's first triggered, it doesn't come up with a huge value.
    guslib::AppClock::getPtr()->renew();

    return true;
  }


  //
  // Application run function - create render thread - > initialize Ogre backend.
  // - create input thread.
  //


  void OgreApp::run()
  {
    // Make some pre-initializations (load the config, start logging).
    preInit();

    useInputThread_ = config_["thread"]["separateInputThread"].getAsBoolOrDefault(false);
    useLogicThread_ = config_["thread"]["separateLogicThread"].getAsBoolOrDefault(false);

    try
    {
      // Start the working threads.
      // The rendering thread is always started.
      GUS_THREAD_TYPE thread1(tlaunch_renderMain, this);
      // GUS_THREAD_TYPE thread1(boost::bind(&OgreApp::threadRenderMain, this));

      const int waitingForInitSleepDuration = 20; // TODO: get from config?
      int nMax = 5000; // 5 seconds; TODO: get from config.

      // Wait for the initialization to finish.
      while (!isExitRequired() && !isInitializationReady() && nMax > 0)
      {
        GUS_THREAD_SLEEP(waitingForInitSleepDuration);
        nMax -= waitingForInitSleepDuration;
      }

      // Initialization not finished successfully? Then stop.
      if (isExitRequired())
      {
        cleanup();
        return;
      }

      // Initialization is done.
      // Secondary threads may be started.

      GUS_THREAD_TYPE thread2(tlaunch_logicMain, this);
      // GUS_THREAD_TYPE thread2(boost::bind(&OgreApp::threadLogicHandlingMain, this));

      GUS_THREAD_TYPE thread3(tlaunch_inputMain, this);
      // GUS_THREAD_TYPE thread3(boost::bind(&OgreApp::threadInputMain, this));

      GUS_THREAD_TYPE thread4(tlaunch_networkingMain, this);
      // GUS_THREAD_TYPE thread4(boost::bind(&OgreApp::threadNetworkingMain, this));


      if (useLogicThread_)
      {
        GTRACE(4, "Signaling that extra threads have started that can use the render API.");
        renderSysPtr_->postExtraThreadsStarted();
      }

      GTRACE(4, "Joining threads");

      // Join the main thread. This means, we wait for this thread to finish.
      // It also means that we must ensure that the other threads will be finished prior to this main thread.
      thread1.join();
      cleanup();
    }
    catch (const Ogre::Exception & e)
    {
      // TODO(Augustin Preda, 2014.04.26): Take platform into account!
      MessageBoxA(NULL, e.getFullDescription().c_str(), "An OGRE exception has occured!",
          MB_OK | MB_ICONERROR | MB_TASKMODAL);
    }
  }


  bool OgreApp::isExitRequired()
  {
    return exitRequired_;
  }

  bool OgreApp::isInitializationReady()
  {
    // guslib::GScopedLock myLock(initializationMutex);
    return bInitReady_;
  }

  bool OgreApp::isThreadInputFinished()
  {
    if (useInputThread_)
    {
      return threadInputFinished_;
    }

    return exitRequired_;
  }

  bool OgreApp::isThreadInputStarted()
  {
    if (useInputThread_)
    {
      return threadInputStarted_;
    }

    return true;
  }

  bool OgreApp::isThreadLogicFinished()
  {
    if (useLogicThread_)
    {
      return threadLogicFinished_;
    }

    return exitRequired_;
  }

  bool OgreApp::isThreadLogicStarted()
  {
    if (useLogicThread_)
    {
      return threadLogicStarted_;
    }

    return true;
  }

  bool OgreApp::isThreadNetworkFinished()
  {
    return threadNetworkFinished_;
  }

  void OgreApp::setExitRequired (bool value, const std::string &description)
  {
    //guslib::GScopedLock myLock (threadStateMutex_);
    GTRACE(2, "Received exit required signal; description: " << description);
    exitRequired_ = value;
  }

  void OgreApp::setInitializationReady (bool value)
  {
    //guslib::GScopedLock myLock (initializationMutex);
    bInitReady_ = value;
  }

  void OgreApp::setThreadInputFinished (bool value)
  {
    //guslib::GScopedLock myLock (threadStateMutex);
    threadInputFinished_ = value;
  }
  void OgreApp::setThreadInputStarted (bool value)
  {
    //guslib::GScopedLock myLock (threadStateMutex);
    threadInputStarted_ = value;
  }

  void OgreApp::setThreadLogicFinished (bool value)
  {
    //guslib::GScopedLock myLock (threadStateMutex);
    threadLogicFinished_ = value;
  }

  void OgreApp::setThreadLogicStarted (bool value)
  {
    //guslib::GScopedLock myLock( threadStateMutex);
    threadLogicStarted_ = value;
  }

  void OgreApp::setThreadNetworkFinished (bool value)
  {
    //guslib::GScopedLock myLock (threadStateMutex);
    threadNetworkFinished_ = value;
  }

  void OgreApp::setThreadRenderFinished (bool value)
  {
    //guslib::GScopedLock myLock( threadStateMutex);
    threadRenderFinished_ = value;
  }


  void OgreApp::cleanup()
  {
    GTRACE(1, "OgreApp: cleanup");
    setExitRequired (true, "cleanup");

    //if (enginePtr)
    //{

    //}
    // Get rid of the used singletons

    // Get rid of the resource manager.
  }


  bool OgreApp::initGUI()
  {
    if (renderSysPtr_)
    {
      std::string renderTargetName("");

      Ogre::RenderSystem::RenderTargetIterator it = renderSysPtr_->getRenderTargetIterator();

      // Normally, a single render target is expected.
      while (it.hasMoreElements())
      {
        if (renderTargetName.length() == 0) // string is empty
        {
          renderTargetName = it.current()->second->getName();
        }

        GTRACE(3, "Render target: " << renderTargetName);
        it.moveNext();
      }

      std::string guiLogFileName(config_["logging"]["ceguiLog"].getAsString());
      int guiLogLevel = config_["logging"]["ceguiLogLevel"].getAsIntOrDefaultVal(2);

      guiPtr_ = new SerpGUI(renderSysPtr_->getRenderTarget(renderTargetName), guiLogFileName, guiLogLevel);
      std::vector <std::string> schemeList = config_["resources"]["Schemes"].getAsStringList();

      // Assume that the scheme list is relative to the data folder.
      std::string dataFolder(config_["_auto_"]["dataFolder"].getAsString());

      guiPtr_->initialize();
      guiPtr_->loadSchemes(schemeList);

      std::string defaultMouseCursor(
          config_["resources"]["defaultCursor"].getAsStringOrDefaultVal("GlossySerpentCursors/MouseArrow"));
      guiPtr_->setMouseCursor(defaultMouseCursor);

      GTRACE(3, "Loading fonts");
      GTRACE(3, "TODO:  verify if fonts should be set to auto-scaled!!!");
      // TODO:  verify if fonts should be set to auto-scaled!!!

      std::vector <std::string> fontList = config_["resources"]["Fonts"].getAsStringList();
      for (std::vector <std::string>::iterator it = fontList.begin();
        it != fontList.end(); ++it)
      {
        guiPtr_->addFont(*it);
      }

      std::string defaultFont = config_["resources"]["DefaultFont"].getAsString();
      if (defaultFont.length() > 0)
      {
        std::string fontLocation = dataFolder + defaultFont;
        guiPtr_->setDefaultFont(fontLocation);
      }
      GTRACE(3, "Done loading fonts");

      enginePtr_->setGUIPtr(guiPtr_);
      return true;
    }
    return false;
  }

  bool OgreApp::initSound()
  {
    GTRACE(3, "Initializing sound");

    try
    {
      std::string sound_log_file_name(config_["logging"]["soundLog"].getAsString());

      // Set the logging details
      gussound::SoundManagerLogger::setLoggerTarget(sound_log_file_name.c_str(), 4);
      gussound::SoundManagerFactory::getPtr()->Register("openal", GOpenAl::OpenAlManagerUtil::createSoundManager);
      gussound::SoundManager::setPtr(
          gussound::SoundManagerFactory::getPtr()->CreateObject("openal"));

      // TODO(Augustin Preda, 2014.04.26): this is a temporary hack. Should be integrated into the class.
      ((GOpenAl::OpenAlManagerUtil*)gussound::SoundManager::getPtr())->setListenerPosition (0, 0, 0);

      GTRACE(3, "Done initializing sound");
    }
    catch (const std::exception& e)
    {
      GTRACE(2, "Caught exception: " << e.what());
    }

    return true;
  }


  /**
    Load a plugin identified by its relative path
  */
  bool OgreApp::LoadPlugin(const std::string& pluginName)
  {
    GTRACE(5, "Loading plugin [" << pluginName << "]");
    try
    {
      ogreRootPtr_->loadPlugin(pluginName);
    }
    catch (const Ogre::Exception& e)
    {
      GTRACE(1, "Exception encountered! " << e.getFullDescription());
      return false;
    }

    GTRACE(3, "Loaded plugin [" << pluginName << "]");
    return true;
  }

  /**
    Load a plugin identified by its relative path in a given set of possible directories.
  */
  bool OgreApp::LoadPluginFromLocations(
      const std::string& pluginName,
      std::vector <std::string>& pluginDirectoryList)
  {
    for (std::vector <std::string>::iterator it = pluginDirectoryList.begin(); it != pluginDirectoryList.end(); ++it)
    {
      std::string pluginWithPath(*it);
      pluginWithPath.append(pluginName);

      bool loaded = this->LoadPlugin(pluginWithPath);
      if (loaded)
      {
        return loaded;
      }
    }

    // We reached this point. It means that the plugin was not loaded from any path.
    std::string message = std::string("Unable to load plugin [");
    message.append(pluginName);
    message.append("].\nVerify that there are plugins stored at a location (relative to the game's directory)\n");
    for (std::vector <std::string>::iterator it = pluginDirectoryList.begin(); it != pluginDirectoryList.end(); ++it)
    {
      message.append("-> ");
      message.append(*it);
      message.append("\n");
    }

    GTRACE(2, message);
    return false;
  }


  /**
    Load expected plugins list. (the list is read from configuration).
  */
  void OgreApp::LoadPluginsOrDie()
  {
    // Note: it is possible to load only one plugin instead of both D3D and OpenGL, but while
    // this will provide a speed-up at the application startup it will make the resolution changes more difficult,
    // since the DLL would still require loading, then and reading the supported settings.
    // So for the sake of simplicity it's better to load the data in a single place.

    // The loading of additional plugins is done in the same loop also for simplicity.
    std::string dataFolder(config_["_auto_"]["dataFolder"].getAsString());
    std::string pluginFolder(config_["plugins"]["pluginDir"].getAsStringOrDefaultVal("data/plugins/"));

    std::vector <std::string> pluginDirectories(config_["plugins"]["pluginDir"].getAsStringList());
    for (std::vector <std::string>::iterator it = pluginDirectories.begin(); it != pluginDirectories.end(); ++it)
    {
      *it = dataFolder + *it;
      *it = guslib::filehelper::EnsureTrailingSeparator(*it, true, true);
      *it = guslib::filehelper::NormalizeFileOrFolderName(*it, true);
    }

    // Assume that the plugin folder is relative to the data folder.
    std::string pluginHost(dataFolder);
    pluginHost.append(pluginFolder);

    GTRACE(3, "The plugins will be loaded from [" << pluginHost << "]");

    // Make sure the plugin dir has a trailing slash. Allthough it would be nicer to separate between
    // linux slash and windows double backslash... why bother? windows can interpret slash as well.
    pluginHost = guslib::filehelper::EnsureTrailingSeparator(pluginHost, true, true);
    pluginHost = guslib::filehelper::NormalizeFileOrFolderName(pluginHost, true);

    // Get the list of plugins to load. These include the render plugins as well as any scene management and
    // special effects plugins.
    std::vector<std::string> rendererPlugins = config_["plugins"]["rendererList"].getAsStringList();
    std::vector<std::string> pluginList = config_["plugins"]["pluginList"].getAsStringList();

    GTRACE(3, "..:: Loading renderer list(" << rendererPlugins.size() << "  items) ::..");
    int amountOfLoadedRendererPlugins = 0;
    for (std::vector<std::string>::const_iterator it = rendererPlugins.begin(); it != rendererPlugins.end(); ++it)
    {
      if (true == this->LoadPluginFromLocations(*it, pluginDirectories))
      {
        ++amountOfLoadedRendererPlugins;
      }
    }

    // At least one renderer must be loaded successfully.
    if (0 == amountOfLoadedRendererPlugins)
    {
      throw guslib::SimpleException("Could not load any renderer plugins");
    }

    GTRACE(3, "..:: Loading plugin list(" << pluginList.size() << "  items) ::..");
    for (std::vector<std::string>::const_iterator it = pluginList.begin(); it != pluginList.end(); ++it)
    {
      if (false == this->LoadPluginFromLocations(*it, pluginDirectories))
      {
        std::string message = std::string("Unable to load plugin: ") + *it
          + std::string(".\nAre you sure there are plugins stored at the location [")
          + pluginHost + std::string("] ?");

        throw guslib::SimpleException("Not all plugins were loaded");
      }
    }
  }

  /**
    Prepare the initial resource locations to use for the application.
    The resource locations may be altered later on.
  */
  void OgreApp::PrepareInitialResourceLocations()
  {
    // Store the data folder
    std::string dataFolder(config_["_auto_"]["dataFolder"].getAsString());
    GTRACE(3, "..:: Adding resource locations ::..");
    GTRACE(3, "..:: Using the base folder [" << dataFolder << "] as data folder ::..");

    // Load the bootstrap resources.
    std::string groupNameBootstrap = "Bootstrap";  // TODO(Augustin Preda, 2014.04.26): remove hardcoding.
    std::vector <std::string> resList(config_["resources"][groupNameBootstrap].getAsStringList());

    // Adjust the paths
    for (std::vector <std::string>::iterator it = resList.begin(); it != resList.end(); ++it)
    {
      std::string temp(dataFolder);
      temp.append(*it);
      *it = temp;
      GTRACE(3, "Item of resource location for [" << groupNameBootstrap << "] [" << *it << "].");
    }

    app::SerpentsGameHelper::getPtr()->addResourceLocationList(resList, groupNameBootstrap);

    // Load the GUI resources.
    std::string groupNameGUI = "General";  // TODO(Augustin Preda, 2014.04.26): remove hardcoding.
    resList = config_["resources"][groupNameGUI].getAsStringList();
    // Adjust the paths
    for (std::vector <std::string>::iterator it = resList.begin(); it != resList.end(); ++it)
    {
      std::string temp(dataFolder);
      temp.append(*it);
      *it = temp;
      GTRACE(3, "Item of resource location for [" << groupNameGUI << "] [" << *it << "].");
    }

    app::SerpentsGameHelper::getPtr()->addResourceLocationList(resList, groupNameGUI);

    // Load the extra resources.
    std::string groupNameRTSS = "rtss";  // TODO(Augustin Preda, 2014.04.26): remove hardcoding.
    resList = config_["resources"][groupNameRTSS].getAsStringList();
    // Adjust the paths
    for (std::vector <std::string>::iterator it = resList.begin(); it != resList.end(); ++it)
    {
      std::string temp(dataFolder);
      temp.append(*it);
      *it = temp;
      GTRACE(3, "Item of resource location for [" << groupNameRTSS << "] [" << *it << "].");
    }

    app::SerpentsGameHelper::getPtr()->addResourceLocationList(resList, groupNameRTSS);

    // Destroy the "General" resource group, since it will be initialized in the engine, and a "loading..." will
    // be displayed while the actual resource loading is done.
    // TODO(Augustin Preda, 2014.04.26): can't do it yet, as CEGUI requires it.
    // Ogre::ResourceGroupManager::getSingleton().destroyResourceGroup("General");
  }

  /**
    Set the rendering engine automatically, based on the configuration loaded thus far and
    the success of loading various plugins.
    @return True of a rendering engine was set, false otherwise.
  */
  bool OgreApp::AutoSetRenderingEngine()
  {
    // Get the available render systems. We need to select the currently desired one.
    // If you choose to only load one engine at startup, there should be only one entry here.

    GTRACE(4, "Setting rendering engine ...");
    Ogre::RenderSystem *rsys = NULL;
    std::string preferredRenderer = config_["video"]["preferredEngine"].getAsStringOrDefaultVal("Direct3D");

    // The rendered name is stored in the config file as either Direct3D or OpenGL. But Ogre does not recognize
    // those names. So parse the name to something that is recognized by Ogre.
    preferredRenderer = app::SerpentsGameHelper::getPtr()->getDriverNameForPseudoName(preferredRenderer);
    app::SerpentsGameHelper::getPtr()->setPreferredRendererName(preferredRenderer);

    Ogre::RenderSystemList renderers = ogreRootPtr_->getAvailableRenderers();

    for (Ogre::RenderSystemList::iterator rslIt = renderers.begin(); rslIt != renderers.end(); ++rslIt)
    {
      Ogre::String rsName = (*rslIt)->getName();
      GTRACE(3, "Render system to load: " << rsName);
      app::SerpentsGameHelper::get().getEditableAvailableVideoDrivers().push_back(rsName);
      GTRACE(3, "Getting supported display options & resolutions for render system.");
      Ogre::ConfigOptionMap optionsMap = (*rslIt)->getConfigOptions();

      GTRACE(3, "Adding supported video modes to internal mapping.");
      try
      {
        for (Ogre::ConfigOptionMap::iterator optit = optionsMap.begin(); optit != optionsMap.end(); ++optit)
        {
          if (optit->second.name == "Video Mode")
          {
            Ogre::StringVector possibleValues = optit->second.possibleValues;
            app::SerpentsGameHelper::get().getEditableResolutionsMapping()[rsName].clear();

            for (Ogre::StringVector::const_iterator subit = possibleValues.begin();
              subit != possibleValues.end(); ++subit)
            {
              app::SerpentsGameHelper::get().getEditableResolutionsMapping()[rsName].push_back(*subit);
            }
          }
        }
      }
      catch (const std::exception& e)
      {
        GTRACE(2, "Caught exception: " << e.what());
      }

      GTRACE(4, "Available renderer found: " << rsName);
      if ((*rslIt)->getName() == preferredRenderer)
      {
        rsys = *rslIt;
        GTRACE(2, "Setting rendering engine to " << preferredRenderer);
      }
    }

    // If we did not find a preferred engine, try to get any available engine.
    if (rsys == NULL)
    {
      if (renderers.empty())
      {
        GTRACE(1, "ERROR: Unable to get any renderer! (renderers list empty).");
        return false;
      }

      rsys = *(renderers.begin());
    }

    // If we still don't have an engine, it means nothing was loaded, so there's something wrong with the paths.
    if (rsys == NULL)
    {
      GTRACE(1, "ERROR: Unable to get any renderer!");
      return false;
    }

    // Store the renderer.
    ogreRootPtr_->setRenderSystem (rsys);
    renderSysPtr_ = rsys;
    GTRACE(3, "Render system set.");

    return true;
  }


  bool OgreApp::initialize()
  {
    using namespace std;

    // ------------ Start the Ogre app configuring ------------

    GTRACE(3, "Creating Ogre root...");

    // The Ogre logging will be disabled for an empty file name.
    ogreRootPtr_ = new Ogre::Root("", "", config_["logging"]["ogreLog"].getAsString());

    // Load the necessary plugins.
    // This will load the plugins needed to allow OGRE 3D to function properly.
    this->LoadPluginsOrDie();

    // Add the initial resource locations to the OGRE resource group manager.
    this->PrepareInitialResourceLocations();

    // Decide which rendering engine to use (not just based on the setting that the user specifies as
    // desired, but also based on the success of loading the plugins).
    if (false == this->AutoSetRenderingEngine())
    {
      return false;
    }

    // also place the setting for this new device.

    ogreRootPtr_->initialise(false);
    GTRACE(3, "Ogre root initialized");

    // -------------------- Create the window to be used ------------------------------

    // Get the general parameters required for window initialization:
    // fullscreen antialiasing (default: 0), window width and height, fullscreen mode and vertical sync.

    int fsaa = config_["video"]["fsaa"].getAsIntOrDefaultVal(0);
    int width = config_["video"]["window.width"].getAsIntOrDefaultVal(1024);
    int height = config_["video"]["window.height"].getAsIntOrDefaultVal(600);

    int temporaryFullscreenMode = config_["video"]["displayMode"].getAsIntOrDefaultVal(0);
    bool fullscreen = (temporaryFullscreenMode == app::FULLSCREEN_EXCLUSIVE);
    int colourDepth = config_["video"]["colourDepth"].getAsIntOrDefaultVal(32);
    bool vsync = config_["video"]["vsync"].getAsBoolOrDefault(false);

    app::SerpentsGameHelper::getPtr()->setPreferredDisplayMode(static_cast<app::DisplayModes>(temporaryFullscreenMode));

    // If the user selected Windowed (Fullscreen) mode, do additional preparations.
    if (temporaryFullscreenMode == app::WINDOWED_BORDERLESS_FULL_SIZE)
    {
      width = app::SerpentsGameHelper::getPtr()->getDesktopResolutionWidth();
      height = app::SerpentsGameHelper::getPtr()->getDesktopResolutionHeight();
      GTRACE(3, "Windowed (Fullscreen) mode selected; desktop at " << width << " x " << height << ".");
    }

    // Ensure that whatever resolution settings are chosen, they are supported by the system.
    // In case they are not, they will be changed to some acceptable values.

    std::string chosenMode;
    app::ResolutionConfig myResolutionConfig;

    myResolutionConfig.depth = colourDepth;
    myResolutionConfig.width = width;
    myResolutionConfig.height = height;

    myResolutionConfig = app::SerpentsGameHelper::getPtr()->validateWindowSettings(ogreRootPtr_, myResolutionConfig);

    width = myResolutionConfig.width;
    height = myResolutionConfig.height;
    colourDepth = myResolutionConfig.depth;
    chosenMode = myResolutionConfig.fullText;

    app::SerpentsGameHelper::getPtr()->setPreferredResolutionConfig(myResolutionConfig);

    std::string preferredRenderer = ogreRootPtr_->getRenderSystem()->getName();
    config_["video"]["preferredEngine"].setAsString(
      app::SerpentsGameHelper::getPtr()->getPseudoNameForDriverName(preferredRenderer));
    config_["video"]["window.width"].setAsInt(width);
    config_["video"]["window.height"].setAsInt(height);

    Ogre::NameValuePairList miscParams;
    miscParams["vsync"] = Ogre::StringConverter::toString(vsync);
    miscParams["VSync Interval"] = 1; // TODO: investigate possible values.
    miscParams["FSAA"] = Ogre::StringConverter::toString(fsaa);

    //miscParams["Display Frequency"]= StringConverter::toString(0);
    //miscParams["Colour Depth"]=StringConverter::toString(32);

    miscParams["displayFrequency"] = Ogre::StringConverter::toString(0);
    GTRACE(4, "Setting colour depth to : " << colourDepth);
    miscParams["colourDepth"] =  Ogre::StringConverter::toString(colourDepth);

    renderWindowPtr_ = ogreRootPtr_->createRenderWindow("Gusty's Serpents", width, height,
                      fullscreen, &miscParams);
    GTRACE(3, "Created app window, size=" << renderWindowPtr_->getWidth() << "," << renderWindowPtr_->getHeight()
      <<", fullscr=" << renderWindowPtr_->isFullScreen()
      <<", depth=" << renderWindowPtr_->getColourDepth());

    // TODO: store the actual resolution for the aspect ratio.

    // store the used resolution in the helper.
    {
      int dummyX = 0;
      int dummyY = 0;
      std::string sampleResolutionForRenderer = app::SerpentsGameHelper::getPtr()->retrieveSampleRenderSystemWindowSize(
          ogreRootPtr_, dummyX, dummyY);
      sampleResolutionForRenderer = app::SerpentsGameHelper::getPtr()->getUpdatedResolutionString(
          sampleResolutionForRenderer, width, height);
      app::SerpentsGameHelper::getPtr()->setPreferredResolutionName(sampleResolutionForRenderer);

      GTRACE(4, "Set preferred resolution for helper to " << sampleResolutionForRenderer);
    }

    GTRACE(4, "Setting run-time application icon...");
    app::SerpentsGameHelper::getPtr()->setApplicationIcon(renderWindowPtr_, IDI_GUS_APP_ICON);

    // If the user selected Windowed (Fullscreen) mode, do additional preparations.
    if (temporaryFullscreenMode == 2)
    {
      app::SerpentsGameHelper::getPtr()->setBorderlessWindowWithSize(renderWindowPtr_, width, height);
    }

    // Also calculate the aspect ratio that can be used for the GUI.
    GTRACE(3, "Calculating gui aspect ratio and adding resource locations accordingly");
    std::string currentAspectRatioString;
    currentAspectRatioString = app::SerpentsGameHelper::getPtr()->getNearestAspectRatioStringForWindowSize(
      width, height);
    app::SerpentsGameHelper::getPtr()->getEditableApplicationVariablesMapping()["ASPECT_RATIO"] = currentAspectRatioString;
    app::SerpentsGameHelper::getPtr()->retryToAddIncompleteResourceDirs();


    // Set the texture filtering.
    // Possible values:
    // 0 - TFO_NONE
    // 1 - TFO_BILINEAR
    // 2 - TFO_TRILINEAR
    // 3 - TFO_ANISOTROPIC

    int textureFilteringMode = config_["video"]["textureFiltering"].getAsIntOrDefaultVal(3);

    // Change the default texture filtering mode.
    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(
        static_cast<Ogre::TextureFilterOptions>(textureFilteringMode));

    // Make the window active and autoupdated when rendered.

    renderWindowPtr_->setActive(true);
    renderWindowPtr_->setAutoUpdated(true);
    GTRACE(3, "Set window to active + autoupdate");

    // Choose a scene manager. In this case, exterior, from up close.

    sceneManagerPtr = ogreRootPtr_->createSceneManager(Ogre::ST_EXTERIOR_CLOSE, "SceneMgr");

    GTRACE(3, "Creating camera...");

    Ogre::Camera * myCam = sceneManagerPtr->createCamera("PlayerCam");
    int nearClipDistance = config_["camera"]["clipNear"].getAsIntOrDefaultVal(5);
    myCam->setNearClipDistance(5);

    Ogre::Viewport *vp = renderWindowPtr_->addViewport(myCam, 10);
    vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0)); // TODO: parameterize
    myCam->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));

    // --- second camera
    Ogre::Camera * secCam = sceneManagerPtr->createCamera("SecondaryCam");
    nearClipDistance = config_["camera"]["clipNear"].getAsIntOrDefaultVal(5);
    secCam->setNearClipDistance(5);

    Ogre::Viewport *secvp = renderWindowPtr_->addViewport(secCam, 5, 0, 0);
    secvp->setBackgroundColour(Ogre::ColourValue(0, 0, 0)); // TODO: parameterize
    secCam->setAspectRatio(Ogre::Real(secvp->getActualWidth()) / Ogre::Real(secvp->getActualHeight()));


    // Set default mipmap level (NB some APIs ignore this)

    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    // Create the game engine and frame listener.
    enginePtr_ = new SerpEngine(sceneManagerPtr, ogreRootPtr_, "serp");
    enginePtr_->setMainCamera(myCam);
    enginePtr_->setSecondaryCamera(secCam);
    enginePtr_->setConfig(config_);
    frameListenerPtr_ = new SerpFrameListener(enginePtr_, renderWindowPtr_);
    enginePtr_->setFrameListener(frameListenerPtr_);

    // Also add a material listener.
    materialListenerPtr_ = new MaterialListener();
    Ogre::MaterialManager::getSingleton().addListener(materialListenerPtr_);

    std::string preferredLevel = config_["gameplay"]["preferredWorkbench"].getAsStringOrDefaultVal("");
    enginePtr_->getStateMgr().addState(new Serpents::WorkbenchState("test", preferredLevel, enginePtr_));
    //enginePtr_->getStateMgr().addState(new Serpents::MenuState("menu_test", enginePtr_));

    guslib::Configuration cfgMenu;
    enginePtr_->getStateMgr().setStateNextTick("test", app::SerpStateParams(cfgMenu));

#if GUS_USE_RTSS
    GTRACE(3, "Initializing the RTSS...");
    SerpEngineStartupSettings engineStartupSetting;
    engineStartupSetting.preferredShadingLanguage = config_["general"]["preferredShadingLanguage"].getAsStringOrDefaultVal("cg");
    engineStartupSetting.storeRtssCacheInMemory = config_["general"]["rtssMemoryCache"].getAsBoolOrDefault(false);
    bool bResult = enginePtr_->initializeRTShaderSystem(enginePtr_->getSceneManagerPtr(), engineStartupSetting);
    GTRACE(3, "Result: " << bResult);

    Ogre::Viewport* mainVP = myCam->getViewport();
    mainVP->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

    // for the time being, don't use shaders for the second viewport(s)

#endif

#if 0
    StencilOpQueueListener *stencilListener = new StencilOpQueueListener();
    int minGroup = GBaseAppSettings::getPtr()->setting.renderg.firstID;
    int maxGroup = GBaseAppSettings::getPtr()->setting.renderg.lastID;

    if (minGroup != 0 && maxGroup != 0)
    {
      stencilListener->setFirstStencilOpRenderQueue( minGroup);
      stencilListener->setLastStencilOpRenderQueue( maxGroup);
      for ( unsigned i = 0; i<GBaseAppSettings::getPtr()->setting.renderg.groups.size(); ++i)
      {
        stencilListener->addRenderQueue(
          GBaseAppSettings::getPtr()->setting.renderg.groups[i].id,
          GBaseAppSettings::getPtr()->setting.renderg.groups[i].compareFunc,
          GBaseAppSettings::getPtr()->setting.renderg.groups[i].stencilFailOp,
          GBaseAppSettings::getPtr()->setting.renderg.groups[i].depthFailOp,
          GBaseAppSettings::getPtr()->setting.renderg.groups[i].passOp,
          GBaseAppSettings::getPtr()->setting.renderg.groups[i].value);
      }
      sceneManager->addRenderQueueListener(stencilListener);
    }
#endif

    if (ogreRootPtr_->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_HWRENDER_TO_TEXTURE))
    {
      // In D3D (and OpenGL with modern HW support) use a 1024x1024 shadow texture
      sceneManagerPtr->setShadowTextureSettings(2048, 2);
    }
    else
    {
      // Use 256x256 texture in old GL since we can't go higher than the window res
      // Make sure you use a resolution higher than this!
      sceneManagerPtr->setShadowTextureSettings(256, 2);
    }

    setInitializationReady(true);
    return true;
  }


  // -----------------------------------------------------------------------------------------------
  // Main Render thread.
  // -----------------------------------------------------------------------------------------------
  void OgreApp::threadRenderMain()
  {
    GTRACE(3, "OgreApp::threadRenderMain : calling initialize");

    if (false == initialize())
    {
      setExitRequired(true, "render: failed init");
      cleanup();
      return;
    }

    if (false == initGUI())
    {
      GTRACE(1, "OgreApp::threadRenderMain could not init GUI; threadID : " << GUS_THREAD_CURRENT_ID);
    }

    if (false == initSound())
    {
      GTRACE(1, "OgreApp::threadRenderMain could not init Sound; threadID : " << GUS_THREAD_CURRENT_ID);
    }

    GTRACE(3, "OgreApp::threadRenderMain start. threadID : " << GUS_THREAD_CURRENT_ID);
    if (enginePtr_ == NULL || renderSysPtr_ == NULL)
    {
      setExitRequired(true, "render: null ptrs");
      return;
    }

    renderSysPtr_->preExtraThreadsStarted();

    GTRACE(5, "Creating game states");

    // Local constants, retrieved from config
    int iSleep = config_["thread"]["delayBetweenRenders"].getAsIntOrDefaultVal(10);

    GTRACE(3, "Beginning render loop");
    if (false == useLogicThread_)
    {
      GTRACE(3, "Render thread will include logic render loop");
    }

    bool bKeepLoop = true;
    while (bKeepLoop)
    {
      if (enginePtr_->isExiting())
      {
        break;
      }

      //  allow the message pumping to take place. Without it, application switching (alt-tab) will not work ok.
      Ogre::WindowEventUtilities::messagePump();

      if (enginePtr_->isExiting())
      {
        break;
      }

      if (! ogreRootPtr_)
      {
        GTRACE(1, "ERROR!!! ogre root element is NULL");
        break;
      }

      // get input if input thread is not used?
      if (false == useInputThread_)
      {
        if (frameListenerPtr_ != NULL)
        {
          frameListenerPtr_->getOISKeyboard()->capture();
          frameListenerPtr_->getOISMouse()->capture();
        }
      }

      if (! useLogicThread_)
      {
        enginePtr_->logicThreadLoop(); // TODO: something like this.
      }

      //  Render a frame
      bKeepLoop = enginePtr_->renderCompleteScene();

      if (bKeepLoop && enginePtr_->isExiting())
      {
        GTRACE(3, "Main App thread: engine says it is exiting");
        bKeepLoop = false;
        setExitRequired(true, "render: engine says it's exiting");
      }

      //  Add some cpu idle time.
      if (iSleep > 10)
      {
        iSleep = 10;
      }

      // If setting is smaller than 1, this means no pause for the thread.
      // (for the value 0, the thread will cede cpu control to other threads with similar priority)
      if (iSleep >= 0)
      {
        GUS_THREAD_SLEEP(iSleep);
      }
    }

    GTRACE(3, "Finished render loop");

    // TODO: Unload any OGRE compositor schemes here. ?
    enginePtr_->cleanupRTSGResources();

    // This is the main thread; it was joined with the application's main thread, so before leaving this
    // one, make sure all the other threads have the chance to finish nicely, otherise their new state
    // will be a total mistery => errors/crashes as game exit.
    setExitRequired(true, "render: done render loop");
    waitForSecondaryThreadsToFinish();

    if (ogreRootPtr_)
    {
      delete ogreRootPtr_;
      ogreRootPtr_ = 0;
    }

    setThreadRenderFinished();
    GTRACE(3, "OgreApp::thread1 clean end.");
  }


  void OgreApp::threadNetworkingMain()
  {
    GTRACE(3, "OgreApp::networking thread start");
    const int waitingForInitSleepDuration = 20; // TODO: get from config?

    // Wait for the initialization to finish.
    while (!isExitRequired() && !isInitializationReady())
    {
      GUS_THREAD_SLEEP(waitingForInitSleepDuration);
    }

    GTRACE(3, "OgreApp::threadNetworkingMain done waiting for init. starting translation.; threadID: " << GUS_THREAD_CURRENT_ID);

    int iSleep = 10; // TODO: get from config!
    // Start the processing.
    while (!isExitRequired())
    {
      GUS_THREAD_SLEEP(iSleep);
    }

    setThreadNetworkFinished(true);
    GTRACE(3, "OgreApp::threadNetworkingMain end.");
  }


  // -----------------------------------------------------------------------------------------------
  // Input reading thread.
  // -----------------------------------------------------------------------------------------------
  void OgreApp::threadInputMain()
  {
    if (false == useInputThread_)
    {
      GTRACE(3, "OgreApp::threadInputMain - NOT USED!");
      return;
    }

    bool canImpactRendering(false);
    GTRACE(3, "OgreApp::threadInputMain start; threadID : " << GUS_THREAD_CURRENT_ID);

    const int inputThreadSleepDuration = 20; // TODO: get from config?

    if (canImpactRendering)
    {
      if (renderSysPtr_)
      {
        GTRACE(3, "OgreApp - input thread registering to rendersys.");
        setThreadInputStarted();
      }
      else
      {
        setExitRequired(true, "logic: null render sys");
        return;
      }
    }

    // Start the processing.
    while (!isExitRequired())
    {
      if (frameListenerPtr_ != NULL)
      {
        frameListenerPtr_->getOISKeyboard()->capture();
        frameListenerPtr_->getOISMouse()->capture();
      }

      GUS_THREAD_SLEEP(inputThreadSleepDuration);
    }

    setThreadInputFinished(true);
    if (canImpactRendering)
    {
      GTRACE(3, "OgreApp::threadInputMain unregistering from rendersys.");
      if (renderSysPtr_)
      {
        renderSysPtr_->unregisterThread();
      }
    }

    GTRACE(3, "OgreApp::threadInputMain end.");
  }


  // -----------------------------------------------------------------------------------------------
  // Game Logic thread.
  // -----------------------------------------------------------------------------------------------

  void OgreApp::threadLogicHandlingMain()
  {
    if (false == useLogicThread_)
    {
      GTRACE(3, "OgreApp::threadLogicHandlingMain - NOT USED!");
      return;
    }

    bool canImpactRendering(false);
    GTRACE(3, "OgreApp::threadLogicHandlingMain start; threadID : " << GUS_THREAD_CURRENT_ID);

    if (canImpactRendering)
    {
      if (renderSysPtr_)
      {
        GTRACE(3, "GBaseMainApp::threadLogicHandlingMain registering to rendersys.");
        renderSysPtr_->registerThread();
      }
      else
      {
        setExitRequired(true, "logic: null render sys");
        return;
      }
    }

    setThreadLogicStarted();

    int iSleep = 10; // TODO: get from config. GBaseAppSettings::getPtr()->setting.perf.cpuSleep;
    GTRACE(3, "OgreApp::threadLogicHandlingMain done waiting for init. starting translation.");

    // do the loop
    bool bKeepLoop = true;
    while (bKeepLoop)
    {
      if (enginePtr_->isExiting())
      {
        break;
      }
      enginePtr_->logicThreadLoop(); // TODO: something like this.

      if (bKeepLoop && enginePtr_->isExiting())
      {
        GTRACE(3, "Main App thread: engine says it is exiting");
        bKeepLoop = false;
        setExitRequired(true, "logic: engine says it's exiting");
      }

      //  Add some cpu idle time.

      // If setting is smaller than 1, this means no pause for the thread.
      // (for the value 0, the thread will cede cpu control to other threads with similar priority)
      if (iSleep >= 0)
      {
        GUS_THREAD_SLEEP(iSleep);
      }
    }

    setThreadLogicFinished();

    if (canImpactRendering)
    {
      GTRACE(3, "OgreApp::threadLogicHandlingMain unregistering from rendersys.");
      if (renderSysPtr_)
      {
        renderSysPtr_->unregisterThread();
      }
    }
    GTRACE(3, "OgreApp::threadLogicHandlingMain end.");
  }


  void OgreApp::waitForSecondaryThreadsToFinish()
  {
    GTRACE(4, "Waiting for secondary threads to finish... iexitreq=" << isExitRequired());
    // giving a max of 5 seconds
    int nMaxCount = 5000;
    int nDelay = 10;

    while (false == isThreadInputFinished()
        && false == isThreadNetworkFinished()
        && false == isThreadLogicFinished()
        && nMaxCount > 0)
    {
      GTRACE(5, "timeleft... " << nMaxCount);
      GUS_THREAD_SLEEP(nDelay);
      nMaxCount -= nDelay;
    }
    GTRACE(4, "Waited for secondary threads to finish... time left was: " << nMaxCount);
  }
}
