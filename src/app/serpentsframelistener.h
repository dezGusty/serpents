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
// Other libraries' headers
//

#include "OIS.h"

//
// This project's headers
//

// Forwards the inclusion of ogre.h
#include "config/serpentsogre.h"

namespace app
{
  //
  //    Forward class declarations.
  //
  class SerpEngine;


  /// The base class for frame listeners in the base framework
  class SerpFrameListener
    : public Ogre::FrameListener
    , public Ogre::WindowEventListener
  {
  protected:
    Ogre::RenderWindow* renderWindowPtr_;

    //  OIS Input devices
    OIS::InputManager* inputManagerPtr_;
    OIS::Mouse*    mousePtr_;
    OIS::Keyboard* keyboardPtr_;

    SerpEngine * enginePtr_;

    bool activeWindow_;

  public:
    ///  Constructor.
    SerpFrameListener(
        SerpEngine *engine,
        Ogre::RenderWindow *win,
        bool bufferedKeys = false,
        bool bufferedMouse = false);

    ///  Destructor.
    virtual ~SerpFrameListener(void);


    /**
      Resize the window. Adjust mouse clipping area.
    */
    virtual void windowResized(Ogre::RenderWindow *rw);


    /**
      Close the window. Unattach OIS before window shutdown (very important under Linux).
    */
    virtual void windowClosed(Ogre::RenderWindow *rw);


    /**
      React to the window losing the focus.
      (You could trigger non-rendering functions here, such as pausing or resuming the music.
      Keep in mind synchronization issues!)
    */
    virtual void windowFocusChange(Ogre::RenderWindow* rw);


    /**
      A "frame started rendering" callback. This is where most of the drawing is done, or should be done.
      Typically, called from SerpEngine -> renderCompleteScene
    */
    virtual bool frameStarted(const Ogre::FrameEvent &evt);


    /**
      The ending part of a frame rendering. This is where the overlay items should be shown.
    */
    virtual bool frameEnded(const Ogre::FrameEvent &evt);

    /// Getter for the OIS Keyboard.
    OIS::Keyboard* getOISKeyboard()
    {
      return keyboardPtr_;
    }

    /// Getter for the OIS Mouse.
    OIS::Mouse* getOISMouse()
    {
      return mousePtr_;
    }

    /// Getter for the render window.
    Ogre::RenderWindow* getRenderWindow()
    {
      return renderWindowPtr_;
    }

    virtual void releaseEngine();
  };
}
