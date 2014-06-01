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

#include "cegui/CEGUI.h"
#include "CEGUI/RendererModules/Ogre/Renderer.h"


namespace app
{
  /**
    Utility class to encapsulate a 3rd party library for dealing with the GUI.
  */
  class SerpGUI
  {
  protected:
    CEGUI::System* ceguiSysPtr_;
    std::string resourceGroupName_;

  public:
    SerpGUI(Ogre::RenderTarget* targetPtr, const std::string& logFileName, int logLevel = 2);

    virtual ~SerpGUI();

    void addFont(const std::string& fontName);
    void setDefaultFont(const std::string& fontName);

    void setMouseCursor(const std::string& cursorName);

    virtual void initialize();

    virtual void loadSchemes(std::vector <std::string> schemeList);

    virtual void renderGUI(float timeSinceLastRender = 1.0f);

    /**
      Commence the API Logging.
      @param fileName The name of the file to write to. It should be in the working directory.
    */
    static void startLogging(const std::string& fileName);
  };
}
