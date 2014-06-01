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
#include "app/serpentsframelistener.h"

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

// Engine class has a fwd declaration in the header, so it must be included here.
#include "app/serpentsengine.h"

namespace app
{
  SerpFrameListener::SerpFrameListener(
      SerpEngine *engine,
      Ogre::RenderWindow *win,
      bool bufferedKeys,
      bool bufferedMouse)
      : activeWindow_(true),
        enginePtr_(engine),
        renderWindowPtr_(win),
        inputManagerPtr_(0),
        mousePtr_(0),
        keyboardPtr_(0)
  {
    GTRACE(3, "SerpFrameListener CTor");

    OIS::ParamList params;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    win->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    params.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    // Insert additional properties to have the mouse as NON-exclusive.
    // (meaning one can move the mouse freely outside the window).
    params.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND")));
    params.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
    params.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND")));
    params.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE")));

    inputManagerPtr_ = OIS::InputManager::createInputSystem(params);

    // Create all devices
    keyboardPtr_ = static_cast<OIS::Keyboard*>(inputManagerPtr_->createInputObject(OIS::OISKeyboard, bufferedKeys));
    mousePtr_ = static_cast<OIS::Mouse*>(inputManagerPtr_->createInputObject(OIS::OISMouse, bufferedMouse));

    //  Set initial mouse clipping size
    windowResized(renderWindowPtr_);
    keyboardPtr_->setBuffered(true);
    keyboardPtr_->setTextTranslation(OIS::Keyboard::Ascii);
    keyboardPtr_->setEventCallback(enginePtr_);

    mousePtr_->setBuffered(true);
    mousePtr_->setEventCallback(enginePtr_);
    
    // Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(renderWindowPtr_, this);
  }


  SerpFrameListener::~SerpFrameListener()
  {
    // call the engine destruction if it is not yet destroyed.

    GTRACE(3, "SerpFrameListener destructor called...");
    if (enginePtr_)
    {
      enginePtr_->releaseFrameListener();
    }

    if (enginePtr_) 
    {  
      enginePtr_->shutDown();
    }

    if (enginePtr_) 
    {
      releaseEngine();
    }

    // Remove the current object as a Window listener
    if (renderWindowPtr_)
    {
      Ogre::WindowEventUtilities::removeWindowEventListener(renderWindowPtr_, this);
      windowClosed(renderWindowPtr_);
    }

    GTRACE(3, "SerpFrameListener destructor exiting...");
  }


  void SerpFrameListener::releaseEngine()
  {
    GTRACE(4, "GBaseFrameListener received command to release engine."
        << "Setting ptr to engine to NULL.(Engine not deleted).");
    enginePtr_ = NULL;
  }


  void SerpFrameListener::windowResized(Ogre::RenderWindow* rw)
  {
    if (rw == renderWindowPtr_)
    {
      if (enginePtr_ == NULL)
      {
        unsigned int width, height, depth;
        int left, top;
        rw->getMetrics(width, height, depth, left, top);
        const OIS::MouseState &ms = mousePtr_->getMouseState();
        ms.width = width;
        ms.height = height;
      }
      else
      {
        unsigned int width, height, depth;
        int left, top;
        rw->getMetrics(width, height, depth, left, top);
        const OIS::MouseState &ms = mousePtr_->getMouseState();
        ms.width = width;
        ms.height = height;
      }
    }
  }


  void SerpFrameListener::windowClosed(Ogre::RenderWindow* rw)
  {
    GTRACE(3, "GBaseFrameListener windowClosed called...");

    // Only close for renderwindow
    if (rw && rw == renderWindowPtr_)
    {
      if (enginePtr_ != NULL)
      {
        enginePtr_->shutDown();
        GTRACE(3, "GBaseFrameListener sent shutdown cmd to engine...");
      }

      GUS_THREAD_SLEEP(100);  // TODO(Augustin Preda, 2014.06.01): get from config at start-up.

      // The input devices should probably wait for a loop of the input reading thread to finish.

      GTRACE(4, "destroying input manager and objects");
      if (inputManagerPtr_)
      {
        inputManagerPtr_->destroyInputObject(mousePtr_);
        inputManagerPtr_->destroyInputObject(keyboardPtr_);

        OIS::InputManager::destroyInputSystem(inputManagerPtr_);
        inputManagerPtr_ = NULL;
        keyboardPtr_ = NULL;
        mousePtr_ = NULL;
      }

      GTRACE(4, "finished destroying input manager and objects");
      renderWindowPtr_ = NULL;
    }
  }


  void SerpFrameListener::windowFocusChange(Ogre::RenderWindow* rw)
  {
    activeWindow_ = !activeWindow_;
    if (rw == renderWindowPtr_)
    {
      enginePtr_->setWindowIsActive(activeWindow_);
    }
  }

  /**
    A "frame started rendering" callback. This is where most of the drawing is done, or should be done.
    Typically, called from SerpEngine -> renderCompleteScene
  */
  bool SerpFrameListener::frameStarted(const Ogre::FrameEvent &evt)
  {
    GTRACE(5, "SerpFrameListener::frameStarted");
    if (NULL == renderWindowPtr_)
    {
      return false;
    }

    if (renderWindowPtr_->isClosed())
    {
      return false;
    }

    bool result(true);

    if (enginePtr_)
    {
      // Call the scene rendering here.
      result = enginePtr_->renderScene(evt.timeSinceLastFrame);
    }

    GTRACE(5, "SerpFrameListener::frameStarted exit");
    return result;
  }

  bool SerpFrameListener::frameEnded(const Ogre::FrameEvent &evt)
  {
    GTRACE(5, "SerpFrameListener::frameEnded");
    if (renderWindowPtr_ == NULL)
    {
      return false;
    }

    if (this->renderWindowPtr_->isClosed())
    {
      return false;
    }

    bool result = true;
    if (enginePtr_ != NULL)
    {
      result = enginePtr_->renderGUI(evt.timeSinceLastFrame);
    }

    GTRACE(5, "SerpFrameListener::frameEnded exit");
    return result;
  }
}
