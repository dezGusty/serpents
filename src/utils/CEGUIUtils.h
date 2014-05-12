#pragma once

//   This file is part of Gusty's Serpents, licensed under the terms of the MIT License
//   (further described below).
//
//   Copyright (C) 2009-2012  Augustin Preda (thegusty999@gmail.com)
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
//
//    CEGUI Utility classes.
//
//    Last change:  $LastChangedDate$
//    Revision:    $Revision$

#ifdef CEGUI_07
#include "CEGUI.h"
#else
#include "cegui/CEGUI.h"
#endif

#include <OIS.h>

namespace utils
{
  class SimpleListItem
    : public CEGUI::ListboxTextItem
  {
  protected:
    // allows you to store custom data.
    std::string containedData_;

  public:
    SimpleListItem (const CEGUI::String& text
            , const std::string& schemaToUse, const std::string& brushToUse
            , const std::string& customData, unsigned int id=0);

    SimpleListItem (const CEGUI::String& text
            , const std::string& schemaToUse, const std::string& brushToUse
            , const CEGUI::Colour& selectionColour
            , const std::string& customData, unsigned int id=0);

    const std::string& getContainedData () const { return containedData_; }
    void setContainedData (const std::string& customData)  { containedData_ = customData; }
  };


  /**
    A class to be used as a namespace placeholder for most CEGUI functionality.
    This is to alleviate some of the issues caused by a change in the API syntax between various versions of CEGUI.

  */
  class CEGUIHelperUtil
  {
  public:

    /**
      Add a child widget to a CEGUI widget
      @param parentPtr The parent pointer to the CEGUI Window
      @param childPtr The child pointer to the CEGUI Window.
    */
    static void addChildWidget (CEGUI::Window* parentPtr, CEGUI::Window* childPtr);

    /**
      Remove a child widget from a CEGUI widget
      @param parentPtr The parent pointer to the CEGUI Window
      @param childPtr The child pointer to the CEGUI Window.
    */
    static void removeChildWidget (CEGUI::Window* parentPtr, CEGUI::Window* childPtr);

    static CEGUI::MouseButton convertOISButtonToCegui (int buttonID);

    // Make sure that the FULL widget name (including path) can be used easily in CEGUI_07 as well.
    static std::string getNameForWidget (const std::string& name);

    static CEGUI::Window* getWindow (const CEGUI::String& name);
    static CEGUI::Window* getWindowForSystem (CEGUI::System* sys, const CEGUI::String& name);
    static CEGUI::Window* getWindowForLoadedLayout (CEGUI::Window* parentWnd, const CEGUI::String& name);

    /// @param startingWindow: the window from where the dump to log shall commence. For 
    /// a full system dump, use the root window.
    /// @param level: the amount of indentation levels to use.
    static void dumpFullWindowToLog (CEGUI::Window* startingWindow, int level = 0);

    static bool isWindowPresent (const CEGUI::String& name);
    static bool isWindowPresentForSystem (CEGUI::System* sys, const CEGUI::String& name);

    static CEGUI::Window* loadLayoutFromFile (const CEGUI::String& name);

    static void injectKeyDown (OIS::KeyCode myKey);
    static void injectKeyDown (CEGUI::System* sys, OIS::KeyCode myKey);

    static void injectKeyUp (OIS::KeyCode myKey);
    static void injectKeyUp (CEGUI::System* sys, OIS::KeyCode myKey);

    static void injectChar (int myKey);
    static void injectChar (CEGUI::System* sys, int myKey);
    
    static void injectMousePosition (float x, float y);
    static void injectMousePosition (CEGUI::System* sys, float x, float y);

    static void injectMouseWheelChange (float pos);
    static void injectMouseWheelChange (CEGUI::System* sys, float pos);

    static void injectMouseButtonDown (OIS::MouseButtonID btn);
    static void injectMouseButtonDown (CEGUI::System* sys, OIS::MouseButtonID btn);

    static void injectMouseButtonUp (OIS::MouseButtonID btn);
    static void injectMouseButtonUp (CEGUI::System* sys, OIS::MouseButtonID btn);

    static void injectTimePulse (float timeElapsed);
    static void injectTimePulse (CEGUI::System* sys, float timeElapsed);

    static void setScrollPositionForWidget (const CEGUI::String& widgetName, float newScrollPosition);


    static void setDefaultFont (const CEGUI::String& fontName);
  };
}
