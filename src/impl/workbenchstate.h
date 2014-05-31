#pragma once

//    This file is part of Gusty's Serpents
//    Copyright (C) 2009-2012  Augustin Preda (thegusty999@gmail.com)
//
//    Gusty's Serpents is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    -------------------------------------------------------------------------
//
//    The state that handles the work bench where things are tested.
//
//    Last change:  $LastChangedDate: 2011-09-12 16:50:43 +0300 (L, 12 sep. 2011) $
//    Revision:    $Revision: 278 $

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
#include "guslib/common/smartptr.h"
#include "guslib/util/configuration.h"

#include "OgreCommon.h"

#ifdef CEGUI_07
#include "CEGUI.h"
#else
#include "cegui/CEGUI.h"
#endif


//
// This project's headers
//

#include "app/serpstate.h"


namespace Serpents
{
  // Forward class declarations.
  class GusCamera;

  /**
    The class to be used as testing grounds for new functionality.
  */
  class WorkbenchState
    : public app::SerpState
  {
  private:
    bool initialized_;
    bool initialize();

    guslib::Configuration levelConfig_;
    GSmartPtr<GusCamera> playerCamera_;
    GSmartPtr<GusCamera> secondCamera_;

    bool canRotate_;
    int oldMouseX_;
    int oldMouseY_;
    bool movingCameraForward_;
    bool movingCameraBack_;
    bool movingCameraLeft_;
    bool movingCameraRight_;

    bool movingCameraFaster_;
    bool movingCameraSlower_;

    std::string staticGroupName_;
    std::string expGroupName_;

    std::string reloadGroupNextTick_;

    int shadowTechMinimum_;
    int shadowTechMaximum_;
    int currentShadowTech_;
    bool applyShadowTechNextTick_;

    int cameraSetupMinimum_;
    int cameraSetupMaximum_;
    int currentCameraSetup_;
    bool applyCameraSetupNextTick_;

    bool rendererIsOpenGL_;
    bool compositorsAreEnabled_;

    void applyShadowTextureCasterMaterial(const std::string& materialName);
    void applyShadowTextureReceiverMaterial(const std::string& materialName);

    /**
      Apply a sky-box to the scene.
    */
    void applySkyBox();

    /**
      Reload a resource group, identified by its name.
      @param resourceGroupName The name of the resoruce group to reload.
    */
    void reloadResourceGroup(const char* resourceGroupName);

    /**
      TEMPORARY ONLY. Only for DEMO purposes.
      Populate the contents of a Multi Column List with hard-coded values.
    */
    void populateHardCodedMCLValues(CEGUI::MultiColumnList* mclPtr, const char* brushName);

     /**
      TEMPORARY ONLY. Only for DEMO purposes.
      Populate the contents of a Combo Box with hard-coded values.
    */
    void populateHardCodedComboValues(CEGUI::Combobox* comboPtr, const char* brushName);

    Ogre::MovablePlane* mPlane;

    CEGUI::Window* mContextMenuPtr;

    std::string preferredLevel_;

    std::string preferredSkin_;

    bool m_secondaryViewActive;
    std::string m_preferredSkinConfig;

    void applyLightSettingsFromCfg(const std::string& lightCfgEntry);
    void readAndApplyShadowCasterMaterial();

    void addCompositorByName(const std::string& name, Ogre::Viewport* vp);
    void removeCompositorByName(const std::string& name, Ogre::Viewport* vp);

    void addSceneCompositors();
    void removeSceneCompositors();

    //
    // Connect the mouse hover and mouse click events for a particular widget.
    //
    bool addWidgetMouseEffects(
        const std::string& widget_name,
        bool apply_hover_effect = true,
        bool apply_click_effect = true);

  public:
    // TODO(Augustin Preda, 2014.05.31): make protected, add friend factory
    WorkbenchState(const std::string& name, const std::string& prefLvl, app::SerpEngine* ptr = 0);

    // The overrides for the state handling
    virtual void enter(const app::SerpStateParams& param);
    virtual void exit();

    virtual void pause();
    virtual void resume();

    virtual void setCompositorEnabledStatus(bool bCompositorStatus = true);

    virtual void reactToKeyPress(OIS::KeyCode keycode);
    virtual void reactToKeyRelease(OIS::KeyCode keycode);
    virtual void reactToMouseMoved(int x, int y);
    virtual void reactToMousePressed(const OIS::MouseButtonID& btn, int x, int y);
    virtual void reactToMouseReleased(const OIS::MouseButtonID& btn, int x, int y);

    virtual bool handleLogicThreadLoop();
    virtual bool renderScene();
    virtual bool renderGUI();
    virtual void notifyOfShutdown();

    void setupSecondaryView(const std::string& hostWidgetName);

    void applyShadowTechnique(Ogre::ShadowTechnique st);
    void applyShadowTechnique(int st);
    void createLevelObjects();
    void createLights();
    void createCameras();

    void moveToNextShadowTechnique();
    void moveToPrevShadowTechnique();

    void moveToNextCameraSetup();
    void moveToPrevCameraSetup();

    bool onButtonQuitClicked(const CEGUI::EventArgs& evt);
    bool onButtonOptionsClicked(const CEGUI::EventArgs& evt);
    bool onButtonOptionsCloseClicked(const CEGUI::EventArgs& evt);
    bool onButtonOptionsApplyClicked(const CEGUI::EventArgs& evt);
    bool onButtonSubmenuClicked(const CEGUI::EventArgs& evt);
    bool onButtonReloadGroupClicked(const CEGUI::EventArgs& evt);
    bool onButtonNextShadowTech(const CEGUI::EventArgs& evt);
    bool onButtonPrevShadowTech(const CEGUI::EventArgs& evt);
    bool onScrollValueChange(const CEGUI::EventArgs& evt);
    bool onButtonGetRandomValueClicked(const CEGUI::EventArgs& evt);

    bool onWidgetHover(const CEGUI::EventArgs& evt);
    bool onWidgetClick(const CEGUI::EventArgs& evt);
  };
}
