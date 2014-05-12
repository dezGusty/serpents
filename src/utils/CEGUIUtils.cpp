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

#include "CEGUIUtils.h"
#include <guslib/trace/Trace.h>

namespace utils
{
    SimpleListItem::SimpleListItem (const CEGUI::String& text, const std::string& schemaToUse
            , const std::string& brushToUse, const std::string& customData, unsigned int id)
      : ListboxTextItem (text, id)
    {
      GTRACE (5, "Adding a new simple item: " << text << ", schema/brush: [" << schemaToUse << "/" << brushToUse <<"]");
      std::string localCopy = schemaToUse;
      localCopy.append ("/");
      localCopy.append (brushToUse);
      const CEGUI::Image* sel_img = &CEGUI::ImageManager::getSingleton().get(localCopy);
      setSelectionBrushImage (sel_img);
      setAutoDeleted (true);
      setText (text);
    }

    SimpleListItem::SimpleListItem (const CEGUI::String& text, const std::string& schemaToUse
            , const std::string& brushToUse, const CEGUI::Colour& selectionColour, const std::string& customData, unsigned int id)
      : ListboxTextItem (text, id)
    {
      GTRACE (5, "Adding a new simple item: " << text << ", schema/brush: [" << schemaToUse << "/" << brushToUse <<"]");
      std::string localCopy = schemaToUse;
      localCopy.append ("/");
      localCopy.append (brushToUse);
      const CEGUI::Image* sel_img = &CEGUI::ImageManager::getSingleton().get(localCopy);
      setSelectionBrushImage (sel_img);
      setAutoDeleted (true);
      setText (text);
      setSelectionColours (selectionColour);
    }


    /**
      Add a child widget to a CEGUI widget
      @param parentPtr The parent pointer to the CEGUI Window
      @param childPtr The child pointer to the CEGUI Window.
    */
    void CEGUIHelperUtil::addChildWidget (CEGUI::Window* parentPtr, CEGUI::Window* childPtr)
    {
      parentPtr->addChild (childPtr);
    }


    /**
      Remove a child widget from a CEGUI widget
      @param parentPtr The parent pointer to the CEGUI Window
      @param childPtr The child pointer to the CEGUI Window.
    */
    void CEGUIHelperUtil::removeChildWidget (CEGUI::Window* parentPtr, CEGUI::Window* childPtr)
    {
      parentPtr->removeChild (childPtr);
    }


    // static
    CEGUI::Window* CEGUIHelperUtil::getWindow (const CEGUI::String& name)
    {
      CEGUI::System* ceguiSysPtr_ = CEGUI::System::getSingletonPtr ();
      return getWindowForSystem (ceguiSysPtr_, name);
    }

    // static
    CEGUI::Window* CEGUIHelperUtil::getWindowForSystem (CEGUI::System* sys, const CEGUI::String& name)
    {
      CEGUI::Window* root = sys->getDefaultGUIContext().getRootWindow();

      if (root->isChild (name))
      {
        return root->getChild (name);
      }
      else
      {
        if (root->getName () == name)
        {
          return root;
        }
        else
        {
          GTRACE (3, "Could not find child [" << name << "] within " << root->getChildCount () << " children");
        }
      }

      return 0;
    }
    

    CEGUI::Window* CEGUIHelperUtil::getWindowForLoadedLayout (CEGUI::Window* parentWnd, const CEGUI::String& name)
    {
      if (0 == parentWnd)
      {
        return 0;
      }

      if (parentWnd->isChild (name))
      {
        return parentWnd->getChild (name);
      }
      else
      {
        if (parentWnd->getName () == name)
        {
          return parentWnd;
        }
        else
        {
          GTRACE (3, "Could not find child [" << name << "] within " << parentWnd->getChildCount () << " children");
        }
      }

      return 0;
    }

    void CEGUIHelperUtil::dumpFullWindowToLog (CEGUI::Window* startingWindow, int level)
    {
      if (0 == startingWindow)
      {
        return;
      }

      // Write the current element.
      std::stringstream ss;
      ss << level << ":";
      for (int i = 0; i < level ; ++ i)
      {
        ss << "    ";
      }
  
      int numChildren = startingWindow->getChildCount ();
      GTRACE (4, ss.str ().c_str () << startingWindow->getName().c_str () << " ("<< numChildren<< " children)");

      for (int i = 0; i < numChildren; ++ i)
      {
        CEGUI::Window* childPtr = startingWindow->getChildAtIdx (i);
        dumpFullWindowToLog (childPtr, level + 1);
      }
    }

    bool CEGUIHelperUtil::isWindowPresent (const CEGUI::String& name)
    {
      return (0 != getWindow (name));
    }

    bool CEGUIHelperUtil::isWindowPresentForSystem (CEGUI::System* sys, const CEGUI::String& name)
    {
      return (0 != getWindowForSystem (sys, name));
    }

    CEGUI::Window* CEGUIHelperUtil::loadLayoutFromFile (const CEGUI::String& name)
    {
      CEGUI::WindowManager& win_mgr = CEGUI::WindowManager::getSingleton ();
      return win_mgr.loadLayoutFromFile (name);
    }
    

    void CEGUIHelperUtil::injectKeyDown (OIS::KeyCode myKey)
    {
      injectKeyDown (CEGUI::System::getSingletonPtr(), myKey);
    }

    void CEGUIHelperUtil::injectKeyDown (CEGUI::System* sys, OIS::KeyCode myKey)
    {
      if (!sys)
      {
        return;
      }

      sys->getDefaultGUIContext ().injectKeyDown (static_cast<CEGUI::Key::Scan> (myKey));
    }

    void CEGUIHelperUtil::injectKeyUp (OIS::KeyCode myKey)
    {
      injectKeyUp (CEGUI::System::getSingletonPtr(), myKey);
    }


    void CEGUIHelperUtil::injectKeyUp (CEGUI::System* sys, OIS::KeyCode myKey)
    {
      if (!sys)
      {
        return;
      }

      sys->getDefaultGUIContext ().injectKeyUp (static_cast<CEGUI::Key::Scan> (myKey));
    }


    void CEGUIHelperUtil::injectChar (int myKey)
    {
      injectChar (CEGUI::System::getSingletonPtr(), myKey);
    }


    void CEGUIHelperUtil::injectChar (CEGUI::System* sys, int myKey)
    {
      if (!sys)
      {
        return;
      }

      sys->getDefaultGUIContext ().injectChar (static_cast<CEGUI::Key::Scan> (myKey));
    }


    void CEGUIHelperUtil::injectMousePosition (float x, float y)
    {
      injectMousePosition (CEGUI::System::getSingletonPtr(), x, y);
    }

    void CEGUIHelperUtil::injectMousePosition (CEGUI::System* sys, float x, float y)
    {
      if (!sys)
      {
        return;
      }

      sys->getDefaultGUIContext ().injectMousePosition (x, y);
    }

    void CEGUIHelperUtil::injectMouseWheelChange (float pos)
    {
      injectMouseWheelChange (CEGUI::System::getSingletonPtr(), pos);
    }

    void CEGUIHelperUtil::injectMouseWheelChange (CEGUI::System* sys, float pos)
    {
      if (!sys)
      {
        return;
      }

      sys->getDefaultGUIContext ().injectMouseWheelChange (pos);
    }

    void CEGUIHelperUtil::injectMouseButtonDown (OIS::MouseButtonID btn)
    {
      injectMouseButtonDown (CEGUI::System::getSingletonPtr(), btn);
    }

    void CEGUIHelperUtil::injectMouseButtonDown (CEGUI::System* sys, OIS::MouseButtonID btn)
    {
      if (!sys)
      {
        return;
      }

      sys->getDefaultGUIContext ().injectMouseButtonDown (convertOISButtonToCegui (btn));
    }

    void CEGUIHelperUtil::injectMouseButtonUp (OIS::MouseButtonID btn)
    {
      injectMouseButtonUp (CEGUI::System::getSingletonPtr(), btn);
    }

    void CEGUIHelperUtil::injectMouseButtonUp (CEGUI::System* sys, OIS::MouseButtonID btn)
    {
      if (!sys)
      {
        return;
      }

      sys->getDefaultGUIContext ().injectMouseButtonUp (convertOISButtonToCegui (btn));
    }

    void CEGUIHelperUtil::injectTimePulse (float timeElapsed)
    {
      injectTimePulse (CEGUI::System::getSingletonPtr(), timeElapsed);
    }


    void CEGUIHelperUtil::injectTimePulse (CEGUI::System* sys, float timeElapsed)
    {
      if (!sys)
      {
        return;
      }

      sys->injectTimePulse (timeElapsed);
      sys->getDefaultGUIContext ().injectTimePulse (timeElapsed);
    }


    CEGUI::MouseButton CEGUIHelperUtil::convertOISButtonToCegui (int buttonID)
    {
      switch (buttonID)
      {
      case OIS::MB_Left:
        return CEGUI::LeftButton;
      case OIS::MB_Right:
        return CEGUI::RightButton;
      case OIS::MB_Middle:
        return CEGUI::MiddleButton;
      }
      // default:
      return CEGUI::LeftButton;
    }


    std::string CEGUIHelperUtil::getNameForWidget (const std::string& name)
    {
      return name;
    }


    void CEGUIHelperUtil::setScrollPositionForWidget (const CEGUI::String& widgetName, float newScrollPosition)
    {
      CEGUI::Scrollbar* scroller;
      scroller = static_cast<CEGUI::Scrollbar*> (utils::CEGUIHelperUtil::getWindow (widgetName));
      if (scroller)
      {
        scroller->setUnitIntervalScrollPosition (0.5f);
      }
    }



    void CEGUIHelperUtil::setDefaultFont (const CEGUI::String& fontName)
    {
      if (CEGUI::FontManager::getSingleton ().isDefined (fontName))
      {
        CEGUI::System::getSingletonPtr ()->getDefaultGUIContext ().setDefaultFont ((CEGUI::utf8*)fontName.c_str ());
        GTRACE (5, "CEGUIHelper: Feature not supported: setDefaultFont(" << fontName << ")");
      }
    }

}

