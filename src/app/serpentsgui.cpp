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
# include "app/serpentsgui.h"

//
// C++ system files
//
#include <string>
#include <vector>

//
// Other libraries' headers
//

// Add tracing
#include "guslib/trace/trace.h"

//
// This project's headers
//

#include "utils/CEGUIUtils.h"

namespace app
{
  SerpGUI::SerpGUI(Ogre::RenderTarget* targetPtr, const std::string& logFileName, int logLevel)
  {
    GTRACE(4, "SerpGUI Ctor");

    resourceGroupName_ = std::string("General");  // TODO: remove hard coding.

    // Initialize the logging before anything else.
    new CEGUI::DefaultLogger();
    if (logLevel >= CEGUI::Errors && logLevel <= CEGUI::Insane)
    {
      CEGUI::LoggingLevel myLevel = static_cast<CEGUI::LoggingLevel>(logLevel);
      CEGUI::Logger::getSingleton().setLoggingLevel(myLevel);
    }

    if (logFileName.length() > 0)
    {
      CEGUI::DefaultLogger::getSingleton().setLogFilename(logFileName);
    }

    // Bootstrap CEGUI::System with an OgreRenderer object that uses the
    // default Ogre rendering window as the default output surface, an Ogre based
    // ResourceProvider, and an Ogre based ImageCodec.
    // A render target NEEDS to be passed, when Ogre is not handling the window creation itself.
    CEGUI::OgreRenderer::bootstrapSystem(*targetPtr);

    GTRACE(4, "SerpGUI Ctor done bootstrapping the system");
  }


  SerpGUI::~SerpGUI()
  {
    GTRACE(4, "SerpGUI Dtor");
  }


  void SerpGUI::setMouseCursor(const std::string& cursorName)
  {
    GTRACE(4, "SerpGUI setting mouse cursor to : " << cursorName);
    CEGUI::System* ceguiSysPtr = CEGUI::System::getSingletonPtr();

    ceguiSysPtr->getDefaultGUIContext().getMouseCursor().setDefaultImage(cursorName);
    ceguiSysPtr->getDefaultGUIContext().getMouseCursor().setImage(cursorName);
  }


  void SerpGUI::initialize()
  {
    GTRACE(3, "SerpGUI initializing");

    // Store the pointer.
    ceguiSysPtr_ = CEGUI::System::getSingletonPtr();

    GTRACE(3, "SerpGUI setting default resource group to: " << resourceGroupName_);
    ceguiSysPtr_->getResourceProvider()->setDefaultResourceGroup(resourceGroupName_);

    GTRACE(3, "SerpGUI done init");
  }


  void SerpGUI::loadSchemes(std::vector <std::string> schemeList)
  {
    GTRACE(3, "Loading scheme list: " << schemeList.size() << "items");
    for (std::vector <std::string>::iterator it = schemeList.begin(); it != schemeList.end(); ++it)
    {
      try
      {
        CEGUI::SchemeManager::getSingleton().createFromFile(
            (CEGUI::utf8*)(it->c_str()), (CEGUI::utf8*)resourceGroupName_.c_str());
      }
      catch (const std::exception & e)
      {
        GTRACE(2, "Unable to load scheme [" << it->c_str() << "]. Exception encountered: " << e.what());
      }
    }
  }


  void SerpGUI::renderGUI(float timeSinceLastRender)
  {
    GTRACE(5, "SerpGUI renderGUI");
    ceguiSysPtr_->renderAllGUIContexts();

    // You must inject the elapsed time into the cegui system, as it does not keep track of time itself.
    // failing to inject the time will result in time sensitive events(fades, tooltips, etc) not working as expected.
    utils::CEGUIHelperUtil::injectTimePulse(timeSinceLastRender);
  }


  void SerpGUI::addFont(const std::string& fontName)
  {
    GTRACE(5, "SerpGUI: Creating font [" << fontName << "]");
    try
    {
      CEGUI::FontManager::getSingleton().createFromFile(
          (CEGUI::utf8*)fontName.c_str(), (CEGUI::utf8*)resourceGroupName_.c_str());
    }
    catch (const std::exception & e)
    {
      GTRACE(2, "Could not create font [" << fontName << "]. Encountered exception: " << e.what());
    }

    GTRACE(5, "SerpGUI: Created font " << fontName);
  }

  void SerpGUI::setDefaultFont(const std::string& fontName)
  {
    utils::CEGUIHelperUtil::setDefaultFont(fontName);
  }
}
