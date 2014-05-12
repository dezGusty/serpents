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
//   Game Helper class, supporting miscellaneous functions, such as OS dependent
//   configurations, string parsing for OGRE engine resolutions (note: could be renamed
//   to OgreGameHelper).
//
//    Last change:  $LastChangedDate$
//    Revision:    $Revision$

//
// Includes
//

//
// C++ system headers.
//

// Using the std::string
#include <string>
// Using the std::vector
#include <vector>
// Using the std::map
#include <map>

//
// Other libraries' headers
//

// using the singleton
#include "guslib/common/singleton.hpp"

// Forward declaration to avoid inclusion of header.
namespace Ogre
{
  class Root;
  class RenderWindow;
}


namespace app
{
  /**
   * Enumeration of possible display modes. Please take note that not all may be supported on 
   * all combinations of hardware and software.
   */
  enum DisplayModes
  {
    // the old-school exclusive fullscreen mode. Not nice for multi-tasking on some
    // graphic cards. At least tested and proven slow when alt-tabbing for Radeon HD5000 cards.
    FULLSCREEN_EXCLUSIVE = 0,
    // the old-school bordered mode. Basically you can move the window around and move
    // it to a secondary monitor, and handle it just like a regular application.
    WINDOWED_WITH_BORDER = 1,
    // this is basically an emulated full screen. Underneath it's still a window, but it covers
    // the entire desktop area. This is good for multitasking, but it may be problematic at some
    // points, for the same reason: multitasking. Some applications may attempt to steal away the focus.
    WINDOWED_BORDERLESS_FULL_SIZE = 2
  };


  /**
   * A classic POD containing possible display resolution configurations.
   */
  struct ResolutionConfig
  {
  public:
    int width;
    int height;
    int depth;
    std::string fullText;
    double aspectRatio;        // aspect ratio value (floating point numerical value).
    std::string aspectRatioFolder;  // text to identify the aspect ratio folder (fraction text).
  };


  /**
   * Utility class to help with overall handling of Ogre tasks.
   */
  class SerpentsGameHelperUtil
  {
  protected:
    // A list of supported features.
    std::vector <std::string> supportedFeatures_;

    // The available video drivers.
    std::vector <std::string> availableVideoDrivers_;

    // The available driver pseudo-names.
    std::map <std::string, std::string> videoDriverPseudoNames_;

    // The available display resolutions.
    std::map <std::string, std::vector <std::string> > availableResolutions_;

    // The available descriptions for the display modes.
    std::map <DisplayModes, std::string> displayModeDescriptions_;

    // The available aspect ratio folders.
    std::map <std::string, double> guiAspectRatios_;

    // The available application internal configurable global vars.
    std::map <std::string, std::string> applicationGlobalVars_;

    // This is the set of resource group locations that was added.
    std::map <std::string, std::string> resourceGroupDirs_;

    // This is the set of resource group location that could not be loaded.
    std::map <std::string, std::string> incompleteResourceGroupDirs_;

    // The preferred renderer name.
    std::string preferredRendererName_;

    // The preferred display mode.
    DisplayModes preferredDisplayMode_;

    // The preferred resolution.
    std::string preferredResolutionName_;

    // The preferred resolution configuration.
    ResolutionConfig preferredResolutionConfig_;

    // CTor.
    SerpentsGameHelperUtil();

  public:
    // Give the singleton access to the constructor.
    friend class guslib::Singleton <SerpentsGameHelperUtil>;

    // Dtor, nothing expected to take place here.
    virtual ~SerpentsGameHelperUtil();

    /**
     * Initialize the helper; this will also reload the list of supported features.
     */
    void init();

    /**
     * Obtain access to a mapping of lists of global variables.
     */
    std::map <std::string, std::string>& getEditableApplicationVariablesMapping()
    {
      return applicationGlobalVars_;
    }

    /**
     * Obtain access to a mapping of lists of resolutions.
     */
    std::map <std::string, std::vector <std::string> >& getEditableResolutionsMapping()
    {
      return availableResolutions_;
    }

    /**
     * Get the list of video drivers that can be used. Note: you can edit the list(the ref is retrieved).
     */
    std::vector <std::string>& getEditableAvailableVideoDrivers()
    {
      return availableVideoDrivers_;
    }

    /**
     * Obtain access to a mapping of lists of resolutions.
     */
    std::map <DisplayModes, std::string>& getEditableDisplayModeDescriptions()
    {
      return displayModeDescriptions_;
    }


    /**
     * Obtain access to a mapping of lists of resource group locations.
     */
    std::map <std::string, std::string>& getEditableIncompleteResourceGroupDirs()
    {
      return incompleteResourceGroupDirs_;
    }

    /**
     * Retry to add the directories/resource locations for all groups that failed so far.
     * Maybe that the cause for their failure is no longer occuring.
     */
    void retryToAddIncompleteResourceDirs();


    /**
      Get the associated value for an special application environment variable.
      This could be an OS environment variable.
      @param varName The name of the variable.
      @return The value of the variable, given as a string.
      @author Augustin Preda.
    */
    std::string getCustomVariableValue(const std::string& varName) const;


    /**
     * Get the description for a display mode.
     * @param input The display mode to get the description for.
     * @return The description.
     */
    std::string getDescriptionForDisplayMode(DisplayModes input) const
    {
      std::string returnValue("");

      std::map <DisplayModes, std::string>::const_iterator it = displayModeDescriptions_.find(input);
      if (it != displayModeDescriptions_.end())
      {
        returnValue = it->second;
      }
      return returnValue;
    }


    /**
     * Search in the list of stored features for a specific one.
     * @param featureName The name of the feature to look for.
     */
    bool hasFeature(const std::string& featureName) const;


    /**
     * Gets the updated resolution string from a base resolution string.
     * Purpose: manage differences between Direct3D and OpenGL resolution strings.
     *
     * @param initialString The source resolution string.
     * @param newWidth The new width to use.
     * @param newHeight The new height to use.
     * @param depth The new depth to use(Optional; will choose anything that is available if not specified).
     * @return The full updated name of the entry for the resolution.
     * @author Augustin Preda
     */
    std::string getUpdatedResolutionString(
        const std::string& initialString,
        int newWidth,
        int newHeight,
        int depth = 0) const;

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
    void getSizesFromResolutionString(
        const std::string& initialString,
        int& videoModeWidth,
        int& videoModeHeight) const;


    /**
     * Parse a basic resolution string and get the width and height.
     * Purpose: manage differences between Direct3D and OpenGL resolution strings.
     * @return a partially filled ResolutionConfig object.
     * @author Augustin Preda.
     */
    ResolutionConfig getPartialConfigFromResolutionString(const std::string& initialString) const;

    /**
     * Gets the native resolution on the desktop.
     * Purpose: Setting resolution on initial startup
     *
     * @return Full native desktop resolution, in string form.
     * @author Augustin Preda
     */
    std::string getDesktopResolutionString() const;

    /**
     * Gets the desktop's width.
     * Purpose: Setting resolution on initial startup
     *
     * @return width, in pixels
     * @author Augustin Preda
     */
    int getDesktopResolutionWidth() const;

    /**
     * Gets the desktop's height.
     * Purpose: Setting resolution on initial startup
     *
     * @return height, in pixels
     * @author Augustin Preda
     */
    int getDesktopResolutionHeight() const;


    /**
     * Get the nearest aspect ratio compatible witht the current resolution.
     * This is very useful for windowed mode.
     * @param width The width of the display window.
     * @param height The height of the display window.
     * @return A string containing the nearest folder name corresponding to the given window size.
     * @author Augustin Preda.
     */
    std::string getNearestAspectRatioStringForWindowSize(int width, int height);


    /**
     * Retrieves (via output parameters) the settings requested by the user for the video mode size
     * @param rootPtr The Ogre Root pointer.
     * @param videoModeWidth The width specified in the video mode.
     * @param videoModeHeight The height specified in the video mode.
     * @return The full string containing the video mode specification, from the driver. It may include color depth.
     * @author Augustin Preda
     */
    std::string retrieveSampleRenderSystemWindowSize(Ogre::Root* rootPtr, int& videoModeWidth, int& videoModeHeight);


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
    ResolutionConfig validateWindowSettings(Ogre::Root* rootPtr, const ResolutionConfig& input) const;


    /**
     * Add a list of locations to a specified resource group.
     * @param locList The list(vector actually) of locations.
     * @param groupToAddTo The name of the resource group to add the locations to.
     * @author Augustin Preda
     */
    void addResourceLocationList(const std::vector<std::string> &locList, const std::string &groupToAddTo);

    /**
     * Add a location to a specified resource group.
     * @param location The location.
     * @param groupToAddTo The name of the resource group to add the locations to.
     * @author Augustin Preda
     */
    void addResourceLocation(const std::string& location, const std::string &groupToAddTo);

    //
    // Application icon
    //

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
    bool setApplicationIcon(Ogre::RenderWindow* renderWindow, int iconID);


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
    bool setBorderlessWindowWithSize(Ogre::RenderWindow* renderWindow, int width, int height);

    //
    // Generic getters and setters.
    //

    // The preferred renderer name.
    const std::string & getPreferredRendererName() const
    {
      return preferredRendererName_;
    }

    // The preferred resolution.
    const std::string & getPreferredResolutionName() const
    {
      return preferredResolutionName_;
    }

    // The preferred display mode.
    DisplayModes getPreferredDisplayMode() const
    {
      return preferredDisplayMode_;
    }

    // The preferred resolution configuration.
    ResolutionConfig getPreferredResolutionConfig() const
    {
      return preferredResolutionConfig_;
    }

    // The preferred renderer name.
    std::string getDriverNameForPseudoName(const std::string& pseudoName) const;

    // The preferred renderer name.
    std::string getPseudoNameForDriverName(const std::string& driverName) const;

    // TODO(Augustin Preda): integrate the reading and setting of preferred values with the preferred resolution config.
    // Set the preferred renderer
    void setPreferredRendererName(const std::string& newValue)
    {
      preferredRendererName_ = newValue;
    }

    // Set the preferred resolution
    void setPreferredResolutionName(const std::string& newValue)
    {
      preferredResolutionName_ = newValue;
    }

    // Set the preferred display mode
    void setPreferredDisplayMode(DisplayModes newMode)
    {
      preferredDisplayMode_ = newMode;
    }

    // Set the preferred resolution configuration.
    void setPreferredResolutionConfig(ResolutionConfig newConfig)
    {
      preferredResolutionConfig_ = newConfig;
    }
  };

  typedef guslib::Singleton <app::SerpentsGameHelperUtil> SerpentsGameHelper;
}
