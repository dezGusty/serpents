#ifndef GUS_LIB_CONSOLE_MENU_H
#define GUS_LIB_CONSOLE_MENU_H

//   This file is part of the guslib library, licensed under the terms of the MIT License.
//
//   The MIT License
//   Copyright (C) 2010-2014  Augustin Preda (thegusty999@gmail.com)
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
//   Menu background logic handling.
//
//   Last change:  $LastChangedDate: 2014-04-16 00:16:54 +0300 (Mi, 16 apr. 2014) $
//   Revision:    $Revision: 648 $

//
// Includes
//

//
// C++ system headers
//

// Using the std::map
#include <map>
// Using the std::vector
#include <vector>
// Using the std::string
#include <string>

namespace guslib
{
  // Forward decl. for the menu.
  class GMenu;
  class GMenuItem;
  class GMenuListener;

  typedef enum GMenuCommandEnum
  {
    cmdMenuUp,        // such as moving the menu selection top.
    cmdMenuDown,      // such as moving the menu selection down.
    cmdActionLeft,    // such as changing selection left
    cmdActionRight,   // such as changing selection right
    cmdActionIn,      // such as pressing enter
    cmdActionEsc,     // such as pressing escape :-)
    numCommands
  }GMenuCommand;

  // the list of subitems or child elements.
  typedef std::vector<GMenuItem*> GMenuItemList;


  /**
    The menu classes provide access only to the background logic. But it would be nice to
    have the possibolity of storing some additional data (e.g. graphical related data) in the objects 
    themselves, so this is where the GMenuAuxData comes in. A GMenuAuxData can be added to the menu items.
    Extend this class to provide the required information.
  */
  class GMenuAuxData
  {
  public:
    GMenuAuxData()
    {
    }

    ~GMenuAuxData()
    {
    }
  };


  /**
    Base class for menu items. It is not abstract, so it can be instantiated.
    Each menu item will have a vector of child menu items.
    @note The name provided to the menu item should be unique, but uniqueness will not be enforced.
    Some functions will search for menu items according to their names, and in case they find 2 with
    the same name, they will return the first found.
    Example of usage:
    @code
    GMenu myMenu;
    GMenuItem mi1 (&myMenu, "item1", "Hello");
    myMenu.getRoot ().addChild (&mi1);
    @endcode
  */
  class GMenuItem
  {
  public:
    GMenuItem(const GMenu * gmMaster, const std::string &sName, const std::string &sCaption);
    virtual ~GMenuItem();

    virtual void addChild(GMenuItem * child);
    virtual GMenuAuxData * getAuxData()
    {
      return auxData;
    }
    virtual std::string getCaption()
    {
      return caption;
    }
    const GMenuItemList getChildren() const
    {
      return children;
    }
    virtual int getItemDepth()
    {
      return itemDepth;
    }

    const std::string & getName() const
    {
      return name;
    }
    virtual GMenuItem * getSelectedChild();
    virtual int getSelectedChildIndex();
    virtual const std::string &getType()
    {
      static std::string s("menu");
      return s;
    }
    virtual bool hasChildren()
    {
      return children.size() > 0;
    }
    virtual bool isSelected()
    {
      return selected;
    }

    virtual void reactToCommand(GMenuCommand cmd);
    virtual void removeAllChildren();
    virtual void removeChild(const GMenuItem * child);
    virtual void returnControlToParent(bool bTriggeredByEsc = true);
    virtual bool selectNextChild(bool bOverrideWrap = false);
    virtual bool selectPreviousChild(bool bOverrideWrap = false);

    virtual bool selectNextChildBase(bool bOverrideWrap = false);
    virtual bool selectPreviousChildBase(bool bOverrideWrap = false);

    virtual void setAuxData(GMenuAuxData * aux)
    {
      auxData = aux;
    }
    virtual void setCaption(const std::string &aCaption)
    {
      caption = aCaption;
    }
    virtual void setName(const std::string &sName)
    {
      name = sName;
    }
    virtual void setParent(GMenuItem *newParent);
    virtual void setSelected(bool bInput)
    {
      selected = bInput;
    }
    virtual void setSelectedChild(int nNewChildSelectionIndex);
    virtual void setSelectedChild(GMenuItem *aChild);
    virtual void setSelectionWrapping(bool bInput)
    {
      childWrap = bInput;
    }

    // handle the action when an "enter" is pressed, or smtn similar. override in subclasses.
    virtual void startAction();
  protected:
    GMenu * ptrToMenu;
    std::string name;
    std::string caption;

    GMenuAuxData * auxData;
    bool childWrap;
    bool selected;
    GMenuItemList children;
    GMenuItem *ptrToParent;
    int childSelectionIndex;  // Which child is selected (index in vector).
    int itemDepth;  // The depth in the menu where the item is found.
  };


  /**
  The submenu implements no additional functionality. Basically any submenu should be completely
  replaceable by the root menu item class, so the only difference is the standard type name
  which is different for this one.
  @code
  GMenu * myMenu = new GMenu();
  GMenuSubMenu * msm1 = new GMenuSubMenu(myMenu, "menuitem1", "Hello...");
  GMenuItem *i1 = new GMenuItem(myMenu, "i1", "Item 1");
  GMenuItem *i2 = new GMenuItem(myMenu, "i2", "Item 2");
  msm1->addChild(i1);
  msm1->addChild(i2);
  myMenu->getRoot()->addChild(mi1);
  @endcode
  */
  class GMenuSubMenu : public GMenuItem
  {
  public:
    GMenuSubMenu(const GMenu * masterMenu, const std::string &name, const std::string &capt="");
    virtual ~GMenuSubMenu();

    virtual const std::string & getType()
    {
      static std::string s("menu");
      return s;
    }
  };


  /**
  A menu item that has an action attached. No children should be attached to this item, since
  they will not be used. Use the listener interface to find out when the action related to this
  item should be triggered.
  <b>Example of usage:</b>
  @code
  GMenu * myMenu = new GMenu();
  GMenuAction * ma1 = new GMenuAction(myMenu, "action1", "Click me!");
  myMenu->getRoot()->addChild(ma1);
  @endcode
  @see startAction.
  */
  class GMenuAction : public GMenuItem
  {
  public:
    GMenuAction(const GMenu * masterMenu, const std::string &name, const std::string &capt = "");
    ~GMenuAction();
    virtual const std::string & getType()
    {
      static std::string s("action");
      return s;
    }

    // Override the hdl.
    virtual void startAction();
  protected:
  };


  /**
  A "back" menu item. This could be simulated with a menu action, but is provided in this form for
  convenience.
  */
  class GMenuBack : public GMenuItem
  {
  public:
    GMenuBack(const GMenu * masterMenu, const std::string &name, const std::string &capt = "");
    ~GMenuBack();
    virtual const std::string & getType()
    {
      static std::string s="back";
      return s;
    }

    // Override the hdl.
    virtual void startAction();
  };

  /// A menu option item (a menu item with the child elements being the possible choices).
  class GMenuOption : public GMenuItem
  {
  public:
    GMenuOption(const GMenu * masterMenu, const std::string &name, const std::string &capt = "");
    virtual ~GMenuOption();
    virtual std::string getCaption();
    virtual const std::string & getType()
    {
      static std::string s("option");
      return s;
    }

    // Override the hdl.
    virtual void reactToCommand(GMenuCommand cmd);
    virtual void returnControlToParent(bool bTriggeredByEsc = true);
    virtual bool selectNextChild(bool bOverrideWrap = false);
    virtual bool selectPreviousChild(bool bOverrideWrap = false);
    virtual void setInitialSelection(int idx);
    virtual void startAction();
  protected:
    int initialSelection;
  };


  /// The menu class. This contains a single root menu item. All further menu items are connected to
  /// and contained by this root item.
  class GMenu
  {
  public:
    GMenu();
    virtual ~GMenu();

    virtual void clearKeyMap();

    virtual GMenuItem * getActive() const
    {
      return activeItem;
    }

    virtual GMenuListener * getListener() const
    {
      return listener;
    }

    virtual GMenuItem * getRoot() const
    {
      return rootItem;
    }

    virtual GMenuItem * getVisible() const
    {
      return visibleItem;
    }

    // Flag that says whether the position of the menu selection should be reset to the
    // 1st item when the menu changes, or not (leaving it as it is).
    virtual bool isResetPositionOnLevelChange()
    {
      return resetPosOnLvlChange_;
    }

    virtual void reactToCommand(GMenuCommand cmd);

    /// React to a key action. Note that the menu class does not implement a different concept
    /// for pressing/releasing a key. The user must provide a key, which will be used in the configured
    /// mapping to determine an appropriate action to take.
    /// @see reactToCommand.
    virtual void reactToKeyPress(int keyCode);

    /// Get a real modifiable pointer to an item, searched for by name. Returns NULL if item not found.
    /// This search will not be very fast, so try not to call this from a rendering loop if speed is required.
    /// The search can be restricted to a subtree. If itemToSearchFrom set to null, the entire tree is searched.
    /// Also note that this is a recursive function. It may crash for large menu tree configs.
    virtual GMenuItem * searchForItemByName(std::string itemName, GMenuItem * itemToSearchFrom = NULL);
    virtual void setKeyMap(GMenuCommand cmd, int keyCode);
    virtual void setListener(const GMenuListener *ptrListener);
    virtual void setRootItem(const GMenuItem* root)
    {
      rootItem = static_cast<GMenuSubMenu*>(const_cast<GMenuItem*>(root));
    }

    virtual void setActiveItem(GMenuItem * item)
    {
      activeItem = item;
    }

    virtual void setVisibleItem(GMenuItem * item)
    {
      visibleItem = item;
    }

    virtual void setResetPositionOnLevelChange(bool value)
    {
      resetPosOnLvlChange_ = value;
    }
  protected:
    bool resetPosOnLvlChange_;
    GMenuItem * rootItem;
    // Pointer to the active menu item.
    GMenuItem * activeItem;

    // It is possible to view an item, while editing another. Use with care.
    GMenuItem * visibleItem;

    GMenuListener *listener;

    typedef std::map<GMenuCommand, int> GusKeyMapping;
    GusKeyMapping keyMap;
  };


  /// The listener interface.
  /// This will receive the events of the user's navigation thru a created menu.
  class GMenuListener
  {
  public:
    GMenuListener()
    {
    }

    virtual ~GMenuListener()
    {
    }

    /// React to an action-in event on an menu action item.
    virtual void OnMenuAction(GMenuItem * item) = 0;

    /// React to a back button being used.
    /// Must return true to allow the return to the parent to occur, or false to block it.
    /// This could be used to allow some sort of confirmation for returning to previous levels.
    virtual bool OnMenuBack(GMenuItem * item) = 0;

    /// This is a notification on every change of the menu that might require redrawing.
    /// The other functions could be used individually, but this one is provided as a convenience
    /// to allow the drawing to be called from a single place.
    virtual void OnMenuShouldRedraw(GMenu * menu) = 0;

    /// Called when the selection in the active menu is changed.
    virtual void OnMenuSelectionChange(GMenuItem *item) = 0;

    /**
    Called when going into a sub-menu, or returning from one.
    @note: the menu can be NULL, but that should happen only when the user is at the root level
    (for example in the main menu) and uses a "back" action (such as pressing [ESC]).
    @param menu The menu for which the depth was changed.
    @param bFromTop True if the depth was changed from a higher level menu, false if the change
    came from the bottom.
    */
    virtual void OnMenuDepthChange(GMenu * menu, bool bFromTop) =0;

    /// Called when starting to edit an option.
    virtual void OnMenuOptionStartEdit(GMenuItem *optionGroup, GMenuItem *option) = 0;

    /// Called when the editing stops. The editing of an option can stop with acceptance of changes,
    /// or with cancelling the changes done, indicated by the bCanceled flag.
    virtual void OnMenuOptionStopEdit(GMenuItem *optionGroup, GMenuItem *option, bool bCanceled) = 0;

    /// Called when an option changes value.
    /// This is called from within options, so the provided menu item should always be an option
    /// (or a descendant of it). This is called in real time, so the value provided here is not final.
    /// (The user may still cancel the changes).
    virtual void OnMenuOptionValueChange(GMenuItem *optionGroup, GMenuItem *option) = 0;
  };
}

#endif  // GUS_LIB_CONSOLE_MENU_H
