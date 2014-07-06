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
//    Menu State handling class.
//
//    Last change:  $LastChangedDate$
//    Revision:    $Revision$

//
// Includes
//

//
// C++ system headers.
//

//
// Other libraries' headers
//
#include "OIS.h"

//
// This project's headers
//

// Extend the basic state
#include "app/serpentsstate.h"

// Allow a 3D scene to be displayed
#include "engine/scene.h"

namespace Serpents
{
  class MenuState
    : public app::SerpState
  {
  private:
    bool initialized_;
    bool initialize();

    void quitApp();
    void newGame();

    app::SerpStateParams enteringParams;

    Serpents::Scene* scene_;

    /**
      Save engine configuration.
    */
    void saveConfiguration();

    class GUIInternals;
    GUIInternals* guiInternals_;

  public:
    /**
      Constructor.
    */
    MenuState(
      const char* name,
      app::SerpEngine *enginePtr);

    /**
      Destructor.
    */
    virtual ~MenuState();

    //
    // The overrides for the state handling
    //

    // Handle state initialization.
    virtual void enter(const app::SerpStateParams& params) override;
    virtual void exit();

    virtual void pause();
    virtual void resume();

    //
    // The overrides to handle input
    //
    virtual void reactToKeyPress(OIS::KeyCode keycode);
    virtual void reactToKeyRelease(OIS::KeyCode keycode);

    virtual void reactToMouseMoved(int x, int y) override;
    virtual void reactToMousePressed(const OIS::MouseButtonID& btn, int x, int y) override;
    virtual void reactToMouseReleased(const OIS::MouseButtonID& btn, int x, int y) override;

    virtual bool handleLogicThreadLoop();
    virtual bool renderScene();
    virtual bool renderGUI();
    virtual void notifyOfShutdown();
  };
}
