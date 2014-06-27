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
//    Last change:  $LastChangedDate$
//    Revision:    $Revision$

//
// Includes
//

//
// Own header.
//
#include "app/serpentsgamehelper.h"

// Build options
#include <guslib/guslib_platform.h>

//
// C system headers.
//
#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
# include <windows.h>
#elif GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_LINUX
# include <X11/Xlib.h>
# include <X11/extensions/Xrandr.h>
#elif GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_APPLE
# include <CoreFoundation/CFBundle.h>
#endif  // GUSLIB_PLATFORM_TYPE

//
// C++ system headers
//
#include <map>
#include <string>
#include <vector>

//
// Other libraries' headers
//
#include "guslib/trace/trace.h"

// string handling utility
#include "guslib/util/stringutil.h"

//
// This library's headers
//

#include "OgreRoot.h"
#include "OgreRenderWindow.h"
#include "OgreEntity.h"
#include "OgreSubEntity.h"

namespace app
{
  SerpentsGameHelperUtil::SerpentsGameHelperUtil()
  {
  }

  SerpentsGameHelperUtil::~SerpentsGameHelperUtil()
  {
  }


  /**
   * Initialize the helper; this will also reload the list of supported features.
   */
  void SerpentsGameHelperUtil::init()
  {
    supportedFeatures_.clear();

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    // Feature to allow a non-exclusive fullscreen mode. It's basically a borderless window covering the entire
    // desktop area; on top of other windows. (It's still not "always-on-top").
    supportedFeatures_.push_back("non-exclusive-fullscreen");   // aka windowed (fullscreen) mode.

#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    // TODO(Augustin Preda): add platform specific code.
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    // TODO(Augustin Preda): add platform specific code.
#endif

    // Prepare the texts.  Note: these are only the default texts so that something is displayed at all times.
    // You can override them (see getEditableDisplayModeDescriptions).
    displayModeDescriptions_[FULLSCREEN_EXCLUSIVE] = "Fullscreen (Exclusive Mode)";
    displayModeDescriptions_[WINDOWED_WITH_BORDER] = "Window (With Borders)";
    displayModeDescriptions_[WINDOWED_BORDERLESS_FULL_SIZE] = "Window (Borderless)";

    videoDriverPseudoNames_["Direct3D9 Rendering Subsystem"] = "Direct3D";
    videoDriverPseudoNames_["OpenGL Rendering Subsystem"] = "OpenGL";

    guiAspectRatios_[std::string("004_003")] = static_cast<double>(4)  / static_cast<double>(3);
    guiAspectRatios_[std::string("005_004")] = static_cast<double>(5)  / static_cast<double>(4);
    guiAspectRatios_[std::string("016_010")] = static_cast<double>(16) / static_cast<double>(10);
    guiAspectRatios_[std::string("016_009")] = static_cast<double>(16) / static_cast<double>(9);
  }


  /**
    Get the associated value for an special application environment variable.
    This could be an OS environment variable.
    @param varName The name of the variable.
    @return The value of the variable, given as a string. An empty string if nothing is found.
    @author Augustin Preda.
  */
  std::string SerpentsGameHelperUtil::getCustomVariableValue(const std::string& varName) const
  {
    std::string out("");

    std::map <std::string, std::string>::const_iterator it = applicationGlobalVars_.find(varName);
    if (it != applicationGlobalVars_.end())
    {
      return it->second;
    }
    return out;
  }


  /**
   * Search in the list of stored features for a specific one.
   */
  bool SerpentsGameHelperUtil::hasFeature(const std::string& featureName) const
  {
    for (std::vector <std::string>::const_iterator it = supportedFeatures_.begin();
      it != supportedFeatures_.end(); ++it)
    {
      if (*it == featureName)
      {
        return true;
      }
    }
    return false;
  }


  std::string SerpentsGameHelperUtil::getUpdatedResolutionString(
      const std::string& initialString,
      int newWidth,
      int newHeight,
      int depth) const
  {
    std::string returnValue;
    returnValue = initialString;

    std::string sRight;

    int nPos = returnValue.find(" ");
    if (nPos == std::string::npos)
    {
      // some error...
      return "";
    }

    sRight= returnValue.substr(nPos + 3);   // + 3 chars for " x "
    nPos = sRight.find(" ");

    if (nPos == std::string::npos)
    {
      // we don't have a colour depth.
      sRight = "";
    }
    else
    {
      if (depth != 0
        && nPos + 5 < static_cast<int>(sRight.length()))
      {
        std::stringstream ss2;
        ss2 << " @ " << depth << sRight.substr(nPos + 5);
        sRight = ss2.str();
      }
      else
      {
        sRight = sRight.substr(nPos);
      }
    }

    // Use a string stream to parse the needed string.
    std::stringstream ss;
    ss << newWidth << " x " << newHeight << sRight;
    returnValue = ss.str();

    return returnValue;
  }


  /**
    * Gets the desktop's width.
    * Purpose: Setting resolution on initial startup
    *
    * @return width, in pixels
    * @author Augustin Preda
    */
  int SerpentsGameHelperUtil::getDesktopResolutionWidth() const
  {
    int xRes = 800;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    xRes = GetSystemMetrics(SM_CXSCREEN);
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    // TODO(Augustin Preda): add platform specific code.
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    // TODO(Augustin Preda): add platform specific code.
#endif
    return xRes;
  }

  /**
    * Gets the desktop's height.
    * Purpose: Setting resolution on initial startup
    *
    * @return height, in pixels
    * @author Augustin Preda
    */
  int SerpentsGameHelperUtil::getDesktopResolutionHeight() const
  {
    int yRes = 600;
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    yRes = GetSystemMetrics(SM_CYSCREEN);
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    // TODO(Augustin Preda): add platform specific code.
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    // TODO(Augustin Preda): add platform specific code.
#endif
    return yRes;
  }


  /**
   * Gets the native resolution on the desktop.
   * Purpose: Setting resolution on initial startup
   *
   * @return Full native desktop resolution, in string form.
   * @author Augustin Preda
   */
  std::string SerpentsGameHelperUtil::getDesktopResolutionString() const
  {
    int xRes = getDesktopResolutionWidth();
    int yRes = getDesktopResolutionHeight();

    std::stringstream ss;
    ss << xRes << " x " << yRes;
    return ss.str();
  }


  /**
   * Get the nearest aspect ratio compatible witht the current resolution.
   * This is very useful for windowed mode.
   * @param width The width of the display window.
   * @param height The height of the display window.
   * @return A string containing the nearest folder name corresponding to the given window size.
   * @author Augustin Preda.
   */
  std::string SerpentsGameHelperUtil::getNearestAspectRatioStringForWindowSize(int width, int height)
  {
    double myAspectRatio = 0;
    double smallestDiff = 999.0;
    std::string returnValue("");

    if (height == 0)
    {
      // return the first entry in the map?
      if (false == guiAspectRatios_.empty())
      {
        GTRACE(2, "WARNING: zero height specied; returning any available ratio!");
        return guiAspectRatios_.begin()->first;
      }
      else
      {
        // this should not occur!
        GTRACE(2, "WARNING: getNearestAspectRatioStringForWindowSize has no access to guiAspectRatios_! (empty map)");
        return returnValue;
      }
    }

    myAspectRatio = static_cast<double>(width) / static_cast<double>(height);

    for (std::map <std::string, double>::iterator it = guiAspectRatios_.begin();
      it != guiAspectRatios_.end(); ++it)
    {
      double currentDiff = myAspectRatio - it->second;
      if (currentDiff < 0)
      {
        currentDiff = - currentDiff;
      }

      if (smallestDiff > currentDiff)
      {
        smallestDiff = currentDiff;
        returnValue = it->first;
      }
    }

    return returnValue;
  }


  /**
    Dump the contents of the given scene node and any child nodes that may be attached to it to
    the (default) logger.
    @param rootNode The node to start the print from.
    @author Augustin Preda.
  */
  void SerpentsGameHelperUtil::printAllKidsToLogger(Ogre::SceneNode * rootNode, int level)
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

      printAllKidsToLogger(node, level + 1);
    }
  }

  /**
   * Retrieves (via output parameters) the settings requested by the user for the video mode size
   * @param rootPtr The Ogre Root pointer.
   * @param videoModeWidth The width specified in the video mode.
   * @param videoModeHeight The height specified in the video mode.
   * @return The full string containing the video mode specification, from the driver. It may include color depth.
   * @author Augustin Preda
   */
  std::string SerpentsGameHelperUtil::retrieveSampleRenderSystemWindowSize(
      Ogre::Root* rootPtr, int& videoModeWidth, int& videoModeHeight)
  {
    std::string returnValue("");

    // If the user selected a Windowed mode, but with the size of the entire screen, do additional preparations.
    Ogre::RenderSystem* myRenderSys = rootPtr->getRenderSystem();
    if (myRenderSys != 0)
    {
      Ogre::ConfigOptionMap myCfgMap = myRenderSys->getConfigOptions();
      for (Ogre::ConfigOptionMap::iterator it = myCfgMap.begin();
          it != myCfgMap.end();
          ++it)
      {
        GTRACE(4, "option name = [" << it->second.name << "], val = [" << it->second.currentValue << "]");
      }

      Ogre::ConfigOptionMap::iterator opt_it = myCfgMap.find("Video Mode");
      if (opt_it != myCfgMap.end())
      {
        GTRACE(4, "Currently selected video mode: " << opt_it->second.currentValue.c_str());
        returnValue = opt_it->second.currentValue;
        std::string sLeft, sRight;
        int nPos = returnValue.find(" ");
        if (nPos == std::string::npos)
        {
          // some error...
          GTRACE(2, "ERROR: Unable to find space in string: " << returnValue.c_str());
          return "";
        }

        sLeft = returnValue.substr(0, nPos);
        sRight= returnValue.substr(nPos + 3);   // + 3 for " x "
        nPos = sRight.find(" ");
        std::string sAux;

        if (nPos == std::string::npos)
        {
          // we don't have a colour depth.
          sAux = sRight;
        }
        else
        {
          sAux = sRight.substr(0, nPos);
        }

        videoModeWidth = guslib::stringutil::StringToInt(sLeft);
        videoModeHeight = guslib::stringutil::StringToInt(sAux);
      }
    }
    else
    {
      GTRACE(1, "Called retrieveSampleRenderSystemWindowSize, but no render system is available!");
    }

    GTRACE(4, "retrieveSampleRenderSystemWindowSize returning resolution: " << returnValue
      << " a.k.a. " << videoModeWidth << ", " << videoModeHeight);
    return returnValue;
  }


  /**
   * Parse a basic resolution string and get the width and height.
   * Purpose: manage differences between Direct3D and OpenGL resolution strings.
   *
   * @param initialString The source resolution string.
   * @param[out] newWidth The width to read.
   * @param[out] newHeight The height to read.
   * @return Nothing.
   * @author Augustin Preda
   */
  void SerpentsGameHelperUtil::getSizesFromResolutionString(
      const std::string& initialString,
      int& videoModeWidth,
      int& videoModeHeight) const
  {
    std::string tempString;
    tempString = initialString;

    std::string sLeft, sRight;
    int nPos = tempString.find(" ");
    if (nPos == std::string::npos)
    {
      // some error...
      return;
    }

    sLeft = tempString.substr(0, nPos);
    sRight= tempString.substr(nPos + 3);  // + 3 chars for " x "
    nPos = sRight.find(" ");
    std::string sAux;

    if (nPos == std::string::npos)
    {
      // we don't have a colour depth.
      sAux = sRight;
      sRight = "";
    }
    else
    {
      sAux = sRight.substr(0, nPos);
      sRight = sRight.substr(nPos);
    }

    videoModeWidth = guslib::stringutil::StringToInt(sLeft);
    videoModeHeight = guslib::stringutil::StringToInt(sAux);
  }


  /**
   * Parse a basic resolution string and get the width and height.
   * Purpose: manage differences between Direct3D and OpenGL resolution strings.
   * @return a partially filled ResolutionConfig object.
   * @author Augustin Preda.
   */
  ResolutionConfig SerpentsGameHelperUtil::getPartialConfigFromResolutionString(const std::string& initialString) const
  {
    ResolutionConfig out;

    std::string tempString(initialString);

    std::string sLeft, sRight;
    int nPos = tempString.find(" ");
    if (nPos == std::string::npos)
    {
      // some error...
      return out;
    }

    sLeft = tempString.substr(0, nPos);
    sRight= tempString.substr(nPos + 3);  // + 3 chars for " x "
    nPos = sRight.find(" ");
    std::string sAux;

    if (nPos == std::string::npos)
    {
      // we don't have a colour depth.
      sAux = sRight;
      sRight = "";
    }
    else
    {
      sAux = sRight.substr(0, nPos);
      sRight = sRight.substr(nPos);
    }

    out.width = guslib::stringutil::StringToInt(sLeft);
    out.height = guslib::stringutil::StringToInt(sAux);
    out.depth = 0;

    if (nPos + 5 < static_cast<int>(sRight.length()))
    {
      // the string is expected to be: " @ XY bit " - where XY is the colour depth.
      sAux = sRight.substr(3, 2);
      out.depth = guslib::stringutil::StringToInt(sAux);
    }

    return out;
  }

  /**
   * After changes in the graphics hardware/software, some of the previously stored settings may
   * no longer be available. This ensures that the application will try to use some valid settings.
   * This function is called once after startup to provide a mechanism ensuring that the used resolution
   * is possible, with a fallback case of 640x480. If this is not possible, it returns anything found.
   *
   * @param rootPtr The Ogre Root pointer.
   * @param input The proposed resolution config to validate.
   * @return The input resolution configuration, if it's valid. If it's not valid, a different configuration
   * is given back.
   * @author Augustin Preda
   */
  ResolutionConfig SerpentsGameHelperUtil::validateWindowSettings(
      Ogre::Root* rootPtr,
      const ResolutionConfig& input) const
  {
    ResolutionConfig out = input;
    std::string ignored;

    if (NULL == rootPtr)
    {
      // The resolution is not available, try to pick a new one.
      GTRACE(1, "Utils::validateWindowSettings called with NULL ogre root pointer.");
      return out;
    }

    Ogre::ConfigOptionMap options = rootPtr->getRenderSystem()->getConfigOptions();
    for (Ogre::ConfigOptionMap::iterator it = options.begin(); it != options.end(); ++it)
    {
      if (it->second.name == "Video Mode")
      {
        std::stringstream gssTemp;
        std::string sTemp;
        size_t nPos;
        Ogre::StringVector::iterator opt_it;
        std::vector <std::string> possibleResolutions;

        // Direct3D settings are displayed as "640 x 400 @ 16-bit" colour
        // OpenGL settings are displayed as "640 x 400"

        gssTemp << input.width << " x " << input.height;
        sTemp = gssTemp.str();

        // first, try to find the width x height resolution, opengl style

        GTRACE(5, "Looking for [" << gssTemp.str() << "]. Found allowed resolutions:");

        for (opt_it = it->second.possibleValues.begin();
              opt_it != it->second.possibleValues.end(); ++opt_it)
        {
          GTRACE(3, "* [" << *opt_it << "]");
          nPos = (*opt_it).find(sTemp);
          if (nPos != std::string::npos )
          {
            GTRACE(3, "* (found) [" << *opt_it << "]");
            possibleResolutions.push_back(*opt_it);
          }
        }

        // If the requested resolution was found in some instances, try to set the desired depth.

        if (possibleResolutions.size() > 0)
        {
          // clear the string stream, as we have data in it already.
          gssTemp.str("");
          gssTemp << input.width << " x " << input.height << " @ " << input.depth;
          sTemp = gssTemp.str();
          for (std::vector<std::string>::const_iterator myIt = possibleResolutions.begin();
            myIt != possibleResolutions.end(); ++myIt)
          {
            nPos = (*myIt).find(sTemp);
            if (nPos != std::string::npos)
            {
              // The resolution is found, use it as is.
              GTRACE(4, "Utils::validateWindowSettings usage of resolution [" << sTemp << "] approved");
              out.fullText = *myIt;
              return out;
            }
          }

          // clear the string and check it without the colour depth.
          gssTemp.str("");
          gssTemp << input.width << " x " << input.height;
          sTemp = gssTemp.str();
          for (std::vector<std::string>::const_iterator myIt = possibleResolutions.begin();
            myIt != possibleResolutions.end(); ++myIt)
          {
            nPos = (*myIt).find(sTemp);
            if (nPos != std::string::npos)
            {
              // The resolution is found, use it as is.
              GTRACE(4, "Utils::validateWindowSettings usage of resolution [" << sTemp << "] approved");
              out.fullText = *myIt;
              return out;
            }
          }

          // If this point is reached, the desired depth was not found. Therefore, choose any other
          // depth, but keep the same screen resolution.

          out.depth = 0;  // indicate that the requested colour depth wasn't set.
          // TODO(Augustin Preda): also store the string for the chosed display mode?
          out.fullText = "";
          return out;
        }

        // The resolution is not available, try to pick a new one.
        GTRACE(1, "Utils::validateWindowSettings usage of resolution " << input.width << " x " << input.height
          << " rejected. Not found in the " << possibleResolutions.size() << " possible items.");

        // Try a 640x480 resolution, any depth. If that fails, use whatever is found.

        sTemp = "640 x 480";
        for (opt_it = it->second.possibleValues.begin();
              opt_it != it->second.possibleValues.end(); opt_it++)
        {
          nPos = (*opt_it).find(sTemp);
          if (nPos != std::string::npos)
          {
            out.width = 640;
            out.height = 480;
            out.fullText = *opt_it;
            // The resolution is found, use it as is.
            GTRACE(4, "Utils::validateWindowSettings changed resolution to "
                << out.width << " x " << out.height << ".");
            return out;
          }
        }

        GTRACE(1, "Utils::validateWindowSettings failed to change to " << sTemp);

        // Failed default, try anything.

        for (opt_it = it->second.possibleValues.begin();
              opt_it != it->second.possibleValues.end(); ++opt_it)
        {
          std::string sFullEntry = *opt_it;
          std::string sLeft, sRight;
          nPos = sFullEntry.find(" ");
          if (nPos == std::string::npos)
          {
            // some error...
            GTRACE(3, "Unable to find space in [" << sFullEntry << "]");
            return out;
          }

          sLeft = sFullEntry.substr(0, nPos);
          sRight= sFullEntry.substr(nPos + 3);  // + 3 for " x "
          nPos = sRight.find(" ");
          std::string sAux;
          if (nPos == std::string::npos)
          {
            // we don't have a colour depth.
            sAux = sRight;
          }
          else
          {
            sAux = sRight.substr(0, nPos);
          }
          out.width = guslib::stringutil::StringToInt(sLeft);
          out.height = guslib::stringutil::StringToInt(sAux);
          out.fullText = *opt_it;
          GTRACE(1, "Utils::validateWindowSettings changed resolution to " << out.width << " x " << out.height << ".");
          return out;
        }

        GTRACE(1, "Utils::validateWindowSettings failed to find a display resolution it would agree to.");
      }
    }

    return out;
  }


  /**
   * Add a list of locations to a specified resource group.
   * @param locList The list (vector actually) of locations.
   * @param groupToAddTo The name of the resource group to add the locations to.
   * @author Augustin Preda
   */
  void SerpentsGameHelperUtil::addResourceLocationList(
      const std::vector<std::string> &locList,
      const std::string &groupToAddTo)
  {
    GTRACE(3, "..:: Adding resource locations (" << locList.size() << " items)...");
    for (std::vector<std::string>::const_iterator it = locList.begin(); it!= locList.end(); ++it )
    {
      this->addResourceLocation(*it, groupToAddTo);
    }
  }


  /**
    * Add a location to a specified resource group.
    * @param location The location.
    * @param groupToAddTo The name of the resource group to add the locations to.
    * @author Augustin Preda
    */
  void SerpentsGameHelperUtil::addResourceLocation(const std::string& loc, const std::string &groupToAddTo)
  {
    std::string location(loc);
    std::string secName(groupToAddTo);
    std::string typeName("FileSystem");
    bool bCanAddThis(true);

    size_t locationOfPercentageChar = location.find("%");
    if (locationOfPercentageChar != std::string::npos)
    {
      // found a percentage char.
      bCanAddThis = false;

      size_t locationOfSecondChar = location.find("%", locationOfPercentageChar + 1);
      if (locationOfSecondChar != std::string::npos)
      {
        std::string variableName = location.substr(locationOfPercentageChar + 1,
            locationOfSecondChar - locationOfPercentageChar - 1);

        std::string variableValue = getCustomVariableValue(variableName);
        if (variableValue.length() > 0)
        {
          bCanAddThis = true;
          location = location.substr(0, locationOfPercentageChar)
            + variableValue
            + location.substr(locationOfSecondChar + 1);
        }
        else
        {
          GTRACE(2, "There is a variable in use that is not YET defined: " << variableName);
        }
      }
    }

    if (location.length() > 4)
    {
      if (location.substr(location.length() - 4) == ".zip")
      {
        typeName = "Zip";
      }
    }

    if (bCanAddThis)
    {
      Ogre::ResourceGroupManager::getSingleton().addResourceLocation(location, typeName, secName);
      GTRACE(3, "Adding location to resource list:[" << location << "], group:[" << secName << "]");
      resourceGroupDirs_[secName] = location;
    }
    else
    {
      incompleteResourceGroupDirs_[secName] = location;
    }
  }


  /**
    Set the application icon to use at run-time.
    This icon will be seen when switching applications (E.g. during ALT+TABing on Windows). It will also
    be visible during the bordered window display mode, in the titlebar.

    @param renderWindow The Ogre render window. It will contain OS specific data that may be used.
    Note: not tested for Linux & Apple platforms, maybe in those cases the information is not available.
    Then again, maybe it's needed on those platforms.
    @param iconID A numerical identifier for the icon.
    Note: on Windows, this should be the ID of the icon in the resource file. As an example: add the
    constant IDI_MY_ICON in the header file
      [MyResource.h]
      #define IDI_MY_ICON  101
    And also add the reference to the used file in the rc file:
      [MyResource.rc]
      #include "resource.h"
      IDI_MY_ICON        ICON                    "res\\myIcon.ico"
    @return True if the application icon was successfully set, false otherwise.

    @author Augustin Preda.
  */
  bool SerpentsGameHelperUtil::setApplicationIcon(Ogre::RenderWindow* renderWindow, int iconID)
  {
    if (NULL == renderWindow)
    {
      return false;
    }

    //
    // Platform specific code - set the application icon.
    // On Windows, it's required to set the icon of the application.
    // This will be visible in the taskbar + alt-tab operations.
    // Also, if the application is started in windowed mode, the icon is set to the titlebar.
    //
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    {
      HWND hwnd;
      renderWindow->getCustomAttribute("WINDOW", &hwnd);

      if (NULL == hwnd)
      {
        return false;
      }

      HINSTANCE hinstance;
      hinstance = GetModuleHandle(0);

      HICON icon = LoadIcon(hinstance, MAKEINTRESOURCE(iconID));
      if (NULL == icon)
      {
        GTRACE(2, "Unable to load icon [" << iconID << "]");
        return false;
      }

      SendMessage(hwnd, WM_SETICON, ICON_BIG, LPARAM(icon));
      SendMessage(hwnd, WM_SETICON, ICON_SMALL, LPARAM(icon));
      DWORD errID = GetLastError();
      if (errID == 5)
      {
        // Access denied.
        GTRACE(2, "Access denied while trying to set window icon");
        return false;
      }
    }
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    // TODO(Augustin Preda): add platform specific code.
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    // TODO(Augustin Preda): add platform specific code.
#endif
    return true;
  }


  /**
    Set the application icon to use at run-time.
    This icon will be seen when switching applications (E.g. during ALT+TABing on Windows). It will also
    be visible during the bordered window display mode, in the titlebar.

    @param renderWindow The Ogre render window. It will contain OS specific data that may be used.
    Note: not tested for Linux & Apple platforms, maybe in those cases the information is not available.
    Then again, maybe it's needed on those platforms.
    @param width The width to set for the window.
    @param height The height to set for the window.
    @return True if the operation was successful, false otherwise.

    @author Augustin Preda.
  */
  bool SerpentsGameHelperUtil::setBorderlessWindowWithSize(Ogre::RenderWindow* renderWindow, int width, int height)
  {
    if (NULL == renderWindow)
    {
      return false;
    }

    //
    // Platform specific code - set the application icon.
    // On Windows, it's required to set the icon of the application.
    // This will be visible in the taskbar + alt-tab operations.
    // Also, if the application is started in windowed mode, the icon is set to the titlebar.
    //
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    {
      HWND hwnd;
      renderWindow->getCustomAttribute("WINDOW", &hwnd);

      if (NULL == hwnd)
      {
        return false;
      }

      SetWindowLong(hwnd, GWL_STYLE, WS_VISIBLE);
      SetWindowPos(hwnd, HWND_TOP, 0, 0, width, height, SWP_SHOWWINDOW);

      SendMessage(hwnd, WM_STYLECHANGING, 0, WS_OVERLAPPED);
      UpdateWindow(hwnd);
    }
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    // TODO(Augustin Preda): add platform specific code.
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    // TODO(Augustin Preda): add platform specific code.
#endif
    return true;
  }


  // The preferred renderer name.
  std::string SerpentsGameHelperUtil::getDriverNameForPseudoName(const std::string& pseudoName) const
  {
    std::string returnVal("");
    std::map <std::string, std::string>::const_iterator it;
    for (it = videoDriverPseudoNames_.begin(); it != videoDriverPseudoNames_.end(); ++it)
    {
      if (it->second == pseudoName)
      {
        returnVal = it->first;
      }
    }
    return returnVal;
  }


  // The preferred renderer name.
  std::string SerpentsGameHelperUtil::getPseudoNameForDriverName(const std::string& driverName) const
  {
    std::string returnVal("");
    std::map <std::string, std::string>::const_iterator it = videoDriverPseudoNames_.find(driverName);

    if (it != videoDriverPseudoNames_.end())
    {
      returnVal = it->second;
    }

    return returnVal;
  }


  /**
   * Retry to add the directories/resource locations for all groups that failed so far.
   * In order for this function to work, you must add resource locations using the functions defined in this class.
   *  - addResourceLocation
   *  - addResourceLocationList
   * If you only add resource locations using the standard OGRE functions, this function will have no effect.
   *
   * This will be mainly used to load resource groups after the aspect ratio is calculated.
   * The aspect ratio will be specified as a custom string in the resource paths.
   * That custom string (E.g. "$(ASPECT_RATIO)") will need to be replaced by the string
   * of the actual aspect ratio, corresponding to a folder name (E.g. "016_009");
   * @author Augustin Preda
   */
  void SerpentsGameHelperUtil::retryToAddIncompleteResourceDirs()
  {
    std::map <std::string, std::string> localCopy = incompleteResourceGroupDirs_;
    incompleteResourceGroupDirs_.clear();

    for (std::map <std::string, std::string>::iterator it = localCopy.begin();
      it != localCopy.end(); ++it)
    {
      addResourceLocation(it->second, it->first);
    }
  }
}
