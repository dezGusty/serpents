//    This file is part of Gusty's Serpents
//    Copyright(C) 2009  Augustin Preda(thegusty999@gmail.com)
//
//    Gusty's Serpents is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

//
// Includes
//

//
// Own header
//
#include "impl/workbenchstate.h"

//
// C++ system files
//
#include <map>
#include <string>
#include <vector>

//
// Other libraries' headers
//

#include "guslib/system/random.h"
#include "guslib/system/apprestart.h"
#include "guslib/trace/trace.h"
#include "guslib/util/filehelper.h"
#include "guslib/util/stringutil.h"

#include "gussound.h"

//
// This project's headers
//

#include "app/SerpEngine.h"
#include "app/SerpFrameListener.h"

#include "engine/GusCamera.h"

#include "app/SerpentsGameHelper.h"
// Main sound header.

#include "utils/CEGUIUtils.h"

namespace Serpents
{
  WorkbenchState::WorkbenchState(const std::string& name, const std::string& prefLvl, app::SerpEngine* ptr)
    : SerpState(name, ptr),
      initialized_(false),
      canRotate_(false),
      oldMouseX_(-1),
      oldMouseY_(-1),
      movingCameraForward_(false),
      movingCameraBack_(false),
      movingCameraLeft_(false),
      movingCameraRight_(false),
      movingCameraFaster_(false),
      movingCameraSlower_(false),
      reloadGroupNextTick_(""),
      staticGroupName_("WbenchStatic"),
      expGroupName_("WbenchExperiment"),
      shadowTechMinimum_(0),
      shadowTechMaximum_(6),
      currentShadowTech_(0),
      applyShadowTechNextTick_(false),
      rendererIsOpenGL_(false),
      currentCameraSetup_(0),
      applyCameraSetupNextTick_(false),
      cameraSetupMinimum_(0),
      cameraSetupMaximum_(4),
      mPlane(0),
      mContextMenuPtr(0),
      preferredLevel_(prefLvl),
      preferredSkin_(""),
      compositorsAreEnabled_(0),
      m_secondaryViewActive(0),
      m_preferredSkinConfig("")
  {
  }


  // The overrides for the state handling
  void WorkbenchState::enter(const app::SerpStateParams& param)
  {
    GTRACE(3, "Entering workbench state. Loading sounds");
    std::string data_folder = enginePtr_->getConfig()["_auto_"]["dataFolder"].getAsString();

    std::string hover_sound = enginePtr_->getConfig()["sounds_ui"]["menu_hover"].getAsString();
    std::string click_sound = enginePtr_->getConfig()["sounds_ui"]["menu_click"].getAsString();

    if (hover_sound.length() > 0)
    {
      std::string file_name = data_folder + hover_sound;
      try
      {
        gussound::SoundManager::getPtr()->getRepository()->addSound(
            "menu_hover", file_name,
            true, gussound::GSC_Effect);
      }
      catch (const std::exception& e)
      {
        GTRACE(1, "Caught exception: " << e.what());
      }
    }

    if (hover_sound.length() > 0)
    {
      std::string file_name = data_folder + click_sound;
      try
      {
        gussound::SoundManager::getPtr()->getRepository()->addSound(
            "menu_click", file_name,
            true, gussound::GSC_Effect);
      }
      catch (const std::exception& e)
      {
        GTRACE(1, "Caught exception: " << e.what());
      }
    }

    GTRACE(3, "Entered workbench state. Loaded sounds");
  }


  void WorkbenchState::exit()
  {
    removeSceneCompositors();

    std::vector<std::string> compositorList = levelConfig_["lighting"]["applyCompositorToShadow"].getAsStringList();
    const unsigned numShadowRTTs = enginePtr_->getSceneManagerPtr()->getShadowTextureCount();
    GTRACE(4, "Removing " << compositorList.size() << " compositors from the "
        << numShadowRTTs << " shadow texture viewports");

    for (unsigned i = 0; i < numShadowRTTs; ++i)
    {
      Ogre::TexturePtr tex = enginePtr_->getSceneManagerPtr()->getShadowTexture(i);
      Ogre::Viewport *vp = tex->getBuffer()->getRenderTarget()->getViewport(0);
      for (std::vector<std::string>::iterator it = compositorList.begin(); it != compositorList.end(); ++it)
      {
        try
        {
          Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, *it, true);
          Ogre::CompositorManager::getSingleton().removeCompositor(vp, *it);
        }
        catch (const std::exception & e)
        {
          GTRACE(1, "Caught exception: " << e.what());
        }
      }
      Ogre::CompositorManager::getSingletonPtr()->removeCompositorChain(vp);
    }
    enginePtr_->shutDown();
  }

  void WorkbenchState::pause()
  {
  }


  void WorkbenchState::resume()
  {
  }

  void WorkbenchState::reactToKeyPress(OIS::KeyCode keycode)
  {
    GTRACE(5, "WorkbenchState::reactToKeyPress: " << keycode);
    if (keycode == OIS::KC_EQUALS)
    {
      // +
      if (playerCamera_)
      {
        playerCamera_->setZoomLevel(playerCamera_->getZoomLevel() + playerCamera_->getZoomStep());
      }
    }
    if (keycode == OIS::KC_MINUS)
    {
      // -
      if (playerCamera_)
      {
        playerCamera_->setZoomLevel(playerCamera_->getZoomLevel() - playerCamera_->getZoomStep());
      }
    }
    if (keycode == OIS::KC_W)
    {
      movingCameraForward_ = true;
      GTRACE(4, "Moving fwd:" << movingCameraForward_);
    }
    if (keycode == OIS::KC_LSHIFT)
    {
      movingCameraSlower_ = true;
    }
    if (keycode == OIS::KC_LCONTROL)
    {
      movingCameraFaster_ = true;
    }
    if (keycode == OIS::KC_S)
    {
      movingCameraBack_ = true;
      GTRACE(4, "Moving back:" << movingCameraBack_);
    }
    if (keycode == OIS::KC_A)
    {
      movingCameraLeft_ = true;
    }
    if (keycode == OIS::KC_D)
    {
      movingCameraRight_ = true;
    }
    if (keycode == OIS::KC_R)
    {
      reloadGroupNextTick_ = expGroupName_;
    }

    if (keycode == OIS::KC_SPACE)
    {
      // show some debug info; write it to the disk atm.
      GTRACE(2, "Player camera info -----------------------");
      Ogre::Vector3 pos = playerCamera_->getCamera()->getPosition();
      GTRACE(2, "Position: " << pos.x << ", " << pos.y << ", " << pos.z);
      GTRACE(4, "\npos.x = " << pos.x << "\npos.y = " << pos.y << "\npos.z = " << pos.z);
      Ogre::Vector3 dir = playerCamera_->getCamera()->getDirection();
      GTRACE(2, "Dir: " << dir.x << ", " << dir.y << ", " << dir.z);
      GTRACE(4, "\ndir.x = " << dir.x << "\ndir.y = " << dir.y << "\ndir.z = " << dir.z);
      Ogre::Vector3 lookAt = pos - dir;
      GTRACE(2, "Look at: " << lookAt.x << ", " << lookAt.y << ", " << lookAt.z);

      // also save a screenshot
      if (m_secondaryViewActive)
      {
        Ogre::Resource* res= Ogre::TextureManager::getSingleton().createOrRetrieve(
            "secondary_view_render_texture",
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME).first.getPointer();
        if (0 == res)
        {
          return;
        }

        Ogre::Texture* texture = static_cast<Ogre::Texture*>(res);
        Ogre::RenderTarget* target = texture->getBuffer()->getRenderTarget();

        target->update();

        Ogre::Image img;
        texture->convertToImage(img);
        std::string saveFile = "test001.png";
        if (saveFile != "")
        {
          img.resize(256, 256);
          img.save(saveFile);
          GTRACE(4, "Saved a temporary file: " << saveFile);
        }
        // end test save
      }
    }
    if (keycode == OIS::KC_L)
    {
      moveToNextShadowTechnique();
    }
    if (keycode == OIS::KC_K)
    {
      moveToPrevShadowTechnique();
    }
    if (keycode == OIS::KC_M)
    {
      moveToNextCameraSetup();
    }
    if (keycode == OIS::KC_N)
    {
      moveToPrevCameraSetup();
    }

    if (keycode == OIS::KC_I)
    {
      compositorsAreEnabled_ = !compositorsAreEnabled_;
      setCompositorEnabledStatus(compositorsAreEnabled_);
    }
  }

  void WorkbenchState::reactToKeyRelease(OIS::KeyCode keycode)
  {
    if (keycode == OIS::KC_W)
    {
      movingCameraForward_ = false;
      GTRACE(4, "Moving fwd:" << movingCameraForward_);
    }
    if (keycode == OIS::KC_S)
    {
      movingCameraBack_ = false;
      GTRACE(4, "Moving back:" << movingCameraBack_);
    }
    if (keycode == OIS::KC_LSHIFT)
    {
      movingCameraSlower_ = false;
    }
    if (keycode == OIS::KC_LCONTROL)
    {
      movingCameraFaster_ = false;
    }
    if (keycode == OIS::KC_A)
    {
      movingCameraLeft_ = false;
    }
    if (keycode == OIS::KC_D)
    {
      movingCameraRight_ = false;
    }
  }

  void WorkbenchState::reactToMouseMoved(int x, int y)
  {
    if (canRotate_)
    {
      if (playerCamera_)
      {
        playerCamera_->getCamera()->yaw(Ogre::Degree((oldMouseX_ - x) *0.15));
        playerCamera_->getCamera()->pitch(Ogre::Degree((oldMouseY_ - y) *0.15));
      }
    }
    oldMouseX_ = x;
    oldMouseY_ = y;
  }

  void WorkbenchState::reactToMousePressed(const OIS::MouseButtonID& btn, int x, int y)
  {
    if (btn == OIS::MB_Right)
    {
      canRotate_ = true;
      GTRACE(5, "Rotating enabled");
    }
    /*else if (btn == OIS::MB_Left)
    {
      if (mContextMenuPtr && mContextMenuPtr->isVisible())
      {
        CEGUI::URect rectangle = mContextMenuPtr->getArea();
        if (x < rectangle.left().d_offset
          || x > rectangle.right().d_offset
          || y < rectangle.top().d_offset
          || y > rectangle.bottom().d_offset)
        {
          mContextMenuPtr->setVisible(false);

          std::string parentWndName = utils::CEGUIHelperUtil::getNameForWidget("MainMenuRootItem");
          CEGUI::Window* parentWindow = utils::CEGUIHelperUtil::getWindow(parentWndName);
          if (parentWindow)
          {
            if (parentWindow->isChild(mContextMenuPtr))
            {
              utils::CEGUIHelperUtil::removeChildWidget(parentWindow, mContextMenuPtr);
              mContextMenuPtr->setVisible(false);
            }
          }
        }
      }
    }*/
  }

  void WorkbenchState::reactToMouseReleased(const OIS::MouseButtonID& btn, int x, int y)
  {
    if (btn == OIS::MB_Right)
    {
      canRotate_ = false;
      GTRACE(5, "Rotating disabled");
    }
  }

  bool WorkbenchState::handleLogicThreadLoop()
  {
    GTRACE(5, "WorkbenchState::preRender");

    return true;
  }


  void WorkbenchState::addCompositorByName(const std::string& name, Ogre::Viewport* vp)
  {
    GTRACE(5, "Adding compositor [" << name << "](or at least trying to)");
    try
    {
      Ogre::CompositorManager::getSingleton().addCompositor(vp, name.c_str());
      Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, name.c_str(), true);
    }
    catch (const std::exception & e)
    {
      GTRACE(1, "Caught exception: " << e.what());
    }
  }

  void WorkbenchState::removeCompositorByName(const std::string& name, Ogre::Viewport* vp)
  {
    GTRACE(5, "Removing compositor [" << name << "]");
    try
    {
      Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, name.c_str(), false);
      Ogre::CompositorManager::getSingleton().removeCompositor(vp, name.c_str());
    }
    catch (const std::exception & e)
    {
      GTRACE(1, "Caught exception: " << e.what());
    }
  }

  void WorkbenchState::addSceneCompositors()
  {
    std::vector<std::string> sceneCompositorList = levelConfig_["general"]["applyCompositorToScene"].getAsStringList();
    GTRACE(4, "Applying " << sceneCompositorList.size() << " compositors to the scene viewports");
    {
      for (std::vector<std::string>::iterator it = sceneCompositorList.begin(); it != sceneCompositorList.end(); ++it)
      {
        addCompositorByName(*it, playerCamera_->getCamera()->getViewport());
      }
    }

    std::vector<std::string> secondarySceneCompositorList =
        levelConfig_["general"]["applyCompositorToSecondaryView"].getAsStringList();

    GTRACE(4, "Applying " << secondarySceneCompositorList.size() << " compositors to the secondary scene viewports");
    {
      for (std::vector<std::string>::iterator it = secondarySceneCompositorList.begin();
          it != secondarySceneCompositorList.end(); ++it)
      {
        addCompositorByName(*it, enginePtr_->getSecondaryCamera()->getViewport());
      }
    }
  }

  void WorkbenchState::removeSceneCompositors()
  {
    std::vector<std::string> sceneCompositorList = levelConfig_["general"]["applyCompositorToScene"].getAsStringList();
    for (std::vector<std::string>::iterator it = sceneCompositorList.begin(); it != sceneCompositorList.end(); ++it)
    {
      removeCompositorByName(*it, playerCamera_->getCamera()->getViewport());
    }

    Ogre::CompositorManager::getSingletonPtr()->removeCompositorChain(playerCamera_->getCamera()->getViewport());

    std::vector<std::string> secondarySceneCompositorList =
      levelConfig_["general"]["applyCompositorToSecondaryView"].getAsStringList();

    for (std::vector<std::string>::iterator it = secondarySceneCompositorList.begin();
        it != secondarySceneCompositorList.end(); ++it)
    {
      removeCompositorByName(*it, enginePtr_->getSecondaryCamera()->getViewport());
    }

    Ogre::CompositorManager::getSingletonPtr()->removeCompositorChain(enginePtr_->getSecondaryCamera()->getViewport());
  }


  bool WorkbenchState::initialize()
  {
    GTRACE(5, "WorkbenchState::initialize");

    std::string levelToLoad("data/levels/Workbench_1.sif");

    if (preferredLevel_.length() > 0)
    {
      levelToLoad = preferredLevel_;
    }

    std::string dataFolder = enginePtr_->getConfig()["_auto_"]["dataFolder"].getAsString();
    levelToLoad = dataFolder + levelToLoad;

    // Load the resources.
    levelConfig_.load(levelToLoad);

    std::vector<std::string> resList;
    resList = levelConfig_["general"]["levelFolder"].getAsStringList();
    for (std::vector <std::string>::iterator it = resList.begin(); it != resList.end(); ++it)
    {
      std::string temp(dataFolder);
      temp.append(*it);
      *it = temp;
    }

    // Create the two resource groups
    Ogre::ResourceGroupManager::getSingleton().createResourceGroup(staticGroupName_);
    Ogre::ResourceGroupManager::getSingleton().createResourceGroup(expGroupName_);

    app::SerpentsGameHelper::getPtr()->addResourceLocationList(resList, staticGroupName_);

    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(staticGroupName_);
    Ogre::ResourceGroupManager::getSingleton().loadResourceGroup(staticGroupName_);

    resList.clear();
    resList = levelConfig_["general"]["levelFolderExp"].getAsStringList();
    for (std::vector <std::string>::iterator it = resList.begin(); it != resList.end(); ++it)
    {
      std::string temp(dataFolder);
      temp.append(*it);
      *it = temp;
    }

    app::SerpentsGameHelper::getPtr()->addResourceLocationList(resList, expGroupName_);

    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(expGroupName_);
    Ogre::ResourceGroupManager::getSingleton().loadResourceGroup(expGroupName_);

    double x, y, z, a;

    // Set the ambient color.
    x = levelConfig_["lighting"]["ambientColor.x"].getAsDoubleOrDefaultVal(1.0);
    y = levelConfig_["lighting"]["ambientColor.y"].getAsDoubleOrDefaultVal(1.0);
    z = levelConfig_["lighting"]["ambientColor.z"].getAsDoubleOrDefaultVal(1.0);
    enginePtr_->getSceneManagerPtr()->setAmbientLight(Ogre::ColourValue(x, y, z));
    GTRACE(4, "Applied ambient light : " << x << ", " << y << ", " << z);


    GTRACE(4, "Setting shadow options...");

    rendererIsOpenGL_ = Ogre::Root::getSingleton().getRenderSystem()->getName().find("GL") != Ogre::String::npos;

    int shadowType = levelConfig_["lighting"]["shadowModeOverride"].getAsInt();


    int shadowDist = levelConfig_["lighting"]["shadowFarDistOverride"].getAsInt();
    enginePtr_->getSceneManagerPtr()->setShadowFarDistance(shadowDist);

    x = levelConfig_["lighting"]["shadowColor.x"].getAsDouble();
    y = levelConfig_["lighting"]["shadowColor.y"].getAsDouble();
    z = levelConfig_["lighting"]["shadowColor.z"].getAsDouble();
    a = levelConfig_["lighting"]["shadowColorAlpha"].getAsDouble();
    enginePtr_->getSceneManagerPtr()->setShadowColour(Ogre::ColourValue(x, y, z, a));

    currentShadowTech_ = shadowType;

    // Create the lights
    createLights();

    bool showDebugShadows = levelConfig_["lighting"]["showDebugShadows"].getAsBoolOrDefault(false);
    enginePtr_->getSceneManagerPtr()->setShowDebugShadows(showDebugShadows);

    // Set the shadow technique before creating the lights.
    applyShadowTechNextTick_ = true;
    applyCameraSetupNextTick_ = true;

    // Create objects; after the lights
    createLevelObjects();

    applySkyBox();

    createCameras();

    double shadowDirLightTextureOffset =
        levelConfig_["lighting"]["shadowDirLightTextureOffset"].getAsDoubleOrDefaultVal(0);

    if (shadowDirLightTextureOffset != 0)
    {
      enginePtr_->getSceneManagerPtr()->setShadowDirLightTextureOffset(shadowDirLightTextureOffset);
    }

    if (Ogre::Root::getSingletonPtr()->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_HWRENDER_TO_TEXTURE))
    {
      int shadowTextureSizeWidth = levelConfig_["lighting"]["shadowTextureSizeWidth"].getAsIntOrDefaultVal(0);
      if (shadowTextureSizeWidth > 0)
      {
        enginePtr_->getSceneManagerPtr()->setShadowTextureSettings(shadowTextureSizeWidth, 2);
      }
    }

    compositorsAreEnabled_ = true;
    addSceneCompositors();

    int nShadowCameraPreferred = levelConfig_["camera"]["shadowCamera"].getAsIntOrDefaultVal(0);
    currentCameraSetup_ = nShadowCameraPreferred;

    // ------------------ init GUI ----------------------------

    GTRACE(3, "Loading CEGUI Layout...");
    // ------------ temporary code begin

    CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();

    CEGUI::System* ceguiSysPtr_ = CEGUI::System::getSingletonPtr();

    std::string layoutFile;
    std::string optionsLayoutFile;
    std::string defaultTooltip;
    std::string defaultCursorSkin;
    std::string defaultCursorName;
    std::string defaultBrushSkin;
    std::string defaultBrushName;
    std::string defaultBrushColour;

    std::string skinConfiguration;
    std::string globalSkinConfiguration;

    try
    {
      globalSkinConfiguration = enginePtr_->getConfig()["ui"]["skinconfig"].getAsStringOrDefaultVal("GlossySerpent");
      skinConfiguration = levelConfig_["gui_0.8"]["skinconfig"].getAsStringOrDefaultVal(globalSkinConfiguration);
      m_preferredSkinConfig = skinConfiguration;

      layoutFile = levelConfig_[skinConfiguration]["layout"].getAsStringOrDefaultVal("Workbench_0.8.layout");
      optionsLayoutFile =
          levelConfig_[skinConfiguration]["optionslayout"].getAsStringOrDefaultVal("workbenchoptions.layout");
      defaultTooltip = levelConfig_[skinConfiguration]["defaultTooltip"].getAsString();
      defaultCursorSkin = levelConfig_[skinConfiguration]["defaultCursorSkin"].getAsString();
      defaultCursorName = levelConfig_[skinConfiguration]["defaultCursor"].getAsString();
      defaultBrushSkin = levelConfig_[skinConfiguration]["defaultBrushSkin"].getAsString();
      defaultBrushName = levelConfig_[skinConfiguration]["defaultBrush"].getAsString();
      preferredSkin_ = levelConfig_[skinConfiguration]["preferredSkin"].getAsString();
      defaultBrushColour = levelConfig_[skinConfiguration]["defaultSelectionColour"].getAsString();

      std::stringstream ssCursor;
      ssCursor << defaultCursorSkin << "/" << defaultCursorName;

      GTRACE(4, "Preparing the mouse cursor for use: " << ssCursor.str());

      CEGUI::Window* myFrame= win_mgr.loadLayoutFromFile(layoutFile);
      ceguiSysPtr_->getDefaultGUIContext().setRootWindow(myFrame);
      ceguiSysPtr_->getDefaultGUIContext().getMouseCursor().setDefaultImage(ssCursor.str());
      ceguiSysPtr_->getDefaultGUIContext().getMouseCursor().setImage(ssCursor.str());

      if (defaultTooltip.length() > 0)
      {
        ceguiSysPtr_->getDefaultGUIContext().setDefaultTooltipType(defaultTooltip);
      }

      CEGUI::Tooltip* defaultTT = CEGUI::System::getSingletonPtr()->getDefaultGUIContext().getDefaultTooltipObject();
      GTRACE(4, "Default tooltip is: " << defaultTT->getNamePath().c_str());

      defaultTT->activate();
      GTRACE(4, "Loaded: " << ssCursor.str());

      ceguiSysPtr_->getDefaultGUIContext().markAsDirty();
      myFrame->setMouseCursor(ssCursor.str());
    }
    catch (const std::exception& e)
    {
      GTRACE(2, "Caught exception: " << e.what());
    }

    GTRACE(4, "Loaded CEGUI Layout... Customizing ... ");

    std::string btnName1 = utils::CEGUIHelperUtil::getNameForWidget("SampleWideButton1");
    addWidgetMouseEffects(btnName1);

    CEGUI::PushButton* btn;
    btn = static_cast<CEGUI::PushButton*>(utils::CEGUIHelperUtil::getWindow(btnName1));
    if (btn)
    {
      btn->setEnabled(false);
    }
    else
    {
      GTRACE(2, "Object not found: " << btnName1);
    }

    btnName1 = utils::CEGUIHelperUtil::getNameForWidget("TopAreaDummy/ButtonQuit");
    addWidgetMouseEffects(btnName1);

    btn = static_cast<CEGUI::PushButton*>(utils::CEGUIHelperUtil::getWindow(btnName1));
    if (btn)
    {
      btn->setWantsMultiClickEvents(false);
      btn->subscribeEvent(
          CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&WorkbenchState::onButtonQuitClicked, this));
      GTRACE(3, "subscribed to click events for " << btnName1 << ".");
    }
    else
    {
      GTRACE(2, "Object not found: " << btnName1);
    }

    btnName1 = utils::CEGUIHelperUtil::getNameForWidget("TopAreaDummy/ButtonOptions");
    addWidgetMouseEffects(btnName1);
    btn = static_cast<CEGUI::PushButton*>(utils::CEGUIHelperUtil::getWindow(btnName1));
    if (btn)
    {
      btn->setWantsMultiClickEvents(false);
      btn->subscribeEvent(
          CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&WorkbenchState::onButtonOptionsClicked, this));
      GTRACE(3, "subscribed to click events for " << btnName1 << ".");
    }
    else
    {
      GTRACE(2, "Object not found: " << btnName1);
    }

    btnName1 = utils::CEGUIHelperUtil::getNameForWidget("TopAreaDummy/ButtonSubMenu");
    addWidgetMouseEffects(btnName1);

    btn = static_cast<CEGUI::PushButton*>(utils::CEGUIHelperUtil::getWindow(btnName1));
    if (btn)
    {
      btn->setWantsMultiClickEvents(false);
      btn->subscribeEvent(
          CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&WorkbenchState::onButtonSubmenuClicked, this));
      GTRACE(3, "subscribed to click events for " << btnName1 << ".");
    }
    else
    {
      GTRACE(2, "ERROR: Expected object not found: " << btnName1);
    }

    btnName1 = utils::CEGUIHelperUtil::getNameForWidget("BottomAreaDummy/ButtonReloadExperimentGroup");
    addWidgetMouseEffects(btnName1);
    btn = static_cast<CEGUI::PushButton*>(utils::CEGUIHelperUtil::getWindow(btnName1));
    if (btn)
    {
      btn->setWantsMultiClickEvents(false);
      btn->subscribeEvent(
          CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&WorkbenchState::onButtonReloadGroupClicked, this));
      GTRACE(3, "subscribed to click events for " << btnName1 << ".");
    }
    else
    {
      GTRACE(2, "Object not found: " << btnName1);
    }

    btnName1 = utils::CEGUIHelperUtil::getNameForWidget("BottomAreaDummy/ButtonPrevShadowTech");
    addWidgetMouseEffects(btnName1);
    btn = static_cast<CEGUI::PushButton*>(utils::CEGUIHelperUtil::getWindow(btnName1));
    if (btn)
    {
      btn->setWantsMultiClickEvents(false);
      btn->subscribeEvent(
          CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&WorkbenchState::onButtonPrevShadowTech, this));
      GTRACE(3, "subscribed to click events for " << btnName1 << ".");
    }
    else
    {
      GTRACE(2, "Object not found: " << btnName1);
    }

    btnName1 = utils::CEGUIHelperUtil::getNameForWidget("BottomAreaDummy/ButtonNextShadowTech");
    addWidgetMouseEffects(btnName1);
    btn = static_cast<CEGUI::PushButton*>(utils::CEGUIHelperUtil::getWindow(btnName1));
    if (btn)
    {
      btn->setWantsMultiClickEvents(false);
      btn->subscribeEvent(
          CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&WorkbenchState::onButtonNextShadowTech, this));
      GTRACE(3, "subscribed to click events for " << btnName1 << ".");
    }
    else
    {
      GTRACE(2, "Object not found: " << btnName1);
    }

    CEGUI::Window* tempWindow;

    // Update the level label
    btnName1 = utils::CEGUIHelperUtil::getNameForWidget("TopLeftFrame/LoadedBenchTextLabel");
    tempWindow = utils::CEGUIHelperUtil::getWindow(btnName1);
    if (tempWindow)
    {
      btnName1 = "Level : ";
      btnName1.append(preferredLevel_);
      tempWindow->setText((CEGUI::utf8*) btnName1.c_str());
    }

    btnName1 = utils::CEGUIHelperUtil::getNameForWidget("TopLeftFrame/Sample_ComboBox");
    addWidgetMouseEffects(btnName1);

    CEGUI::Combobox* cbo = static_cast<CEGUI::Combobox*>(utils::CEGUIHelperUtil::getWindow(btnName1));
    if (cbo)
    {
      populateHardCodedComboValues(cbo, defaultBrushSkin.c_str());
    }

    // Update the level label
    btnName1 = utils::CEGUIHelperUtil::getNameForWidget(
        "TopRightSelectionFrame/OptionsWindowTab/Sample_Tab_1/ButtonGetRandomProgressValue");

    addWidgetMouseEffects(btnName1);
    btn = static_cast<CEGUI::PushButton*>(utils::CEGUIHelperUtil::getWindow(btnName1));
    if (btn)
    {
      btn->setWantsMultiClickEvents(false);
      btn->subscribeEvent(
          CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&WorkbenchState::onButtonGetRandomValueClicked, this));
      GTRACE(3, "subscribed to click events for " << btnName1 << ".");
    }
    else
    {
      GTRACE(3, "could not find item named " << btnName1 << " for connection");
    }

    CEGUI::Scrollbar* scroller;
    btnName1 = utils::CEGUIHelperUtil::getNameForWidget(
        "TopRightSelectionFrame/OptionsWindowTab/Sample_Tab_1/Sample_ProgressSlider");

    addWidgetMouseEffects(btnName1);
    scroller = static_cast<CEGUI::Scrollbar*>(utils::CEGUIHelperUtil::getWindow(btnName1));
    // Update the level label
    if (scroller)
    {
      utils::CEGUIHelperUtil::setScrollPositionForWidget(btnName1, 0.5f);
      scroller->subscribeEvent(
          CEGUI::Scrollbar::EventScrollPositionChanged,
          CEGUI::Event::Subscriber(&WorkbenchState::onScrollValueChange, this));

      GTRACE(3, "subscribed to click events for " << btnName1 << ".");
    }
    else
    {
      GTRACE(3, "could not find item named " << btnName1 << " for connection");
    }

    btnName1 = utils::CEGUIHelperUtil::getNameForWidget(
        "TopRightSelectionFrame/OptionsWindowTab/Sample_Tab_2/SoundVolumeSlider");

    addWidgetMouseEffects(btnName1);
    scroller = static_cast<CEGUI::Scrollbar*>(utils::CEGUIHelperUtil::getWindow(btnName1));
    if (scroller)
    {
      utils::CEGUIHelperUtil::setScrollPositionForWidget(btnName1, 0.5f);
    }
    else
    {
      GTRACE(3, "could not find item named " << btnName1 << " for connection");
    }

    btnName1 = utils::CEGUIHelperUtil::getNameForWidget(
        "TopRightSelectionFrame/OptionsWindowTab/Sample_Tab_2/MusicVolumeSlider");

    addWidgetMouseEffects(btnName1);
    scroller = static_cast<CEGUI::Scrollbar*>(utils::CEGUIHelperUtil::getWindow(btnName1));
    if (scroller)
    {
      utils::CEGUIHelperUtil::setScrollPositionForWidget(btnName1, 0.5f);
    }
    else
    {
      GTRACE(3, "could not find item named " << btnName1 << " for connection");
    }

    // Set the tab control active pane
    btnName1 = utils::CEGUIHelperUtil::getNameForWidget("TopRightSelectionFrame/OptionsWindowTab");
    CEGUI::TabControl* tc = static_cast <CEGUI::TabControl*>(utils::CEGUIHelperUtil::getWindow(btnName1));
    if (tc)
    {
      if (tc->getTabCount() > 0)
      {
        tc->setSelectedTabAtIndex(0);
        //tc->invalidate(true);
        tc->invalidateRenderingSurface();
      }
    }
    else
    {
      GTRACE(3, "could not find item named " << btnName1 << " for connection");
    }


    CEGUI::RadioButton* rbutton;
    btnName1 = utils::CEGUIHelperUtil::getNameForWidget(
        "TopLeftFrame/SFX_RB_Grouper_1/__auto_contentpane__/SFX_RB_Radio1");

    addWidgetMouseEffects(btnName1);
    rbutton = static_cast<CEGUI::RadioButton*>(utils::CEGUIHelperUtil::getWindow(btnName1));
    if (rbutton)
    {
      rbutton->setGroupID(1);
      rbutton->setSelected(true);
    }
    else
    {
      GTRACE(3, "could not find item named " << btnName1 << " for connection");
    }

    btnName1 = utils::CEGUIHelperUtil::getNameForWidget(
        "TopLeftFrame/SFX_RB_Grouper_1/__auto_contentpane__/SFX_RB_Radio2");

    addWidgetMouseEffects(btnName1);
    rbutton = static_cast<CEGUI::RadioButton*>(utils::CEGUIHelperUtil::getWindow(btnName1));
    if (rbutton)
    {
      rbutton->setGroupID(1);
      rbutton->setSelected(false);
    }
    else
    {
      GTRACE(3, "could not find item named " << btnName1 << " for connection");
    }

    btnName1 = utils::CEGUIHelperUtil::getNameForWidget(
        "TopLeftFrame/SFX_RB_Grouper_1/__auto_contentpane__/SFX_RB_Radio3");

    addWidgetMouseEffects(btnName1);
    rbutton = static_cast<CEGUI::RadioButton*>(utils::CEGUIHelperUtil::getWindow(btnName1));
    if (rbutton)
    {
      rbutton->setGroupID(1);
      rbutton->setSelected(false);
    }
    else
    {
      GTRACE(3, "could not find item named " << btnName1 << " for connection");
    }


    CEGUI::Listbox * listbox;
    btnName1 = utils::CEGUIHelperUtil::getNameForWidget("TopRightSelectionFrame/SampleListBox_002");
    listbox= static_cast<CEGUI::Listbox*>(utils::CEGUIHelperUtil::getWindow(btnName1));
    if (listbox)
    {
      for (int i = 1; i <= 20; ++i)
      {
        std::string name("Item Added via code ");
        name.append(guslib::stringutil::IntToString(i));

        listbox->addItem(
            new utils::SimpleListItem(
                name, defaultBrushSkin, "ListboxSelectionBrush", CEGUI::Colour(0xFFFF0101), ""));
      }
    }
    else
    {
      GTRACE(3, "could not find item named " << btnName1 << " for connection");
    }

    CEGUI::ItemListbox* itemListBox;
    btnName1 = utils::CEGUIHelperUtil::getNameForWidget("TopRightSelectionFrame/SampleListBox_001");
    itemListBox = static_cast<CEGUI::ItemListbox*>(utils::CEGUIHelperUtil::getWindow(btnName1));
    if (itemListBox)
    {
      CEGUI::Scrollbar *sb;
      btnName1.append("/__auto_vscrollbar__");
      addWidgetMouseEffects(btnName1);
      sb = static_cast<CEGUI::Scrollbar*>(utils::CEGUIHelperUtil::getWindow(btnName1));
      if (sb)
      {
        GTRACE(3, "Scrollbar props: doc size [" << sb->getDocumentSize()
            << "], page size [" << sb->getPageSize() << "]");
        GTRACE(3, "Scrollbar props: step size [" << sb->getStepSize()
            << "], overlap size [" << sb->getOverlapSize() << "]");
      }
      else
      {
        GTRACE(3, "Could not get scrollbar item: " << btnName1);
      }
    }
    else
    {
      GTRACE(3, "could not find item named " << btnName1 << " for connection");
    }

    btnName1 = utils::CEGUIHelperUtil::getNameForWidget("TopRightSelectionFrame/Sample_MultiColumnList");
    CEGUI::MultiColumnList* mclbox = static_cast<CEGUI::MultiColumnList*>(utils::CEGUIHelperUtil::getWindow(btnName1));

    if (mclbox)
    {
      populateHardCodedMCLValues(mclbox, defaultBrushSkin.c_str());
    }
    else
    {
      GTRACE(3, "could not find item named " << btnName1 << " for connection");
    }

    btnName1 = utils::CEGUIHelperUtil::getNameForWidget("Sample_TransparentListBox");
    listbox= static_cast<CEGUI::Listbox*>(utils::CEGUIHelperUtil::getWindow(btnName1));
    if (listbox)
    {
      for (int i = 1; i <= 20; ++i)
      {
        std::string name = "Item Added via code ";
        name.append(guslib::stringutil::IntToString(i));
        listbox->addItem(
            new utils::SimpleListItem(
                name, "GlossySerpent", "ListboxSelectionBrush", CEGUI::Colour(0xFFFF0101), ""));
      }

      listbox->setShowVertScrollbar(true);
      listbox->setMousePassThroughEnabled(true);
      listbox->getVertScrollbar()->setMousePassThroughEnabled(false);
    }
    else
    {
      GTRACE(3, "could not find item named " << btnName1 << " for connection");
    }

    GTRACE(4, "Done customizing loaded CEGUI Layout.");
    // ------------ temporary code end

    GTRACE(4, "Attempting to load the context menu layout.");
    std::string contextMenuFile;

    contextMenuFile = levelConfig_[skinConfiguration]["contextMenu"].getAsStringOrDefaultVal("PopupMenu_0.8.layout");
    mContextMenuPtr = win_mgr.loadLayoutFromFile(contextMenuFile);

    mContextMenuPtr->setVisible(false);

    // quit item in context menu
    btnName1 = utils::CEGUIHelperUtil::getNameForWidget("Quit");
    CEGUI::Window* item = utils::CEGUIHelperUtil::getWindowForLoadedLayout(mContextMenuPtr, btnName1);
    if (item)
    {
      item->subscribeEvent("Clicked", CEGUI::Event::Subscriber(&WorkbenchState::onButtonQuitClicked, this));
      item->subscribeEvent(
          CEGUI::Window::EventMouseEntersArea,
          CEGUI::Event::Subscriber(&WorkbenchState::onWidgetHover, this));

      item->subscribeEvent(
          CEGUI::Window::EventMouseClick,
          CEGUI::Event::Subscriber(&WorkbenchState::onWidgetClick, this));

      GTRACE(3, "subscribed to click events for " << btnName1 << ".");
    }

    btnName1 = utils::CEGUIHelperUtil::getNameForWidget("Item1");
    item = utils::CEGUIHelperUtil::getWindowForLoadedLayout(mContextMenuPtr, btnName1);
    if (item)
    {
      item->subscribeEvent(
          CEGUI::Window::EventMouseEntersArea,
          CEGUI::Event::Subscriber(&WorkbenchState::onWidgetHover, this));

      item->subscribeEvent(
          CEGUI::Window::EventMouseClick,
          CEGUI::Event::Subscriber(&WorkbenchState::onWidgetClick, this));
    }

    btnName1 = utils::CEGUIHelperUtil::getNameForWidget("Item1/Item1Popup/SubItem1");
    item = utils::CEGUIHelperUtil::getWindowForLoadedLayout(mContextMenuPtr, btnName1);
    if (item)
    {
      item->subscribeEvent(
          CEGUI::Window::EventMouseEntersArea,
          CEGUI::Event::Subscriber(&WorkbenchState::onWidgetHover, this));

      item->subscribeEvent(
          CEGUI::Window::EventMouseClick,
          CEGUI::Event::Subscriber(&WorkbenchState::onWidgetClick, this));
    }

    btnName1 = utils::CEGUIHelperUtil::getNameForWidget("Item1/Item1Popup/SubItem2");
    item = utils::CEGUIHelperUtil::getWindowForLoadedLayout(mContextMenuPtr, btnName1);
    if (item)
    {
      item->subscribeEvent(
          CEGUI::Window::EventMouseEntersArea,
          CEGUI::Event::Subscriber(&WorkbenchState::onWidgetHover, this));

      item->subscribeEvent(
          CEGUI::Window::EventMouseClick,
          CEGUI::Event::Subscriber(&WorkbenchState::onWidgetClick, this));
    }

    //
    // Add all the extra widgets to the sound effects.
    //
    addWidgetMouseEffects("TopLeftFrame/Checkbox_001");
    addWidgetMouseEffects("TopLeftFrame/Checkbox_002");
    addWidgetMouseEffects("TopLeftFrame/Checkbox_003");
    addWidgetMouseEffects("TopLeftFrame/DifficultyLevelSpinner");
    addWidgetMouseEffects("Item1/Item1Popup/SubItem1");
    addWidgetMouseEffects("Item1/Item1Popup/SubItem2");
    addWidgetMouseEffects("TopRightSelectionFrame/__auto_closebutton__");

    addWidgetMouseEffects("TopRightSelectionFrame/OptionsWindowTab/Sample_Tab_1/ButtonGetRandomProgressValue2");
    addWidgetMouseEffects("TopRightSelectionFrame/OptionsWindowTab/Sample_Tab_1/ButtonGetRandomProgressValue3");
    addWidgetMouseEffects("TopRightSelectionFrame/OptionsWindowTab/Sample_Tab_1/ButtonGetRandomProgressValue4");

    addWidgetMouseEffects("TopRightSelectionFrame/OptionsWindowTab/__auto_TabPane__Buttons/__auto_btnSample_Tab_1");
    addWidgetMouseEffects("TopRightSelectionFrame/OptionsWindowTab/__auto_TabPane__Buttons/__auto_btnSample_Tab_2");
    addWidgetMouseEffects("TopRightSelectionFrame/OptionsWindowTab/__auto_TabPane__Buttons/__auto_btnSample_Tab_3");
    addWidgetMouseEffects("TopRightSelectionFrame/OptionsWindowTab/__auto_TabPane__Buttons/__auto_btnSample_Tab_4");
    addWidgetMouseEffects("TopRightSelectionFrame/OptionsWindowTab/__auto_TabPane__Buttons/__auto_btnSample_Tab_Empty");
    addWidgetMouseEffects("TopRightSelectionFrame/OptionsWindowTab/__auto_TabPane__Buttons/__auto_btnSample_Tab_Tex");
    addWidgetMouseEffects("TopRightSelectionFrame/OptionsWindowTab/__auto_TabPane__Buttons/__auto_btnScrollablePaneTab");
    addWidgetMouseEffects("TopRightSelectionFrame/OptionsWindowTab/__auto_TabPane__ScrollLeft");
    addWidgetMouseEffects("TopRightSelectionFrame/OptionsWindowTab/__auto_TabPane__ScrollRight");

    addWidgetMouseEffects("TopRightSelectionFrame/OptionsWindowTab/Sample_Tab_2/MusicVolumeSlider");

    addWidgetMouseEffects("TopRightSelectionFrame/OptionsWindowTab/Sample_Tab_4/UseCPUBurnCheckBox_2");
    addWidgetMouseEffects("TopRightSelectionFrame/OptionsWindowTab/Sample_Tab_4/UseCPUBurnCheckBox_3");
    addWidgetMouseEffects("TopRightSelectionFrame/OptionsWindowTab/Sample_Tab_4/DifficultyLevelSpinner_2");
    addWidgetMouseEffects("TopRightSelectionFrame/OptionsWindowTab/Sample_Tab_4/extra_button1");
    addWidgetMouseEffects("TopRightSelectionFrame/OptionsWindowTab/Sample_Tab_4/extra_button2");


    // Set-up the secondary view.
    std::string secondaryViewWidgetHost = levelConfig_["camera"]["copyRenderingToWidget"].getAsStringOrDefaultVal("");
    setupSecondaryView(secondaryViewWidgetHost);

    GTRACE(4, "Done loading the context menu layout.");

    initialized_ = true;
    return true;
  }


  /**
    TEMPORARY ONLY. Only for DEMO purposes.
    Populate the contents of a Combo Box with hard-coded values.
  */
  void WorkbenchState::populateHardCodedComboValues(CEGUI::Combobox* comboPtr, const char* brushName)
  {
    comboPtr->addItem(
        new utils::SimpleListItem("Easy", brushName, "ComboboxSelectionBrush", CEGUI::Colour(0xFFFF0101), ""));
    comboPtr->addItem(
        new utils::SimpleListItem("Normal", brushName, "ComboboxSelectionBrush", CEGUI::Colour(0xFFFF0101), ""));
    comboPtr->addItem(
        new utils::SimpleListItem("Hard", brushName, "ComboboxSelectionBrush", CEGUI::Colour(0xFFFF0101), ""));

    for (int i = 1; i <= 10; ++i)
    {
      std::string name = "SubItem";
      name.append(guslib::stringutil::IntToString(i));
      comboPtr->addItem(
          new utils::SimpleListItem(name, brushName, "ComboboxSelectionBrush", CEGUI::Colour(0xFFFF0101), ""));
    }

    int itemIndex = 1;
    // Set the list to read-only, so that the user can't type in entries.
    comboPtr->setItemSelectState(itemIndex, true);
    comboPtr->setReadOnly(true);

    // refresh the object.
    comboPtr->handleUpdatedListItemData();
  }


  /**
    TEMPORARY ONLY. Only for DEMO purposes.
    Populate the contents of a Multi Column List with hard-coded values.
  */
  void WorkbenchState::populateHardCodedMCLValues(CEGUI::MultiColumnList* mclPtr, const char* brushName)
  {
    // Add some empty rows to the MCL
    mclPtr->addRow();
    mclPtr->addRow();
    mclPtr->addRow();
    mclPtr->addRow();
    mclPtr->addRow();

    // Set first row item texts for the MCL
    mclPtr->setItem(
        new utils::SimpleListItem(
            "Laggers World", brushName, "MultiListSelectionBrush", CEGUI::Colour(0xFFFF0101), ""),
        0, 0);

    mclPtr->setItem(
        new utils::SimpleListItem(
            "yourgame.some-server.com", brushName, "MultiListSelectionBrush", CEGUI::Colour(0xFFFF0101), ""),
        1, 0);

    mclPtr->setItem(
        new utils::SimpleListItem(
            "[colour='FFFF0000']1000ms", brushName, "MultiListSelectionBrush", CEGUI::Colour(0xFFFF0101), ""),
        2, 0);

    // Set second row item texts for the MCL
    mclPtr->setItem(
        new utils::SimpleListItem(
            "Super-Server", brushName, "MultiListSelectionBrush", CEGUI::Colour(0xFFFF0101), ""),
        0, 1);

    mclPtr->setItem(
        new utils::SimpleListItem(
            "whizzy.fakenames.net", brushName, "MultiListSelectionBrush", CEGUI::Colour(0xFFFF0101), ""),
        1, 1);

    mclPtr->setItem(
        new utils::SimpleListItem(
            "[colour='FF00FF00']8ms", brushName, "MultiListSelectionBrush", CEGUI::Colour(0xFFFF0101), ""),
        2, 1);

    // Set third row item texts for the MCL
    mclPtr->setItem(
        new utils::SimpleListItem(
            "Cray-Z-Eds", brushName, "MultiListSelectionBrush", CEGUI::Colour(0xFFFF0101), ""),
        0, 2);

    mclPtr->setItem(
        new utils::SimpleListItem(
            "crayzeds.notarealserver.co.uk", brushName, "MultiListSelectionBrush", CEGUI::Colour(0xFFFF0101), ""),
        1, 2);

    mclPtr->setItem(
        new utils::SimpleListItem(
            "[colour='FF00FF00']43ms", brushName, "MultiListSelectionBrush", CEGUI::Colour(0xFFFF0101), ""),
        2, 2);

    // Set fourth row item texts for the MCL
    mclPtr->setItem(
        new utils::SimpleListItem(
            "Fake IPs", brushName, "MultiListSelectionBrush", CEGUI::Colour(0xFFFF0101), ""),
        0, 3);

    mclPtr->setItem(
        new utils::SimpleListItem(
            "123.320.42.242", brushName, "MultiListSelectionBrush", CEGUI::Colour(0xFFFF0101), ""),
        1, 3);

    mclPtr->setItem(
        new utils::SimpleListItem(
            "[colour='FFFFFF00']63ms", brushName, "MultiListSelectionBrush", CEGUI::Colour(0xFFFF0101), ""),
        2, 3);

    // Set fifth row item texts for the MCL
    mclPtr->setItem(
        new utils::SimpleListItem(
            "Yet Another Game Server", brushName, "MultiListSelectionBrush", CEGUI::Colour(0xFFFF0101), ""),
        0, 4);

    mclPtr->setItem(
        new utils::SimpleListItem(
            "abc.abcdefghijklmn.org", brushName, "MultiListSelectionBrush", CEGUI::Colour(0xFFFF0101), ""),
        1, 4);

    mclPtr->setItem(
        new utils::SimpleListItem(
            "[colour='FFFF6600']284ms", brushName, "MultiListSelectionBrush", CEGUI::Colour(0xFFFF0101), ""),
        2, 4);
  }


  void WorkbenchState::setupSecondaryView(const std::string& hostWidgetName)
  {
    m_secondaryViewActive = false;

    if (hostWidgetName.compare("") == 0)
    {
      GTRACE(3, "Skipping set-up of secondary view(not needed)");
      return;
    }

    Ogre::TexturePtr char_texture =
        Ogre::TextureManager::getSingleton().createManual(
            "secondary_view_render_texture",
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Ogre::TEX_TYPE_2D,
            512, 512, 0,
            Ogre::PF_R8G8B8A8,
            Ogre::TU_RENDERTARGET);

    Ogre::RenderTarget* char_rt = char_texture->getBuffer()->getRenderTarget();
    char_rt ->setAutoUpdated(true);

    Ogre::Viewport *char_view = char_rt->addViewport(enginePtr_->getSecondaryCamera());
    char_view->setClearEveryFrame(true);
    char_view->setOverlaysEnabled(false);
    char_view->setBackgroundColour(Ogre::ColourValue(0, 0, 0, 1.0));

    // It's possible to also enable skies and shadows for the secondary view, but for performance reasons,
    // let's just disable it for now. TODO (Augustin Preda, 2014.05.31): make configurable.
    // char_view->setSkiesEnabled(false);
    // char_view->setShadowsEnabled(false);
    char_rt->update();

    // Apply needed compositors to the secondary view.
    std::vector<std::string> compositorList =
      levelConfig_["lighting"]["applyCompositorToSecondaryView"].getAsStringList();
    for (std::vector<std::string>::iterator it = compositorList.begin(); it != compositorList.end(); ++it)
    {
      addCompositorByName(*it, char_view);
    }

    char_rt->update();

    CEGUI::OgreRenderer* myRenderer = static_cast<CEGUI::OgreRenderer*>(CEGUI::System::getSingleton().getRenderer());
    CEGUI::Texture& char_ceguiTex = myRenderer->createTexture("character", char_texture);


    // Definition of variables used in creating a render-to-texture (RTT) implementation.
    CEGUI::BasicImage*      d_textureTargetImage;
    CEGUI::TextureTarget*   d_textureTarget;
    d_textureTarget = myRenderer->createTextureTarget();
    CEGUI::String imageName("character_img");
    d_textureTargetImage = static_cast<CEGUI::BasicImage*>(&CEGUI::ImageManager::getSingleton().create(
        "BasicImage", imageName));

    d_textureTargetImage->setTexture(&char_ceguiTex);

    std::string widgetName1 = utils::CEGUIHelperUtil::getNameForWidget(hostWidgetName);
    CEGUI::Window* myWidget = utils::CEGUIHelperUtil::getWindow(widgetName1);
    if (myWidget)
    {
      myWidget->setProperty("Image", "character_img");

      myWidget->setUsingAutoRenderingSurface(false);  // true means caching.
      GTRACE(4, "Set window to auto-rendering");

      CEGUI::UDim dimWidth = myWidget->getSize().d_width;
      CEGUI::UDim dimHeight = myWidget->getSize().d_height;
      GTRACE(4, "Got widget size as: " << dimWidth << " x " << dimHeight);

      CEGUI::Sizef surfaceSize = myWidget->getParentPixelSize();
      GTRACE(4, "Got surface area as: " << surfaceSize.d_width << " x " << surfaceSize.d_height);
      float myWidth =
          surfaceSize.d_width * myWidget->getSize().d_width.d_scale
          + myWidget->getSize().d_width.d_offset;
      float myHeight =
          surfaceSize.d_height * myWidget->getSize().d_height.d_scale
          + myWidget->getSize().d_height.d_offset;

      GTRACE(4, "Calculated widget size as: " << myWidth << " x " << myHeight);
      d_textureTargetImage->setArea(CEGUI::Rectf(0, 0, myWidth, myHeight));

      m_secondaryViewActive = true;
    }
    else
    {
      GTRACE(2, "ERROR!!! Could not find widget [" << widgetName1 << "]");
    }
  }


  void WorkbenchState::setCompositorEnabledStatus(bool bCompositorStatus)
  {
    std::vector<std::string> sceneCompositorList = levelConfig_["general"]["applyCompositorToScene"].getAsStringList();
    GTRACE(4, "Applying compositor enabled status to [" << bCompositorStatus << "] for "
        << sceneCompositorList.size() << " compositors to the scene texture viewports");

    {
      Ogre::Viewport *vp = playerCamera_->getCamera()->getViewport();
      for (std::vector<std::string>::iterator it = sceneCompositorList.begin();
          it != sceneCompositorList.end(); ++it)
      {
        try
        {
          Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, *it, bCompositorStatus);
        }
        catch (const std::exception & e)
        {
          GTRACE(1, "Caught exception: " << e.what());
        }
      }
    }

    std::vector<std::string> secondary_scene_compositor_list =
        levelConfig_["general"]["applyCompositorToSecondaryView"].getAsStringList();

    GTRACE(4, "Applying compositor enabled status to [" << bCompositorStatus << "] for "
      << secondary_scene_compositor_list.size() << " compositors to the secondary scene texture viewports");
    {
      Ogre::Viewport *vp = enginePtr_->getSecondaryCamera()->getViewport();
      for (std::vector<std::string>::iterator it = secondary_scene_compositor_list.begin();
          it != secondary_scene_compositor_list.end(); ++it)
      {
        try
        {
          Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, *it, bCompositorStatus);
        }
        catch (const std::exception & e)
        {
          GTRACE(1, "Caught exception: " << e.what());
        }
      }
    }
  }

  bool WorkbenchState::onButtonOptionsClicked(const CEGUI::EventArgs& evt)
  {
    GTRACE(3, "Options Button Clicked");

    CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
    CEGUI::System* ceguiSysPtr = CEGUI::System::getSingletonPtr();
    std::string optionsLayoutFile =
        levelConfig_[m_preferredSkinConfig]["optionslayout"].getAsStringOrDefaultVal("workbenchoptions.layout");

    try
    {
      CEGUI::Window* currentRoot = ceguiSysPtr->getDefaultGUIContext().getRootWindow();
      if (NULL == currentRoot)
      {
        GTRACE(2, "No root window found!");
        return false;
      }

      // Try to get the window.
      CEGUI::Window* workbenchOptionsWindow = 0;
      std::string expectedOptionsWindowName = "WorkbenchOptionsRoot";
      if (ceguiSysPtr->getDefaultGUIContext().getRootWindow()->isChild(expectedOptionsWindowName))
      {
        workbenchOptionsWindow = ceguiSysPtr->getDefaultGUIContext().getRootWindow()->getChild(
            expectedOptionsWindowName.c_str());
        if (workbenchOptionsWindow)
        {
          workbenchOptionsWindow->show();
        }
      }
      else
      {
        // load
        workbenchOptionsWindow = win_mgr.loadLayoutFromFile(optionsLayoutFile);
        if (NULL == workbenchOptionsWindow)
        {
          GTRACE(2, "NULL layout load result.");
          return false;
        }

        currentRoot->addChild(workbenchOptionsWindow);
        GTRACE(3, "Loaded widget from layout file: " << optionsLayoutFile);

        CEGUI::PushButton* btn;
        btn = static_cast<CEGUI::PushButton*>(
            utils::CEGUIHelperUtil::getWindow("WorkbenchOptionsRoot/WorkbenchOptions/__auto_closebutton__"));

        addWidgetMouseEffects("WorkbenchOptionsRoot/WorkbenchOptions/__auto_closebutton__");
        if (btn)
        {
          btn->subscribeEvent(
              CEGUI::PushButton::EventClicked,
              CEGUI::Event::Subscriber(&WorkbenchState::onButtonOptionsCloseClicked, this));
        }

        btn = static_cast<CEGUI::PushButton*>(utils::CEGUIHelperUtil::getWindow(
            "WorkbenchOptionsRoot/WorkbenchOptions/ApplyAndRestart"));

        addWidgetMouseEffects("WorkbenchOptionsRoot/WorkbenchOptions/ApplyAndRestart");
        if (btn)
        {
          btn->subscribeEvent(
              CEGUI::PushButton::EventClicked,
              CEGUI::Event::Subscriber(&WorkbenchState::onButtonOptionsApplyClicked, this));
        }

        CEGUI::RadioButton* checkbox_001 = static_cast<CEGUI::RadioButton*>(utils::CEGUIHelperUtil::getWindow(
            "WorkbenchOptionsRoot/WorkbenchOptions/PreferredSkinGrouper/__auto_contentpane__/SFX_RB_Radio1"));

        addWidgetMouseEffects(
            "WorkbenchOptionsRoot/WorkbenchOptions/PreferredSkinGrouper/__auto_contentpane__/SFX_RB_Radio1");

        if (checkbox_001)
        {
          if (m_preferredSkinConfig == checkbox_001->getText().c_str())
          {
            checkbox_001->setSelected(true);
          }
        }

        CEGUI::RadioButton* checkbox_002 = static_cast<CEGUI::RadioButton*>(utils::CEGUIHelperUtil::getWindow(
            "WorkbenchOptionsRoot/WorkbenchOptions/PreferredSkinGrouper/__auto_contentpane__/SFX_RB_Radio2"));

        addWidgetMouseEffects(
            "WorkbenchOptionsRoot/WorkbenchOptions/PreferredSkinGrouper/__auto_contentpane__/SFX_RB_Radio2");

        if (checkbox_002)
        {
          if (m_preferredSkinConfig == checkbox_002->getText().c_str())
          {
            checkbox_002->setSelected(true);
          }
        }

        CEGUI::FrameWindow* widget = static_cast<CEGUI::FrameWindow*>(utils::CEGUIHelperUtil::getWindow(
            "WorkbenchOptionsRoot/WorkbenchOptions"));

        if (widget)
        {
          // Toggle the roll-up event twice to invalidate the contents of the frame.
          // The regular invalidation doesn't seem to do anything.
          // Neither is re-setting properties that would cause a redraw.
          widget->toggleRollup();
          widget->toggleRollup();
        }
      }
    }
    catch (const CEGUI::Exception& ex)
    {
      GTRACE(2, "Caught exception: " << ex.what());
    }

    return true;
  }

  bool WorkbenchState::onButtonOptionsCloseClicked(const CEGUI::EventArgs& evt)
  {
    CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();
    CEGUI::System* ceguiSysPtr = CEGUI::System::getSingletonPtr();

    CEGUI::Window* currentRoot = ceguiSysPtr->getDefaultGUIContext().getRootWindow();
    if (NULL == currentRoot)
    {
      GTRACE(2, "No root window found!");
      return false;
    }

    CEGUI::Window* workbenchOptionsWindow = 0;
    std::string expectedOptionsWindowName = "WorkbenchOptionsRoot";
    if (ceguiSysPtr->getDefaultGUIContext().getRootWindow()->isChild(expectedOptionsWindowName))
    {
      workbenchOptionsWindow = ceguiSysPtr->getDefaultGUIContext().getRootWindow()->getChild(
          expectedOptionsWindowName.c_str());

      if (workbenchOptionsWindow)
      {
        workbenchOptionsWindow->hide();
        return true;
      }
    }

    return false;
  }

  bool WorkbenchState::onButtonOptionsApplyClicked(const CEGUI::EventArgs& evt)
  {
    std::string preferredSkin("");
    CEGUI::RadioButton* widgetRadioButton = 0;

    std::string widgetName(
        "WorkbenchOptionsRoot/WorkbenchOptions/PreferredSkinGrouper/__auto_contentpane__/SFX_RB_Radio1");

    widgetRadioButton = static_cast<CEGUI::RadioButton*>(utils::CEGUIHelperUtil::getWindow(widgetName));
    if (widgetRadioButton && widgetRadioButton->isSelected())
    {
      preferredSkin = widgetRadioButton->getText().c_str();
    }

    widgetName ="WorkbenchOptionsRoot/WorkbenchOptions/PreferredSkinGrouper/__auto_contentpane__/SFX_RB_Radio2";
    widgetRadioButton = static_cast<CEGUI::RadioButton*>(utils::CEGUIHelperUtil::getWindow(widgetName));
    if (widgetRadioButton && widgetRadioButton->isSelected())
    {
      preferredSkin = widgetRadioButton->getText().c_str();
    }

    // Check to see whether the currently selected skin is among the allowed ones. We don't allow just any entry.
    std::vector <std::string> allowedSkins = this->enginePtr_->getConfig()["ui"]["allowedSkins"].getAsStringList();

    for (std::string item : allowedSkins)
    {
      if (item == preferredSkin)
      {
        enginePtr_->getConfig()["ui"]["skinconfig"].setAsString(preferredSkin);
      }
    }

    // Write the settings back into the ini file.
    enginePtr_->getConfig().save();

    // Launch the secondary process for a restart.
    guslib::AppRestarter::getPtr()->restart();

    // Exit from the current instance.
    exit();

    return true;
  }

  bool WorkbenchState::onButtonQuitClicked(const CEGUI::EventArgs& evt)
  {
    GTRACE(3, "Quit Button Clicked");

    exit();
    return true;
  }

  bool WorkbenchState::onButtonSubmenuClicked(const CEGUI::EventArgs& evt)
  {
    GTRACE(3, "Submenu display button clicked");
    CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();

    if (NULL == mContextMenuPtr)
    {
      return false;
    }

    std::string parentWndName = utils::CEGUIHelperUtil::getNameForWidget("MainMenuRootItem");
    CEGUI::Window* parentWindow = utils::CEGUIHelperUtil::getWindow(parentWndName);
    if (parentWindow)
    {
      if (parentWindow->isChild(mContextMenuPtr))
      {
        utils::CEGUIHelperUtil::removeChildWidget(parentWindow, mContextMenuPtr);
        mContextMenuPtr->setVisible(false);
      }
      else
      {
        utils::CEGUIHelperUtil::addChildWidget(parentWindow, mContextMenuPtr);
        mContextMenuPtr->setVisible(true);
      }
    }
    else
    {
      GTRACE(4, "WARNING: Could not get widget(parent window): " << parentWndName);
    }

    const CEGUI::WindowEventArgs wndEventArgs = static_cast<const CEGUI::WindowEventArgs&>(evt);
    if (wndEventArgs.window)
    {
      CEGUI::UVector2 subPos;
      // Note: if needed, one could also use the CEGUI::CoordConverter class.
      // E.g.: float posX = CEGUI::CoordConverter::windowToScreenX(*wndEventArgs.window, 0);
      float posX = oldMouseX_ + 1;
      float posY = oldMouseY_ + 1;

      subPos.d_x = CEGUI::UDim(0, posX);
      subPos.d_y = CEGUI::UDim(0, posY);

      GTRACE(4, "Clicked button for a submenu. Position of trigger(X, Y): " << posX << " , " << posY);

      mContextMenuPtr->setPosition(subPos);
    }

#if 0
    std::string popupMenuName("gus_custom_menu_popup");

    std::string expectedExistingName("MainMenuRootItem/gus_custom_menu_popup");
    CEGUI::Window* existingPopupMenu = utils::CEGUIHelperUtil::getWindow(expectedExistingName);

    if (existingPopupMenu)
    {
      GTRACE(4, "The popup menu was already found.");
      return false;
    }

    std::string parentWndName = utils::CEGUIHelperUtil::getNameForWidget("MainMenuRootItem");
    CEGUI::Window* parentWindow = utils::CEGUIHelperUtil::getWindow(parentWndName);

    std::string widgetType(preferredSkin_);
    widgetType.append("/Menubar");
    GTRACE(4, "Creating a menu-bar window.");
    CEGUI::Window* bar = win_mgr.createWindow(widgetType, popupMenuName);

    // Get the location of the clicked item.
    std::string triggerBtnName(utils::CEGUIHelperUtil::getNameForWidget("TopLeftFrame/ButtonSubMenu"));
    CEGUI::Window* trigger = utils::CEGUIHelperUtil::getWindow(triggerBtnName);
    if (trigger)
    {
      CEGUI::UDim leftPos(0, trigger->getXPosition().asAbsolute(10));
      CEGUI::UDim topPos(0, trigger->getYPosition().asAbsolute(10));
      bar->setArea(leftPos, topPos, CEGUI::UDim(0, 120),  CEGUI::UDim(0, 96));
    }
    else
    {
      bar->setArea(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(0.1, 0),  CEGUI::UDim(0, 48));
    }

    bar->setAlwaysOnTop(true);  // we want the menu on top


    if (parentWindow)
    {
      utils::CEGUIHelperUtil::addChildWidget(parentWindow, bar);

      // Create the submenu items.
      // file menu item
      CEGUI::Window* file = win_mgr.createWindow("GlossySerpentFHD/MenuItem");
      file->setText("File");
      file->setArea(CEGUI::UDim(0, 0), CEGUI::UDim(0, 0), CEGUI::UDim(1.0, 0),  CEGUI::UDim(0.5, 0));
      utils::CEGUIHelperUtil::addChildWidget(bar, file);


      // quit item in file menu
      CEGUI::Window* item = win_mgr.createWindow("GlossySerpentFHD/MenuItem");
      item->setText("Quit");
      item->setArea(CEGUI::UDim(0, 0), CEGUI::UDim(0.5, 0), CEGUI::UDim(1.0, 0),  CEGUI::UDim(1.0, 0));
      item->subscribeEvent("Clicked", CEGUI::Event::Subscriber(&WorkbenchState::onButtonQuitClicked, this));
      utils::CEGUIHelperUtil::addChildWidget(bar, item);
    }
#endif
    return true;
  }


  bool WorkbenchState::onButtonReloadGroupClicked(const CEGUI::EventArgs& evt)
  {
    reloadGroupNextTick_ = expGroupName_;
    return true;
  }

  bool WorkbenchState::onButtonNextShadowTech(const CEGUI::EventArgs& evt)
  {
    moveToNextShadowTechnique();
    return true;
  }

  bool WorkbenchState::onButtonPrevShadowTech(const CEGUI::EventArgs& evt)
  {
    moveToPrevShadowTechnique();
    return true;
  }


  bool WorkbenchState::onButtonGetRandomValueClicked(const CEGUI::EventArgs& evt)
  {
    GTRACE(3, "WorkbenchState::onButtonGetRandomValueClicked");
    CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton();

    float myval = guslib::Random::getPtr()->getValueUpTo(100) / 100.0f;

    // Update the level label
    std::string btnName1 = utils::CEGUIHelperUtil::getNameForWidget(
        "TopRightSelectionFrame/OptionsWindowTab/Sample_Tab_1/ButtonGetRandomProgressValue");

    if (utils::CEGUIHelperUtil::getWindow(btnName1))
    {
      GTRACE(3, "onButtonGetRandomValueClicked value is " << myval);
      // Update the level label
      btnName1 = utils::CEGUIHelperUtil::getNameForWidget(
          "TopRightSelectionFrame/OptionsWindowTab/Sample_Tab_1/Sample_ProgressBar");

      CEGUI::ProgressBar * progresser = static_cast<CEGUI::ProgressBar*>(utils::CEGUIHelperUtil::getWindow(btnName1));
      if (progresser)
      {
        progresser->setProgress(myval);
      }
    }
    return true;
  }

  bool WorkbenchState::onScrollValueChange(const CEGUI::EventArgs& evt)
  {
    GTRACE(3, "WorkbenchState::onScrollValueChange");

    float myval = 0.0f;

    // Update the level label
    std::string widgetName = utils::CEGUIHelperUtil::getNameForWidget(

      "TopRightSelectionFrame/OptionsWindowTab/Sample_Tab_1/Sample_ProgressSlider");
    CEGUI::Scrollbar * scroller = static_cast<CEGUI::Scrollbar*>(utils::CEGUIHelperUtil::getWindow(widgetName));
    if (scroller)
    {
      myval = scroller->getScrollPosition();
      GTRACE(3, "Scroll value changed to " << myval);
      // Update the level label
      widgetName = utils::CEGUIHelperUtil::getNameForWidget(
          "TopRightSelectionFrame/OptionsWindowTab/Sample_Tab_1/Sample_ProgressBar");

      CEGUI::ProgressBar * progresser = static_cast<CEGUI::ProgressBar*>(utils::CEGUIHelperUtil::getWindow(widgetName));
      if (progresser)
      {
        progresser->setProgress(myval);
      }
    }

    return true;
  }

  bool WorkbenchState::addWidgetMouseEffects(const std::string& widget_name,
      bool apply_hover_effect,
      bool apply_click_effect)
  {
    CEGUI::Window* widget;
    widget = utils::CEGUIHelperUtil::getWindow(widget_name);
    if (widget)
    {
      if (apply_hover_effect)
      {
        widget->subscribeEvent(
            CEGUI::Window::EventMouseEntersArea,
            CEGUI::Event::Subscriber(&WorkbenchState::onWidgetHover, this));
      }
      if (apply_click_effect)
      {
        widget->subscribeEvent(
            CEGUI::Window::EventMouseClick,
            CEGUI::Event::Subscriber(&WorkbenchState::onWidgetClick, this));
      }
    }
    else
    {
      GTRACE(3, "Could not connect widget mouse effects. Widget not found: " << widget_name);
    }

    return false;
  }


  bool WorkbenchState::onWidgetHover(const CEGUI::EventArgs& evt)
  {
    try
    {
      gussound::SoundManager::getPtr()->getRepository()->getSound("menu_hover")->play2D();
    }
    catch (const std::exception& e)
    {
      GTRACE(1, "Could not play sound: " << e.what());
    }

    return true;
  }

  bool WorkbenchState::onWidgetClick(const CEGUI::EventArgs& evt)
  {
    try
    {
      gussound::SoundManager::getPtr()->getRepository()->getSound("menu_click")->play2D();
    }
    catch (const std::exception& e)
    {
      GTRACE(1, "Could not play sound: " << e.what());
    }

    return true;
  }


  void printAllKids(Ogre::SceneNode * rootNode, int level = 0)
  {
    if (!rootNode)
    {
      return;
    }

    GTRACE(3, "Lvl: " << level << "; Node: " << rootNode->getName());
    Ogre::Node::ChildNodeIterator it = rootNode->getChildIterator();
    while (it.hasMoreElements())
    {
      Ogre::SceneNode * node = static_cast<Ogre::SceneNode*>(it.getNext());
      Ogre::SceneNode::ObjectIterator it2 = node->getAttachedObjectIterator();
      while (it2.hasMoreElements())
      {
        Ogre::MovableObject *movable = static_cast<Ogre::MovableObject*>(it2.getNext());
        GTRACE(3, "attached: " << movable->getName() << "; " << movable->getMovableType());
        if (movable->getMovableType() == "Entity")
        {
          Ogre::Entity * ent = static_cast<Ogre::Entity*>(movable);
          int numSubEntities = ent->getNumSubEntities();
          for (int i = 0; i < numSubEntities; ++i)
          {
            Ogre::SubEntity * subent = ent->getSubEntity(i);
            GTRACE(3, "Entity " << i << " material: " << subent->getMaterialName());
          }
        }
      }

      printAllKids(node, level + 1);
    }
  }


  std::vector <std::string> getEntitiesWithMaterial(Ogre::SceneNode * rootNode, std::string materialName)
  {
    std::vector <std::string> ret;

    if (!rootNode)
    {
      return ret;
    }

    if (materialName.length() <= 0)
    {
      return ret;
    }

    Ogre::Node::ChildNodeIterator it = rootNode->getChildIterator();
    while (it.hasMoreElements())
    {
      Ogre::SceneNode* node = static_cast<Ogre::SceneNode*>(it.getNext());
      if (NULL == node)
      {
        return ret;
      }

      Ogre::SceneNode::ObjectIterator it2 = node->getAttachedObjectIterator();
      while (it2.hasMoreElements())
      {
        Ogre::MovableObject* movable = static_cast<Ogre::MovableObject*>(it2.getNext());
        if (NULL == movable)
        {
          return ret;
        }

        GTRACE(3, "attached: " << movable->getName() << "; " << movable->getMovableType());
        if (movable->getMovableType() == "Entity")
        {
          Ogre::Entity * ent = static_cast<Ogre::Entity*>(movable);
          if (NULL == ent)
          {
            return ret;
          }

          int numSubEntities = ent->getNumSubEntities();
          for (int i = 0; i < numSubEntities; ++i)
          {
            Ogre::SubEntity * subent = ent->getSubEntity(i);
            if (subent->getMaterialName() == materialName)
            {
              ret.push_back(ent->getName());
            }

            GTRACE(3, "Entity " << i << " material: " << subent->getMaterialName());
          }
        }
      }

      std::vector <std::string> subret = getEntitiesWithMaterial(node, materialName);
      for (std::vector <std::string>::iterator myIt = subret.begin(); myIt != subret.end(); ++myIt)
      {
        ret.push_back(*myIt);
      }
    }

    return ret;
  }

  void setMaterialForEntities(
      Ogre::SceneManager * sceneManager,
      std::string materialName,
      std::vector <std::string> entities)
  {
    for (std::vector <std::string>::iterator it = entities.begin(); it != entities.end(); ++it)
    {
      std::string entityName = *it;
      Ogre::Entity *ent = sceneManager->getEntity(entityName);
      if (ent)
      {
        int numSubEntities = ent->getNumSubEntities();
        for (int i = 0; i < numSubEntities; ++i)
        {
          Ogre::SubEntity * subent = ent->getSubEntity(i);
          subent->setMaterialName(materialName);

          GTRACE(3, "Entity " << i << " set material to: " << subent->getMaterialName());
        }
      }
    }
  }


  bool WorkbenchState::renderScene()
  {
    if (false == initialized_)
    {
      return initialize();
    }

    GTRACE(5, "WorkbenchState::renderScene");

    Ogre::RenderTarget::FrameStats stats = enginePtr_->getFrameListener()->getRenderWindow()->getStatistics();

    CEGUI::WindowManager* win_mgr = CEGUI::WindowManager::getSingletonPtr();
    CEGUI::Window* label;

    // ----------------- update the FPS counter ----------------
    if (win_mgr)
    {
      std::string btnName1 = utils::CEGUIHelperUtil::getNameForWidget("TopLeftFrame/FPSValueLabel");
      label = utils::CEGUIHelperUtil::getWindow(btnName1);
      if (label)
      {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << stats.lastFPS;
        std::string sText = ss.str();
        label->setText((CEGUI::utf8*) sText.c_str());
      }
    }

    // ----------------- apply the new camera shadow if needed ----------------
    if (applyCameraSetupNextTick_)
    {
      GTRACE(4, "Applying shadow camera setup in current tick for render");
#if GUS_USE_RTSS
      // Grab the scheme render state.
      Ogre::RTShader::RenderState* schemRenderState = enginePtr_->getShaderGenerator()->getRenderState(
          Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

      // Assume a lighting model of SSLM_PerVertexLighting
      Ogre::RTShader::SubRenderState* perPerVertexLightModel =
          enginePtr_->getShaderGenerator()->createSubRenderState(Ogre::RTShader::FFPLighting::Type);

      schemRenderState->addTemplateSubRenderState(perPerVertexLightModel);
#endif
      Ogre::PSSMShadowCameraSetup * subptr = 0;
      if (win_mgr)
      {
        std::string btnName1 = utils::CEGUIHelperUtil::getNameForWidget("TopLeftFrame/ShadowCameraValueLabel");
        label = utils::CEGUIHelperUtil::getWindow(btnName1);
        if (label)
        {
          std::string sMode("");
          Ogre::ShadowCameraSetupPtr scs;
          switch (currentCameraSetup_)
          {
          case 1:
            scs = Ogre::ShadowCameraSetupPtr(OGRE_NEW Ogre::LiSPSMShadowCameraSetup());
            sMode = "LiSPSMShadowCameraSetup";
            break;
          case 2:
            scs = Ogre::ShadowCameraSetupPtr(OGRE_NEW Ogre::FocusedShadowCameraSetup());
            sMode = "FocusedShadowCameraSetup";
            break;
          case 3:
            if (mPlane)
            {
              scs = Ogre::ShadowCameraSetupPtr(OGRE_NEW Ogre::PlaneOptimalShadowCameraSetup(mPlane));
              sMode = "PlaneOptimalShadowCameraSetup";
            }
            break;
          case 4:
            {
              subptr = OGRE_NEW Ogre::PSSMShadowCameraSetup();
              if (playerCamera_->getCamera())
              {
                double padding = levelConfig_["pssm"]["splitPadding"].getAsDoubleOrDefaultVal(10);
                GTRACE(3, "Setting PSSM shadow camera setup; split padding: " << padding);
                subptr->setSplitPadding(padding);
                int nSplitCount = levelConfig_["pssm"]["splitCount"].getAsIntOrDefaultVal(3);
                double nearSplitDist = levelConfig_["pssm"]["splitNearDist"].getAsDoubleOrDefaultVal(5);
                double farSplitDist = levelConfig_["pssm"]["splitFarDist"].getAsDoubleOrDefaultVal(500);
                for (int i = 0; i < nSplitCount; ++i)
                {
                  std::stringstream ss;
                  ss << "optimalAdjustFactor." << i;
                  double currentOptimalAdjustFactor = levelConfig_["pssm"][ss.str()].getAsDouble();
                  subptr->setOptimalAdjustFactor(i, currentOptimalAdjustFactor);
                }

                GTRACE(3, "Setting PSSM shadow camera setup; calculating split points for: " << nSplitCount << ", "
                  << nearSplitDist << ", " << farSplitDist << ".");
                subptr->calculateSplitPoints(nSplitCount, nearSplitDist, farSplitDist);
                bool simpleOptimalAdjust = levelConfig_["pssm"]["simpleOptimalAdjust"].getAsBoolOrDefault(false);
                subptr->setUseSimpleOptimalAdjust(simpleOptimalAdjust);
                GTRACE(3, "Setting PSSM shadow camera setup; simple optimal adjust: " << simpleOptimalAdjust);
              }
              else
              {
                GTRACE(2, "WARNING: the player camera was not found");
              }
              scs.bind(subptr);
              sMode = "PSSMShadowCameraSetup";
            }
            break;
          case 0:
          default:
            scs = Ogre::ShadowCameraSetupPtr(OGRE_NEW Ogre::DefaultShadowCameraSetup());
            sMode = "DefaultShadowCameraSetup";
            break;
          }

          if (false == scs.isNull())
          {
            enginePtr_->getSceneManagerPtr()->setShadowCameraSetup(scs);
            GTRACE(3, "Applied shadow CAMERA setup of type " << sMode);
            label->setText((CEGUI::utf8*) sMode.c_str());
#if GUS_USE_RTSS
            if (currentCameraSetup_ == 4)
            {
              Ogre::RTShader::SubRenderState* subRenderState =
                  enginePtr_->getShaderGenerator()->createSubRenderState(Ogre::RTShader::IntegratedPSSM3::Type);
              Ogre::RTShader::IntegratedPSSM3* pssm3SubRenderState =
                  static_cast<Ogre::RTShader::IntegratedPSSM3*>(subRenderState);

              if (subptr)
              {
                const Ogre::PSSMShadowCameraSetup::SplitPointList& srcSplitPoints = subptr->getSplitPoints();
                Ogre::RTShader::IntegratedPSSM3::SplitPointList dstSplitPoints;

                for (unsigned int i = 0; i < srcSplitPoints.size(); ++i)
                {
                  dstSplitPoints.push_back(srcSplitPoints[i]);
                }

                pssm3SubRenderState->setSplitPoints(dstSplitPoints);
                schemRenderState->addTemplateSubRenderState(subRenderState);
              }
            }
            else
            {
              Ogre::RTShader::SubRenderStateList myList = schemRenderState->getTemplateSubRenderStateList();
              for (Ogre::RTShader::SubRenderStateList::iterator it = myList.begin(); it != myList.end(); ++it)
              {
                schemRenderState->removeTemplateSubRenderState(*it);
              }
            }
#endif
          }
#if GUS_USE_RTSS
          enginePtr_->getShaderGenerator()->invalidateScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
          readAndApplyShadowCasterMaterial();
#endif
          applyCameraSetupNextTick_ = false;
        }
      }
      GTRACE(4, "Done applying shadow camera setup in current tick for render");
    }

    // ------------------- apply the new shadow technique if needed ----------------
    if (applyShadowTechNextTick_)
    {
      GTRACE(4, "Applying shadow tech in current tick for render");
      applyShadowTechnique(currentShadowTech_);
      applyShadowTechNextTick_ = false;
      if (win_mgr)
      {
        std::string btnName1 = utils::CEGUIHelperUtil::getNameForWidget("TopLeftFrame/ShadowModeValueLabel");
        label = utils::CEGUIHelperUtil::getWindow(btnName1);
        if (label)
        {
          std::string sMode("");
          switch (currentShadowTech_)
          {
          case 1:
            sMode = "STENCIL_MODULATIVE";
            break;
          case 2:
            sMode = "STENCIL_ADDITIVE";
            break;
          case 3:
            sMode = "TEXTURE_ADDITIVE";
            break;
          case 4:
            sMode = "TEXTURE_MODULATIVE";
            break;
          case 5:
            sMode = "TEXTURE_ADDITIVE_INTEGRATED";
            break;
          case 6:
            sMode = "TEXTURE_MODULATIVE_INTEGRATED";
            break;
          case 0:
          default:
            sMode = "OFF";
            break;
          }
          label->setText((CEGUI::utf8*) sMode.c_str());
        }
      }
      GTRACE(4, "Done applying shadow tech in current tick for render");
    }

    // ------------- handle camera movement ---------------------
    if (movingCameraForward_ || movingCameraBack_
      || movingCameraLeft_ || movingCameraRight_)
    {
      Ogre::Real modifier = 0.6f;

      if (movingCameraFaster_ || movingCameraSlower_)
      {
        if (movingCameraFaster_ && movingCameraSlower_)
        {
          modifier = 0.6f;
        }
        else
        {
          if (movingCameraFaster_)
          {
            modifier = 2.0f;
          }
          else  // only slower left.
          {
            modifier = 0.1f;
          }
        }
      }

      Ogre::Vector3 accel = Ogre::Vector3::ZERO;
      if (movingCameraForward_)
      {
        accel += playerCamera_->getCamera()->getDirection();
      }

      if (movingCameraBack_)
      {
        accel -= playerCamera_->getCamera()->getDirection();
      }

      if (movingCameraRight_)
      {
        accel += playerCamera_->getCamera()->getRight();
      }

      if (movingCameraLeft_)
      {
        accel -= playerCamera_->getCamera()->getRight();
      }

      accel.normalise();
      playerCamera_->getCamera()->move(accel * modifier);
      secondCamera_->getCamera()->move(accel * modifier);
    }

    // ----------------- reload the experimental group --------------
    if (reloadGroupNextTick_.length() > 0)
    {
      reloadResourceGroup(reloadGroupNextTick_.c_str());

      reloadGroupNextTick_ = "";
    }


    return true;
  }

  /**
    Reload a resource group, identified by its name.
    @param resourceGroupName The name of the resoruce group to reload.
  */
  void WorkbenchState::reloadResourceGroup(const char* resourceGroupName)
  {
      GTRACE(2, " ---------------------------- Reloading group " << resourceGroupName);
      Ogre::ResourceGroupManager::LocationList dirs =
          Ogre::ResourceGroupManager::getSingleton().getResourceLocationList(resourceGroupName);

      for (Ogre::ResourceGroupManager::LocationList::iterator it = dirs.begin(); it != dirs.end(); ++it)
      {
        GTRACE(3, "Group has path: " <<(*it)->archive->getName());
      }

      Ogre::Node::ChildNodeIterator it = enginePtr_->getSceneManagerPtr()->getRootSceneNode()->getChildIterator();
      printAllKids(enginePtr_->getSceneManagerPtr()->getRootSceneNode());

      Ogre::StringVectorPtr ress = Ogre::ResourceGroupManager::getSingleton().findResourceNames(
          resourceGroupName, "*");

      if (false == ress.isNull())
      {
        for (Ogre::StringVector::const_iterator it = ress.getPointer()->begin();
          it != ress.getPointer()->end();
          ++it)
        {
          GTRACE(3, "Group has resource: " << *it);
        }
      }

      std::vector <std::string> materialsToReload = levelConfig_["reloadables"]["material"].getAsStringList();

      std::map <std::string, std::vector <std::string>> materialToEntitiesMap;
      for (std::vector <std::string>::iterator it = materialsToReload.begin();
        it != materialsToReload.end(); ++it)
      {
        std::string matName = *it;
        std::vector <std::string> entities =
            getEntitiesWithMaterial(enginePtr_->getSceneManagerPtr()->getRootSceneNode(), matName);
        for (std::vector <std::string>::iterator sit = entities.begin(); sit != entities.end(); ++sit)
        {
          GTRACE(4, "FYI: material was removed from entity: " << *sit);
        }
        materialToEntitiesMap[matName] = entities;
      }

      // Clear the existing materials.
      // std::string shadowTextureCasterMaterial = levelConfig_["lighting"]["shadowCasterMaterial"].getAsString();
      std::string shadowTextureReceiverMaterial = levelConfig_["lighting"]["shadowReceiverMaterial"].getAsString();

      applyShadowTextureCasterMaterial("");
      applyShadowTextureReceiverMaterial("");

      Ogre::ShadowTechnique myTech = enginePtr_->getSceneManagerPtr()->getShadowTechnique();
      enginePtr_->getSceneManagerPtr()->setShadowTechnique(Ogre::SHADOWTYPE_NONE);

#if GUS_USE_RTSS
      enginePtr_->cleanupRTSGResources();
#endif
      removeSceneCompositors();

      // reload the group
      Ogre::ResourceGroupManager::getSingleton().clearResourceGroup(resourceGroupName);

#if GUS_USE_RTSS
      enginePtr_->reloadRTSGResources();
#endif

      applyShadowTechNextTick_ = true;
      applyCameraSetupNextTick_ = true;

      Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(resourceGroupName);
      Ogre::ResourceGroupManager::getSingleton().loadResourceGroup(resourceGroupName);
      GTRACE(2, "Cleared and reloaded group: " << resourceGroupName);

      addSceneCompositors();

      // reassign the materials
      for (std::vector <std::string>::iterator it = materialsToReload.begin();
        it != materialsToReload.end(); ++it)
      {
        std::string matName = *it;
        std::vector <std::string> entities = materialToEntitiesMap[matName];
        setMaterialForEntities(enginePtr_->getSceneManagerPtr(), matName, entities);
      }

#if GUS_USE_RTSS
      // TODO(Augustin Preda, 2014.05.31): verify if needed.
      // enginePtr_->reloadRTSGResources();
#endif

      readAndApplyShadowCasterMaterial();
      applyShadowTextureReceiverMaterial(shadowTextureReceiverMaterial);

      enginePtr_->getSceneManagerPtr()->setShadowTechnique(myTech);
      GTRACE(2, " ------------------------ Done Reloading group " << resourceGroupName);
  }

  bool WorkbenchState::renderGUI()
  {
    GTRACE(5, "WorkbenchState Render GUI");

    if (m_secondaryViewActive)
    {
      Ogre::Resource* res = Ogre::TextureManager::getSingleton().createOrRetrieve(
          "secondary_view_render_texture",
          Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME).first.getPointer();

      Ogre::Texture* texture = static_cast<Ogre::Texture*>(res);
      Ogre::RenderTarget* target = texture->getBuffer()->getRenderTarget();

      target->update();

      std::string secondaryViewWidgetHost =
          levelConfig_["camera"]["copyRenderingToWidget"].getAsStringOrDefaultVal("");

      std::string widgetName1 = utils::CEGUIHelperUtil::getNameForWidget(
        secondaryViewWidgetHost);
      CEGUI::Window* myWidget = utils::CEGUIHelperUtil::getWindow(widgetName1);
      if (myWidget)
      {
        myWidget->invalidateRenderingSurface();
      }
    }


    return true;
  }

  void WorkbenchState::notifyOfShutdown()
  {
  }


  void WorkbenchState::applyShadowTechnique(Ogre::ShadowTechnique st)
  {
    enginePtr_->getSceneManagerPtr()->setShadowTechnique(st);
    int indexFrom = levelConfig_["lighting"]["lightIndexFrom"].getAsInt();
    int indexTo = levelConfig_["lighting"]["lightIndexTo"].getAsInt();

    try
    {
      // Go through the lights and reapply the settings.
      for (int nIdx = indexFrom; nIdx <= indexTo; ++nIdx)
      {
        std::stringstream ss;
        ss << "light." << nIdx;
        std::string sName = ss.str();

        applyLightSettingsFromCfg(sName);
      }
    }
    catch (const std::exception &e)
    {
      GTRACE(4, "Exception when applying shadow technique: " << e.what());
    }
  }


  void WorkbenchState::applyShadowTechnique(int shadowType)
  {
    GTRACE(4, "WorkbenchState::applyShadowTechnique : " << shadowType);
    if (shadowType != 0)
    {
      switch (shadowType)
      {
      case 2:
        applyShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
        GTRACE(4, "Using shadow technique: SHADOWTYPE_STENCIL_ADDITIVE");
        break;
      case 3:
        applyShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE);
        GTRACE(4, "Using shadow technique: SHADOWTYPE_TEXTURE_ADDITIVE");
        break;
      case 4:
        applyShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
        GTRACE(4, "Using shadow technique: SHADOWTYPE_TEXTURE_MODULATIVE");
        break;
      case 5:
        applyShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
        GTRACE(4, "Using shadow technique: SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED");
        break;
      case 6:
        applyShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED);
        GTRACE(4, "Using shadow technique: SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED");
        break;
      case 1:
      default:
        applyShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
        GTRACE(4, "Using shadow technique: SHADOWTYPE_STENCIL_MODULATIVE");
        break;
      }
    }
    else
    {
      applyShadowTechnique(Ogre::SHADOWTYPE_NONE);
      GTRACE(4, "Using shadow technique: SHADOWTYPE_NONE");
    }

    // GL performs much better if you pick half-float format.
    //  E.g. setShadowTexturePixelFormat(Ogre::PF_FLOAT16_R);
    // D3D is the opposite: if you ask for PF_FLOAT16_R you get an integer format instead.
    // You can ask for PF_FLOAT16_GR, but the precision doesn't work well
    if (rendererIsOpenGL_)
    {
      enginePtr_->getSceneManagerPtr()->setShadowTexturePixelFormat(Ogre::PF_FLOAT16_R);
    }
    else
    {
      enginePtr_->getSceneManagerPtr()->setShadowTexturePixelFormat(Ogre::PF_FLOAT32_R);
    }

    std::string sCurrentCameraSetupMat = "";
    {
      std::stringstream ss;
      ss << "casterPerShadow." << currentCameraSetup_;
      sCurrentCameraSetupMat = ss.str();
    }

    readAndApplyShadowCasterMaterial();

    std::string shadowTextureReceiverMaterial = levelConfig_["lighting"]["shadowReceiverMaterial"].getAsString();
    applyShadowTextureReceiverMaterial(shadowTextureReceiverMaterial);

    bool shadowCasterRenderBackFaces =
        levelConfig_["lighting"]["shadowCasterRenderBackFaces"].getAsBoolOrDefault(false);

    enginePtr_->getSceneManagerPtr()->setShadowCasterRenderBackFaces(shadowCasterRenderBackFaces);

    bool shadowTextureSelfShadow = levelConfig_["lighting"]["shadowTextureSelfShadow"].getAsBoolOrDefault(false);
    enginePtr_->getSceneManagerPtr()->setShadowTextureSelfShadow(shadowTextureSelfShadow);

    int nDefaultShadowTextureCount = levelConfig_["lighting"]["shadowTextureCount"].getAsIntOrDefaultVal(3);
    int nDefaultOmniLightCount = levelConfig_["lighting"]["shadowTextureOmniLight"].getAsIntOrDefaultVal(0);
    int nDefaultSpotLightCount = levelConfig_["lighting"]["shadowTextureSpotLight"].getAsIntOrDefaultVal(0);
    int nDefaultDirLightCount = levelConfig_["lighting"]["shadowTextureDirLight"].getAsIntOrDefaultVal(0);

    if (nDefaultShadowTextureCount)
    {
      enginePtr_->getSceneManagerPtr()->setShadowTextureCount(nDefaultShadowTextureCount);
    }

    if (nDefaultSpotLightCount)
    {
      enginePtr_->getSceneManagerPtr()->setShadowTextureCountPerLightType(
          Ogre::Light::LT_SPOTLIGHT,
          nDefaultSpotLightCount);
    }

    if (nDefaultDirLightCount)
    {
      enginePtr_->getSceneManagerPtr()->setShadowTextureCountPerLightType(
          Ogre::Light::LT_DIRECTIONAL,
          nDefaultDirLightCount);
    }

    if (nDefaultOmniLightCount)
    {
      enginePtr_->getSceneManagerPtr()->setShadowTextureCountPerLightType(
          Ogre::Light::LT_POINT,
          nDefaultOmniLightCount);
    }

    std::vector<std::string> compositorList = levelConfig_["lighting"]["applyCompositorToShadow"].getAsStringList();
    const unsigned numShadowRTTs = enginePtr_->getSceneManagerPtr()->getShadowTextureCount();
    GTRACE(4, "Applying " << compositorList.size() << " compositors to the "
        << numShadowRTTs << " shadow texture viewports");

    for (unsigned i = 0; i < numShadowRTTs; ++i)
    {
      Ogre::TexturePtr tex = enginePtr_->getSceneManagerPtr()->getShadowTexture(i);
      Ogre::Viewport *vp = tex->getBuffer()->getRenderTarget()->getViewport(0);
      vp->setBackgroundColour(Ogre::ColourValue(1, 1, 1, 1));
      vp->setClearEveryFrame(true);
      for (std::vector<std::string>::iterator it = compositorList.begin(); it != compositorList.end(); ++it)
      {
        addCompositorByName(*it, vp);
      }
    }
  }

  void WorkbenchState::moveToNextCameraSetup()
  {
    currentCameraSetup_ = currentCameraSetup_ + 1;
    if (currentCameraSetup_ > cameraSetupMaximum_)
    {
      currentCameraSetup_ = cameraSetupMinimum_;
    }
    applyCameraSetupNextTick_ = true;
  }

  void WorkbenchState::moveToPrevCameraSetup()
  {
    currentCameraSetup_ = currentCameraSetup_ - 1;
    if (currentCameraSetup_ < cameraSetupMinimum_)
    {
      currentCameraSetup_ = cameraSetupMaximum_;
    }
    applyCameraSetupNextTick_ = true;
  }


  void WorkbenchState::moveToNextShadowTechnique()
  {
    currentShadowTech_ = currentShadowTech_ + 1;
    if (currentShadowTech_ > shadowTechMaximum_)
    {
      currentShadowTech_ = shadowTechMinimum_;
    }
    applyShadowTechNextTick_ = true;
  }

  void WorkbenchState::moveToPrevShadowTechnique()
  {
    currentShadowTech_ = currentShadowTech_ - 1;
    if (currentShadowTech_ < shadowTechMinimum_)
    {
      currentShadowTech_ = shadowTechMaximum_;
    }
    applyShadowTechNextTick_ = true;
  }

  void WorkbenchState::applyLightSettingsFromCfg(const std::string& lightCfgEntry)
  {
    double x, y, z;
    Ogre::Light * lTemp;

    std::string sName(lightCfgEntry);
    std::string sLightName = levelConfig_[sName]["name"].getAsStringOrDefaultVal("");

    lTemp = enginePtr_->getSceneManagerPtr()->getLight(sLightName);

    int lightType = levelConfig_[sName]["type"].getAsInt();
    x = levelConfig_[sName]["lookAt.x"].getAsDouble();
    y = levelConfig_[sName]["lookAt.y"].getAsDouble();
    z = levelConfig_[sName]["lookAt.z"].getAsDouble();


    double spotX = levelConfig_[sName]["range.x"].getAsDoubleOrDefaultVal(0.0f);
    double spotY = levelConfig_[sName]["range.y"].getAsDoubleOrDefaultVal(0.0f);

    double initPosX = levelConfig_[sName]["position.x"].getAsDouble();
    double initPosY = levelConfig_[sName]["position.y"].getAsDouble();
    double initPosZ = levelConfig_[sName]["position.z"].getAsDouble();

    Ogre::Vector3 posVector(initPosX, initPosY, initPosZ);
    Ogre::Vector3 dirVector(x, y, z);

    lTemp->setPosition(initPosX, initPosY, initPosZ);

    switch (lightType)
    {
    case 0:
      lTemp->setType(Ogre::Light::LT_POINT);
      break;
    case 1:
      lTemp->setType(Ogre::Light::LT_DIRECTIONAL);
      {
        Ogre::Vector3 diffVector = dirVector - posVector;
        diffVector.normalise();
        lTemp->setDirection(diffVector);
        GTRACE(4, "Setting direction for light [" << sName << "]: " << dirVector);
      }
      break;
    case 2:
      lTemp->setType(Ogre::Light::LT_SPOTLIGHT);
      {
        Ogre::Vector3 diffVector = dirVector - posVector;
        diffVector.normalise();
        lTemp->setDirection(diffVector);
        GTRACE(4, "Setting direction for light [" << sName << "]: " << dirVector);
      }

      lTemp->setSpotlightRange(Ogre::Degree(spotX), Ogre::Degree(spotY));
      break;
    }

    int nCastShadows = levelConfig_[sName]["castShadows"].getAsInt();
    lTemp->setCastShadows(nCastShadows != 0);

    x = levelConfig_[sName]["diffuseColor.x"].getAsDouble();
    y = levelConfig_[sName]["diffuseColor.y"].getAsDouble();
    z = levelConfig_[sName]["diffuseColor.z"].getAsDouble();
    lTemp->setDiffuseColour(x, y, z);

    x = levelConfig_[sName]["specularColor.x"].getAsDouble();
    y = levelConfig_[sName]["specularColor.y"].getAsDouble();
    z = levelConfig_[sName]["specularColor.z"].getAsDouble();
    lTemp->setSpecularColour(x, y, z);
  }


  void WorkbenchState::createLights()
  {
    GTRACE(3, "creating lights...");

    // Try to create a light if one doesn't exist already
    int indexFrom = levelConfig_["lighting"]["lightIndexFrom"].getAsInt();
    int indexTo = levelConfig_["lighting"]["lightIndexTo"].getAsInt();
    double x, y, z;

    try
    {
      for (int nIdx = indexFrom; nIdx <= indexTo; ++nIdx)
      {
        std::stringstream ss;
        ss << "light." << nIdx;
        std::string sName = ss.str();

        std::string sLightName = levelConfig_[sName]["name"].getAsStringOrDefaultVal("");
        if (sLightName.length() > 0)
        {
          Ogre::Light * lTemp;
          GTRACE(4, "Creating light for scene: " << sLightName);
          lTemp = enginePtr_->getSceneManagerPtr()->createLight(sLightName);

          applyLightSettingsFromCfg(sName);


          double initPosX = levelConfig_[sName]["position.x"].getAsDouble();
          double initPosY = levelConfig_[sName]["position.y"].getAsDouble();
          double initPosZ = levelConfig_[sName]["position.z"].getAsDouble();

          x = levelConfig_[sName]["lookAt.x"].getAsDouble();
          y = levelConfig_[sName]["lookAt.y"].getAsDouble();
          z = levelConfig_[sName]["lookAt.z"].getAsDouble();

          int lightType = levelConfig_[sName]["type"].getAsInt();

          Ogre::ManualObject* myManualObject;
          if (lightType != 0)
          {
            std::string lineName = sName;
            lineName.append(".line");
            std::string lineMaterial = lineName;
            lineMaterial.append(".material");
            myManualObject =  enginePtr_->getSceneManagerPtr()->createManualObject(lineName);
            Ogre::MaterialPtr myManualObjectMaterial =
                Ogre::MaterialManager::getSingleton().create(lineMaterial, staticGroupName_);
            myManualObjectMaterial->setReceiveShadows(false);
            myManualObjectMaterial->getTechnique(0)->setLightingEnabled(true);
            myManualObjectMaterial->getTechnique(0)->getPass(0)->setDiffuse(0, 0, 1, 0);
            myManualObjectMaterial->getTechnique(0)->getPass(0)->setAmbient(0, 0, 1);
            myManualObjectMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(0, 0, 1);
            myManualObject->begin(lineMaterial, Ogre::RenderOperation::OT_LINE_LIST);
            myManualObject->position(initPosX, initPosY, initPosZ);
            myManualObject->position(x, y, z);
            GTRACE(4, "Added light debug line from(" << initPosX << ", " << initPosY << ", " << initPosZ << ") to ("
              << x << ", " << y << ", " << z << ")");
            // etc
            myManualObject->end();
          }


          Ogre::BillboardSet* bbs = enginePtr_->getSceneManagerPtr()->createBillboardSet(sName+std::string("bbs"), 1);
          Ogre::ColourValue myCol(0.95f, 0.95f, 1.0f);
          bbs->createBillboard(Ogre::Vector3::ZERO, myCol);
          bbs->setMaterialName("Workbench/Flare");
          bbs->setCastShadows(false);

          {
            std::string nodeName = sName;
            nodeName.append("node");
            Ogre::SceneNode * myNode =
                enginePtr_->getSceneManagerPtr()->getRootSceneNode()->createChildSceneNode(nodeName);

            // Light attaching to node DISABLED.
            //myNode->attachObject(lTemp);
            if (lightType != 0)
            {
              myNode->attachObject(myManualObject);
            }

            std::string anonName = nodeName;
            anonName.append(".anonymous");
            Ogre::SceneNode * anonNode =
                enginePtr_->getSceneManagerPtr()->getRootSceneNode()->createChildSceneNode(anonName);

            anonNode->attachObject(bbs);
            anonNode->scale(0.2f, 0.2f, 0.2f);

            anonNode->translate(initPosX, initPosY, initPosZ);
            //myNode->addChild(anonNode);

            GTRACE(4, "Moved node " << nodeName);
          }
        }
      }
    }
    catch (const std::exception &e)
    {
      GTRACE(4, "Exception: " << e.what());
    }
  }


  void WorkbenchState::createCameras()
  {
    GTRACE(2, "Creating camera(s)");
    //  set camera properties
    Ogre::Camera * camPtr = enginePtr_->getMainCamera();
    Ogre::Camera * camSecPtr = enginePtr_->getSecondaryCamera();

    double x, y, z;
    x = levelConfig_["camera"]["pos.x"].getAsDoubleOrDefaultVal(0);
    y = levelConfig_["camera"]["pos.y"].getAsDoubleOrDefaultVal(0);
    z = levelConfig_["camera"]["pos.z"].getAsDoubleOrDefaultVal(0);

    camPtr->setPosition(x, y, z);
    if (camSecPtr)
    {
      camSecPtr->setPosition(x, y, z);
    }

    double clipNearValue = levelConfig_["camera"]["clipNear"].getAsDouble();
    if (clipNearValue > 0)
    {
      camPtr->setNearClipDistance(clipNearValue);
      if (camSecPtr)
      {
        camSecPtr->setNearClipDistance(clipNearValue);
      }
    }

    playerCamera_ = new GusCamera(camPtr);
    if (camSecPtr)
    {
      secondCamera_ = new GusCamera(camSecPtr);
    }

    playerCamera_->setZoomMax(1.0f);
    if (camSecPtr)
    {
      secondCamera_->setZoomMax(1.0f);
    }

    double zoomLevel = levelConfig_["camera"]["zoomLevel"].getAsDoubleOrDefaultVal(0);
    playerCamera_->setZoomLevel(zoomLevel);
    if (camSecPtr)
    {
      secondCamera_->setZoomLevel(zoomLevel);
    }

    double fieldOfView = levelConfig_["camera"]["fov"].getAsDoubleOrDefaultVal(50);
    playerCamera_->setFOV(fieldOfView);
    if (camSecPtr)
    {
      secondCamera_->setFOV(fieldOfView);
    }

    double zoomStep = levelConfig_["camera"]["zoomStep"].getAsDoubleOrDefaultVal(0.2);
    playerCamera_->setZoomStep(zoomStep);
    if (camSecPtr)
    {
      secondCamera_->setZoomStep(zoomStep);
    }

    x = levelConfig_["camera"]["target.x"].getAsDoubleOrDefaultVal(0);
    y = levelConfig_["camera"]["target.y"].getAsDoubleOrDefaultVal(0);
    z = levelConfig_["camera"]["target.z"].getAsDoubleOrDefaultVal(0);

    if (x != 0 || y != 0 || z != 0)
    {
      camPtr->lookAt(Ogre::Vector3(x, y, z));
      if (camSecPtr)
      {
        camSecPtr->lookAt(Ogre::Vector3(x, y, z));
      }
    }
    else
    {
      x = levelConfig_["camera"]["dir.x"].getAsDoubleOrDefaultVal(0);
      y = levelConfig_["camera"]["dir.y"].getAsDoubleOrDefaultVal(0);
      z = levelConfig_["camera"]["dir.z"].getAsDoubleOrDefaultVal(0);
      camPtr->setDirection(x, y, z);
      if (camSecPtr)
      {
        camSecPtr->setDirection(x, y, z);
      }
    }

    GTRACE(4, "Done creating camera(s)");
  }


  void WorkbenchState::createLevelObjects()
  {
    GTRACE(2, "Creating level objects...");

    // create additional level objects
    int indexFrom = levelConfig_["world"]["objectIndexFrom"].getAsInt();
    int indexTo = levelConfig_["world"]["objectIndexTo"].getAsInt();
    try
    {
      for (int nIdx = indexFrom; nIdx <= indexTo; ++nIdx)
      {
        std::stringstream ss;
        ss << "worldObject." << nIdx;
        std::string sName = ss.str();
        GTRACE(4, "Creating item: " << sName);

        int nType = levelConfig_[sName]["type"].getAsInt();
        double posX = levelConfig_[sName]["position.x"].getAsDouble();
        double posY = levelConfig_[sName]["position.y"].getAsDouble();
        double posZ = levelConfig_[sName]["position.z"].getAsDouble();

        double sizeWidth = levelConfig_[sName]["size.x"].getAsDouble();
        double sizeHeight = levelConfig_[sName]["size.z"].getAsDouble();
        int segsWidth = levelConfig_[sName]["seg.x"].getAsIntOrDefaultVal(1);
        int segsHeight = levelConfig_[sName]["seg.z"].getAsIntOrDefaultVal(1);

        std::string materialName = levelConfig_[sName]["material"].getAsString();
        int renderGroup = levelConfig_[sName]["renderGroup"].getAsIntOrDefaultVal(0);

        bool castsShadows = levelConfig_[sName]["castShadows"].getAsBoolOrDefault(true);
        GTRACE(4, "Item named " << sName << " will " <<(castsShadows?"":"NOT ") << "cast shadows.");

        double scaleX = levelConfig_[sName]["scale.x"].getAsDoubleOrDefaultVal(1.0);
        double scaleY = levelConfig_[sName]["scale.y"].getAsDoubleOrDefaultVal(1.0);
        double scaleZ = levelConfig_[sName]["scale.z"].getAsDoubleOrDefaultVal(1.0);

        switch (nType)
        {
          case 1:
            // mesh
            {
              std::string meshName = levelConfig_[sName]["mesh"].getAsString();

              if (meshName != "")
              {
                // create the entity, node and attach them to the scene.
                Ogre::Entity * eTemp;
                Ogre::MeshPtr ptrMesh;
                ptrMesh = Ogre::MeshManager::getSingletonPtr()->load(meshName, staticGroupName_);

                bool buildTangents;
                buildTangents = levelConfig_[sName]["buildTangentVectors"].getAsBoolOrDefault(false);
                if (buildTangents)
                {
                  unsigned short src, dest;
                  if (false == ptrMesh-> suggestTangentVectorBuildParams(Ogre::VES_TANGENT, src, dest))
                  {
                    ptrMesh->buildTangentVectors(Ogre::VES_TANGENT, src, dest);
                  }
                }
                eTemp = enginePtr_->getSceneManagerPtr()->createEntity(sName, meshName);
                std::string tempNodeName = sName;
                tempNodeName.append("node");
                Ogre::SceneNode *snTemp =
                    enginePtr_->getSceneManagerPtr()->getRootSceneNode()->createChildSceneNode(tempNodeName);

                snTemp->attachObject(eTemp);

                snTemp->setPosition(posX, posY, posZ);
                snTemp->scale(scaleX, scaleY, scaleZ);

                if (renderGroup!= 0)
                {
                  eTemp->setRenderQueueGroup(renderGroup);
                }
                if (materialName != "")
                {
                  eTemp->setMaterialName(materialName);
                }
                eTemp->setCastShadows(castsShadows);
                GTRACE(4, "Created entity for mesh object and applied shadow cast " << castsShadows);
              }
            }
            break;
          case 0:
          default:
            {
              // plane type
              std::string planeName(sName);
              planeName.append("plane");

              int planeNormalIdx = levelConfig_[sName]["normal"].getAsIntOrDefaultVal(0);
              Ogre::Vector3 planeNormalVector = Ogre::Vector3::UNIT_X;
              switch (planeNormalIdx)
              {
              case -3:
                planeNormalVector = Ogre::Vector3::NEGATIVE_UNIT_Z;
                break;
              case -2:
                planeNormalVector = Ogre::Vector3::NEGATIVE_UNIT_Y;
                break;
              case -1:
                planeNormalVector = Ogre::Vector3::NEGATIVE_UNIT_X;
                break;
              case 1:
                planeNormalVector = Ogre::Vector3::UNIT_X;
                break;
              case 2:
                planeNormalVector = Ogre::Vector3::UNIT_Y;
                break;
              case 3:
              default:
                planeNormalVector = Ogre::Vector3::UNIT_Z;
                break;
              }

              int planeUpIdx = levelConfig_[sName]["up"].getAsIntOrDefaultVal(0);
              Ogre::Vector3 planeUpVector = Ogre::Vector3::UNIT_X;
              switch (planeUpIdx)
              {
              case -3:
                planeUpVector = Ogre::Vector3::NEGATIVE_UNIT_Z;
                break;
              case -2:
                planeUpVector = Ogre::Vector3::NEGATIVE_UNIT_Y;
                break;
              case -1:
                planeUpVector = Ogre::Vector3::NEGATIVE_UNIT_X;
                break;
              case 1:
                planeUpVector = Ogre::Vector3::UNIT_X;
                break;
              case 2:
                planeUpVector = Ogre::Vector3::UNIT_Y;
                break;
              case 3:
              default:
                planeUpVector = Ogre::Vector3::UNIT_Z;
                break;
              }

              GTRACE(4, "Planar object " << sName << " will use normal vector : " << planeNormalVector);
              GTRACE(4, "Planar object " << sName << " will use up vector : " << planeUpVector);

              Ogre::MovablePlane* myPlane = new Ogre::MovablePlane(planeName);
              myPlane->normal = planeNormalVector;
              myPlane->d = posY;

              if (mPlane == 0)
              {
                mPlane = myPlane;
              }

              std::string planeMeshName(sName);
              planeMeshName.append("mesh");
              Ogre::MeshManager::getSingleton().createPlane(
                  planeMeshName,
                  staticGroupName_, *myPlane,
                  sizeWidth, sizeHeight,
                  segsWidth, segsHeight,
                  true, 1, 1, 1, planeUpVector);

              GTRACE(4, "Created plane mesh " << planeMeshName << " size " << sizeWidth << "x" << sizeHeight
                << "; segments " << segsWidth << "x" << segsHeight);
              // create the entity, node and attach them to the scene.
              Ogre::Entity * eTemp;
              eTemp = enginePtr_->getSceneManagerPtr()->createEntity(sName, planeMeshName);
              std::string tempNodeName = sName;
              tempNodeName.append("node");
              Ogre::SceneNode *snTemp =
                  enginePtr_->getSceneManagerPtr()->getRootSceneNode()->createChildSceneNode(tempNodeName);
              snTemp->attachObject(eTemp);

              snTemp->translate(posX, 0, posZ);
              if (renderGroup!= 0)
              {
                eTemp->setRenderQueueGroup(renderGroup);
              }
              if (materialName != "")
              {
#if 0
                Ogre::MaterialPtr matPtr = Ogre::MaterialManager::getSingletonPtr()->getByName(materialName);
                matPtr->setReceiveShadows(!castsShadows);
                eTemp->setMaterial(matPtr);
#else
                eTemp->setMaterialName(materialName);
#endif
              }

              eTemp->setCastShadows(castsShadows);
              GTRACE(4, "Created entity for plane and applied shadow cast " << castsShadows);
            }
            break;
        }
      }
    }
    catch (const std::exception &e)
    {
      GTRACE(4, "Exception: " << e.what());
    }
    GTRACE(4, "Done creating level objects");
  }

  void WorkbenchState::applyShadowTextureCasterMaterial(const std::string& materialName)
  {
    if (materialName.length() <= 0)
    {
      GTRACE(2, "Setting shadow texture caster material to empty");
      enginePtr_->getSceneManagerPtr()->setShadowTextureCasterMaterial(Ogre::StringUtil::BLANK);
      return;
    }

    if (currentShadowTech_ >= 3)
    {
      if (false == Ogre::MaterialManager::getSingletonPtr()->getByName(materialName).isNull())
      {
        enginePtr_->getSceneManagerPtr()->setShadowTextureCasterMaterial(materialName);
        GTRACE(3, "Set shadow caster material to [" << materialName << "]");
      }
      else
      {
        GTRACE(2, "WARNING: Could not set shadow caster material to [" << materialName << "] : not found");
      }
    }
    else
    {
      GTRACE(3, "Not applying shadow texture caster material; IGNORED due to current shadow tech: "
          << currentShadowTech_);
    }
  }

  void WorkbenchState::applyShadowTextureReceiverMaterial(const std::string& materialName)
  {
    if (materialName.length() <= 0)
    {
      GTRACE(2, "Setting shadow texture receiver material to empty");
      enginePtr_->getSceneManagerPtr()->setShadowTextureReceiverMaterial(Ogre::StringUtil::BLANK);
      return;
    }

    if (currentShadowTech_ >= 3)
    {
      if (false == Ogre::MaterialManager::getSingletonPtr()->getByName(materialName).isNull())
      {
        enginePtr_->getSceneManagerPtr()->setShadowTextureReceiverMaterial(materialName);
        GTRACE(3, "Set shadow receiver material to [" << materialName << "]");
      }
      else
      {
        GTRACE(2, "WARNING: Could not set shadow receiver material to [" << materialName << "] : not found");
      }
    }
  }


  /**
    Apply a sky-box to the scene.
  */
  void WorkbenchState::applySkyBox()
  {
    // Use a skybox if required. Loaded from the level.
    std::string sTemp = levelConfig_["general"]["skyBoxName"].getAsString();
    double dTemp = levelConfig_["general"]["skyBoxDist"].getAsDouble();

    GTRACE(4, "Loading skybox...:" << sTemp);

    if (sTemp != "")
    {
      enginePtr_->getSceneManagerPtr()->setSkyBox(
          true,             // enable
          sTemp,            // material
          dTemp,            // dist
          true,             // draw first
          Ogre::Quaternion::IDENTITY,   // orientation
          staticGroupName_);  // resource group name.
    }
  }


  void WorkbenchState::readAndApplyShadowCasterMaterial()
  {
    std::string sCurrentCameraSetupMat = "";
    {
      std::stringstream ss;
      ss << "casterPerShadow." << currentCameraSetup_;
      sCurrentCameraSetupMat = ss.str();
    }
    std::string currentShadowMaterialForCamera = levelConfig_["shadowcaster"][sCurrentCameraSetupMat].getAsString();
    applyShadowTextureCasterMaterial(currentShadowMaterialForCamera);
  }
}
