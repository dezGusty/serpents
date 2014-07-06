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
//    Menu State handling class.
//
//    Last change:  $LastChangedDate$
//    Revision:    $Revision$

//
// Includes
//

//
// Own header.
//
#include "impl/menustate.h"

//
// C++ system headers
//
#include <string>

//
// Other libraries' headers
//

#include "guslib/trace/trace.h"
#include "OgreResourceGroupManager.h" // TODO: Move somewhere else? (2.5s)

//
// This project's headers
//

#include "app/serpentsengine.h"
#include "app/serpentsgui.h"
#include "engine/serpentsgamehelper.h"
#include "utils/CEGUIUtils.h"

namespace Serpents
{
  /**
    Opaque pointer: implementation of members and events specific to the GUI library.
  */
  class MenuState::GUIInternals
  {
  public:
    MenuState* ptrToOwner_;
    CEGUI::FrameWindow* optionsWindow_;

    /**
      Constructor
    */
    GUIInternals(MenuState* ptrToOwner)
      : ptrToOwner_(ptrToOwner),
        optionsWindow_(NULL)
    {
    }

    void loadResolutionOptionsForSelectedRenderer()
    {
      CEGUI::Combobox* cbo;
      cbo = static_cast<CEGUI::Combobox*>(utils::CEGUIHelperUtil::getWindow("VideoDriverBox"));
      if (cbo)
      {
        std::string currentEngine;
        // Get the items for the selected rendered.
        currentEngine = cbo->getSelectedItem()->getText().c_str();

        cbo = static_cast<CEGUI::Combobox*>(utils::CEGUIHelperUtil::getWindow("ResolutionBox"));
        if (cbo)
        {
          // clear existing items
          cbo->resetList();

          // Get the list of resolutions permitted for the selected renderer.
          std::vector <std::string> resOptions =
            app::SerpentsGameHelper::getPtr()->getEditableResolutionsMapping()[currentEngine];

          // Get the pre-saved config.
          app::ResolutionConfig resCfg =
            app::SerpentsGameHelper::getPtr()->getPreferredResolutionConfig();

          // And calculate the preferred resolution based on the preferred config
          std::string prefResolution =
            app::SerpentsGameHelper::getPtr()->getUpdatedResolutionString(*resOptions.begin(), resCfg.width, resCfg.height, resCfg.depth);
          GTRACE(4, "Expecting preferred resolution as: " << prefResolution);

          // Add the entries to the list.
          for (std::vector <std::string>::iterator it = resOptions.begin();
            it != resOptions.end(); ++ it)
          {
            cbo->addItem(new utils::SimpleListItem((CEGUI::utf8*) it->c_str()
                , "GlossySerpent", "ComboboxSelectionBrush", ""));

            // Select the current driver.
            if (*it == prefResolution)
            {
              GTRACE(4, "Found preferred resolution; setting selection to it");
              cbo->setItemSelectState(cbo->getItemCount() - 1, true);
            }
          }

          cbo->handleUpdatedListItemData();
        }
      }
    }



    bool onComboValueChange(const CEGUI::EventArgs& evt)
    {
      const CEGUI::MouseEventArgs& we =static_cast<const CEGUI::MouseEventArgs&>(evt);
      CEGUI::Combobox* cbo = static_cast<CEGUI::Combobox*>(we.window);

      if (cbo == 0)
      {
        return false;
      }

      CEGUI::ListboxItem* item = cbo->getSelectedItem();

      if (item == 0)
      {
        return false;
      }

      if (cbo->getName() == "VideoDriverBox")
      {
        loadResolutionOptionsForSelectedRenderer();
      }


      return true;
    }

    bool onButtonQuitClicked(const CEGUI::EventArgs& evt)
    {
      ptrToOwner_->quitApp();
      return true;
    }

    bool onButtonOptCancelClicked(const CEGUI::EventArgs& evt)
    {
      // hide
      showOptions(false);
      return true;
    }

    bool onButtonOptOkClicked (const CEGUI::EventArgs& evt)
    {
      // hide
      showOptions(false);

      ptrToOwner_->saveConfiguration();

      return true;
    }

    bool onButtonNewGameClicked(const CEGUI::EventArgs& evt)
    {
      return true;
    }

    bool onButtonOptionsClicked(const CEGUI::EventArgs& evt)
    {
      showOptions(true);
      return true;
    }

    void showOptions(bool onOffFlag)
    {
      CEGUI::FrameWindow* options = static_cast<CEGUI::FrameWindow*>(utils::CEGUIHelperUtil::getWindow("OptionsWindow"));

      if (0 == options)
      {
        return;
      }

      if (onOffFlag)
      {
        options->setVisible(true);
        options->setAlwaysOnTop(true);
      }
      else
      {
        options->setVisible(false);
        options->setAlwaysOnTop(false);
      }
    }
  };

  //
  // The Menu state implementation.
  //


  /**
    Constructor.
  */
  MenuState::MenuState(
    const char* name,
    app::SerpEngine* enginePtr)
    : SerpState(name, enginePtr),
      initialized_(false),
      guiInternals_(new MenuState::GUIInternals(this))
  {
    scene_ = new Scene(enginePtr);
  }

  /**
    Destructor.
  */
  MenuState::~MenuState()
  {
    if (NULL != guiInternals_)
    {
      delete guiInternals_;
      guiInternals_ = NULL;
    }
  }

  /**
    Save engine configuration.
  */
  void MenuState::saveConfiguration()
  {
    app::ResolutionConfig resCfg = app::SerpentsGameHelper::getPtr()->getPreferredResolutionConfig();

    // store the options into the helper and into the config.
    CEGUI::Combobox* cbo = static_cast<CEGUI::Combobox*>(utils::CEGUIHelperUtil::getWindow("VideoDriverBox"));
    if (cbo)
    {
      std::string currentEngine = cbo->getSelectedItem()->getText().c_str();
      std::string storageRenderer = app::SerpentsGameHelper::getPtr()->getPseudoNameForDriverName(currentEngine);

      enginePtr_->getConfig() ["video"]["preferredEngine"].setAsString(storageRenderer);
      app::SerpentsGameHelper::getPtr()->setPreferredRendererName(currentEngine);

    }

    cbo = static_cast<CEGUI::Combobox*>(utils::CEGUIHelperUtil::getWindow("DisplayModeBox"));
    if (cbo)
    {
      int mySelectionIndex = cbo->getItemIndex(cbo->getSelectedItem());
      app::DisplayModes prefMode = static_cast <app::DisplayModes>(mySelectionIndex);

      enginePtr_->getConfig() ["video"]["displayMode"].setAsInt(mySelectionIndex);
      app::SerpentsGameHelper::getPtr()->setPreferredDisplayMode(prefMode);
    }

    cbo = static_cast<CEGUI::Combobox*>(utils::CEGUIHelperUtil::getWindow("ResolutionBox"));
    if (cbo)
    {
      std::string currentRes = cbo->getSelectedItem()->getText().c_str();

      app::ResolutionConfig subConfig =
        app::SerpentsGameHelper::getPtr()->getPartialConfigFromResolutionString(currentRes);

      enginePtr_->getConfig() ["video"]["window.width"].setAsInt(subConfig.width);
      enginePtr_->getConfig() ["video"]["window.height"].setAsInt(subConfig.height);
      if (subConfig.depth != 0)
      {
        enginePtr_->getConfig() ["video"]["colourDepth"].setAsInt(subConfig.depth);
        resCfg.depth = subConfig.depth;
      }

      resCfg.width = subConfig.width;
      resCfg.height = subConfig.height;
    }

    app::SerpentsGameHelper::getPtr()->setPreferredResolutionConfig(resCfg);

    // save.
    GTRACE(3, "Telling the engine to save the config...");
    enginePtr_->getConfig().save();
  }

  // The overrides for the state handling
  void MenuState::enter(const app::SerpStateParams& params)
  {
    GTRACE(4, "Entering main menu state...");

    enteringParams = params;
  }

  void MenuState::exit()
  {
    enginePtr_->shutDown();
    enteringParams.getConfig().clear();
  }

  void MenuState::pause()
  {
  }


  void MenuState::resume()
  {
  }

  void MenuState::reactToKeyPress(OIS::KeyCode keycode)
  {
  }

  void MenuState::reactToKeyRelease(OIS::KeyCode keycode)
  {
  }


  void MenuState::reactToMouseMoved(int x, int y)
  {
  }

  void MenuState::reactToMousePressed(const OIS::MouseButtonID& btn, int x, int y)
  {
  }

  void MenuState::reactToMouseReleased(const OIS::MouseButtonID& btn, int x, int y)
  {
  }

  bool MenuState::handleLogicThreadLoop()
  {
    GTRACE(5, "MenuState::handleLogicThreadLoop");

    return true;
  }


  bool MenuState::initialize()
  {
    GTRACE(5, "MenuState::initialize");

    // Load the resources.
    std::string menuGroup = "Menu"; // TODO: remove hardcoding.

    std::vector<std::string> resList;
    resList.push_back("data/levels/Beach_1");
    app::SerpentsGameHelper::getPtr()->addResourceLocationList(resList, menuGroup);
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(menuGroup);
    Ogre::ResourceGroupManager::getSingleton().loadResourceGroup(menuGroup);

    // Use a skybox if required. Loaded from the level.
    std::string sTemp = "SBCloudySkies"; // GBaseAppSettings::getPtr()->level.lvl.skyBoxName;
    double dTemp = 500;  // GBaseAppSettings::getPtr()->level.lvl.skyBoxDist;
    GTRACE(4, "Loading skybox...:" << sTemp);

    // TODO: Add scene helper to manage entire scene and allow the use of a skybox.
    // The scene manager is not used for anything else at the moment.
    if (sTemp != "")
    {
      scene_->setSkyBoxData(
        true,               //  enable
        sTemp.c_str(),      //  material
        dTemp,              //  dist
        true,               //  draw first
        menuGroup.c_str()); //  resource group.
    }

    GTRACE(4, "Setting shadow options...");

    // 2D stuff.

    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("General");
    Ogre::ResourceGroupManager::getSingleton().loadResourceGroup("General");

    CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();

    CEGUI::System* ceguiSysPtr_ = CEGUI::System::getSingletonPtr();

    CEGUI::Window* mainMenuSheet  = win_mgr.loadLayoutFromFile("MainMenu.layout");

    // install this as the root GUI sheet
    ceguiSysPtr_->getDefaultGUIContext().setRootWindow(mainMenuSheet);
    guiInternals_->optionsWindow_ = (CEGUI::FrameWindow*) win_mgr.loadLayoutFromFile("Options.layout");
    guiInternals_->optionsWindow_->setAlwaysOnTop(true);
    mainMenuSheet->addChild(guiInternals_->optionsWindow_);

#if 0
    CEGUI::PushButton* btn;
    btn = static_cast<CEGUI::PushButton*>(utils::CEGUIHelperUtil::getWindow("ButtonExit"));
    if (btn)
    {
      btn->setWantsMultiClickEvents(false);
      btn->subscribeEvent(
        CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onButtonQuitClicked, this));
    }

    btn = static_cast<CEGUI::PushButton*>(utils::CEGUIHelperUtil::getWindow("ButtonOptions"));
    if (btn)
    {
      btn->setWantsMultiClickEvents(false);
      btn->subscribeEvent(
        CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onButtonOptionsClicked, this));
    }

    btn = static_cast<CEGUI::PushButton*>(utils::CEGUIHelperUtil::getWindow("ButtonQuickGame"));
    if (btn)
    {
      btn->setWantsMultiClickEvents(false);
      btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onButtonNewGameClicked, this));
    }

    btn = static_cast<CEGUI::PushButton*>(utils::CEGUIHelperUtil::getWindow("BtnOptCancel"));
    if (btn)
    {
      btn->setWantsMultiClickEvents(false);
      btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onButtonOptCancelClicked, this));
    }

    btn = static_cast<CEGUI::PushButton*>(utils::CEGUIHelperUtil::getWindow("BtnOptOk"));
    if (btn)
    {
      btn->setWantsMultiClickEvents(false);
      btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MenuState::onButtonOptOkClicked, this));
    }
#endif

    CEGUI::Combobox* cbo;
    cbo = static_cast<CEGUI::Combobox*>(utils::CEGUIHelperUtil::getWindow("VideoDriverBox"));
    if (cbo)
    {
      // Make the combo read-only, so it's only a drop-down list.
      cbo->setReadOnly(true);

      std::string prefRenderer = app::SerpentsGameHelper::getPtr()->getPreferredRendererName();
      GTRACE(4, "Will try to set the combo-box to the preferred renderer: " << prefRenderer);
      // Add the entries: the available drivers.
      std::vector <std::string> vidDrivers = app::SerpentsGameHelper::getPtr()->getEditableAvailableVideoDrivers();
      for (std::vector <std::string>::iterator it = vidDrivers.begin();
        it != vidDrivers.end(); ++ it)
      {
        GTRACE(4, "Adding a video driver: " << *it);
        CEGUI::ListboxItem* vidItem = new utils::SimpleListItem((CEGUI::utf8*) it->c_str()
                , "GlossySerpent", "ComboboxSelectionBrush", "");
        cbo->addItem(vidItem);
        // Select the current driver.
        if (*it == prefRenderer)
        {
          GTRACE(4, "Found preferred renderer; setting selection to it");
          cbo->setItemSelectState(cbo->getItemCount() - 1, true);
        }
      }

      cbo->handleUpdatedListItemData();
      cbo->subscribeEvent(
        CEGUI::Combobox::EventListSelectionAccepted,
        CEGUI::Event::Subscriber(&MenuState::GUIInternals::onComboValueChange, this->guiInternals_));
    }


    cbo = static_cast<CEGUI::Combobox*>(utils::CEGUIHelperUtil::getWindow("DisplayModeBox"));
    if (cbo)
    {
      // Make the combo read-only, so it's only a drop-down list.
      cbo->setReadOnly(true);

      app::DisplayModes prefMode = app::SerpentsGameHelper::getPtr()->getPreferredDisplayMode();

      // Add the entries
      // For each entry, check to see if it's the active one and select it if so.
      cbo->addItem(new utils::SimpleListItem((CEGUI::utf8*) app::SerpentsGameHelper::getPtr()->
        getDescriptionForDisplayMode(app::FULLSCREEN_EXCLUSIVE).c_str()
        , "GlossySerpent", "ComboboxSelectionBrush", ""));
      if (prefMode == app::FULLSCREEN_EXCLUSIVE)
      {
        cbo->setItemSelectState(cbo->getItemCount() - 1, true);
      }

      cbo->addItem(new utils::SimpleListItem((CEGUI::utf8*) app::SerpentsGameHelper::getPtr()->
        getDescriptionForDisplayMode(app::WINDOWED_WITH_BORDER).c_str()
        , "GlossySerpent", "ComboboxSelectionBrush", ""));
      if (prefMode == app::WINDOWED_WITH_BORDER)
      {
        cbo->setItemSelectState(cbo->getItemCount() - 1, true);
      }

      if (app::SerpentsGameHelper::getPtr()->hasFeature("non-exclusive-fullscreen"))
      {
        cbo->addItem(new utils::SimpleListItem((CEGUI::utf8*) app::SerpentsGameHelper::getPtr()->
          getDescriptionForDisplayMode(app::WINDOWED_BORDERLESS_FULL_SIZE).c_str()
          , "GlossySerpent", "ComboboxSelectionBrush", ""));
        if (prefMode == app::WINDOWED_BORDERLESS_FULL_SIZE)
        {
          cbo->setItemSelectState(cbo->getItemCount() - 1, true);
        }
      }
    }

    cbo = static_cast<CEGUI::Combobox*>(utils::CEGUIHelperUtil::getWindow("ResolutionBox"));
    if (cbo)
    {
      // Make the combo read-only, so it's only a drop-down list.
      cbo->setReadOnly(true);

      // load the list of options and select the current one (or nearest one).
      guiInternals_->loadResolutionOptionsForSelectedRenderer();
    }


    initialized_ = true;
    return true;
  }

  bool MenuState::renderScene()
  {
    if (! initialized_)
    {
      return initialize();
    }

    return true;
  }

  bool MenuState::renderGUI()
  {
    return true;
  }

  void MenuState::notifyOfShutdown()
  {
  }


  void MenuState::quitApp()
  {
    exit();
  }


  void MenuState::newGame()
  {
  }








}
