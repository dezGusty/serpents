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
//   Last change:  $LastChangedDate: 2014-05-30 23:02:39 +0300 (V, 30 mai. 2014) $
//   Revision:    $Revision: 652 $

//
// Includes
//

//
// Own header
//
#include "guslib/util/consolemenu.h"

//
// C++ system headers
//
#include <string>

//
// This libraries' headers
//
#include "guslib/trace/trace.h"

// by default, the menus will implement a value and selection wrapping.
#define MENU_ITEMS_WRAP_DEFAULT true


namespace guslib
{
  GMenuItem::GMenuItem(const GMenu * gmMaster, const std::string & sName, const std::string & sCaption)
    : name(sName),
      caption(sCaption),
      childSelectionIndex(0)
  {
    ptrToMenu = const_cast<GMenu *>(gmMaster);
    ptrToParent = NULL;
    selected = false;
    itemDepth = 0;
    auxData = NULL;
    childWrap = MENU_ITEMS_WRAP_DEFAULT;
  }


  GMenuItem::~GMenuItem()
  {
    GTRACE(7, "Destruction of menu item : " << name);
  }

  void GMenuItem::addChild(GMenuItem *newItem)
  {
    children.push_back(newItem);
    // if this is the first added item, select it.
    if (children.size() <= 1)
    {
      this->setSelectedChild(0);
    }
    children[children.size()-1]->itemDepth = this->itemDepth + 1;
    children[children.size()-1]->ptrToParent = this;
  }


  GMenuItem * GMenuItem::getSelectedChild()
  {
    if (false == hasChildren())
    {
      return NULL;
    }

    if (childSelectionIndex < 0
      || childSelectionIndex >= static_cast<int>(children.size()))
    {
      childSelectionIndex = 0;
    }

    return children[ childSelectionIndex ];
  }


  int GMenuItem::getSelectedChildIndex()
  {
    return childSelectionIndex;
  }


  void GMenuItem::reactToCommand(GMenuCommand cmd)
  {
    switch (cmd)
    {
    case cmdActionIn:
      {
        // If the item is a menu, it may be possible for it to allow having no child elements.
        GMenuItem * tempPtr = getSelectedChild();
        if (tempPtr != NULL)
        {
          tempPtr->startAction();
        }
        break;
      }
    case cmdMenuDown:
      selectNextChild();
      break;
    case cmdMenuUp:
      selectPreviousChild();
      break;
    case cmdActionEsc:
      returnControlToParent();
      break;
    default:
      {
      }
    }
  }

  void GMenuItem::removeAllChildren()
  {
    children.clear();
  }

  void GMenuItem::removeChild(const GMenuItem * child)
  {
    if (child == NULL)
    {
      return;
    }

    for (GMenuItemList::iterator it = children.begin(); it!= children.end(); ++it)
    {
      if ((*it)->getName() == child->getName())
      {
        children.erase(it);
        return;
      }
    }
  }


  void GMenuItem::returnControlToParent(bool triggeredByEsc)
  {
    if (this->ptrToParent == NULL)
    {
      // If there is no parent, call the callback with a null parameter, to allow special handling in listener.
      if (ptrToMenu->getListener() != NULL)
      {
        ptrToMenu->getListener()->OnMenuDepthChange(NULL, false);
      }

      return;
    }

    ptrToMenu->setActiveItem(ptrToParent);
    ptrToMenu->setVisibleItem(ptrToParent);

    // Called from root item or parent menu item.
    if (ptrToMenu->getListener() != NULL)
    {
      ptrToMenu->getListener()->OnMenuDepthChange(ptrToMenu, false);
      ptrToMenu->getListener()->OnMenuShouldRedraw(ptrToMenu);
    }
  }

  bool GMenuItem::selectNextChildBase(bool bOverrideWrap)
  {
    if (false == hasChildren())
    {
      return false;
    }
    bool bChangedSelection = false;

    // See if it's the last item.
    if (childSelectionIndex >= static_cast<int>(children.size() - 1))
    {
      if (childWrap || bOverrideWrap)
      {
        // If wrap is enabled, move to first, otherwise do nothing.
        childSelectionIndex = 0;
        bChangedSelection = true;
      }
    }
    else
    {
      ++childSelectionIndex;
      bChangedSelection = true;
    }
    return bChangedSelection;
  }


  bool GMenuItem::selectNextChild(bool bOverrideWrap)
  {
    bool bChangedSelection = selectNextChildBase(bOverrideWrap);
    if (bChangedSelection)
    {
      setSelectedChild(childSelectionIndex);
      if (ptrToMenu->getListener() != NULL)
      {
        ptrToMenu->getListener()->OnMenuSelectionChange(this);
        ptrToMenu->getListener()->OnMenuShouldRedraw(ptrToMenu);
      }
    }
    return bChangedSelection;
  }


  bool GMenuItem::selectPreviousChildBase(bool bOverrideWrap)
  {
    if (false == hasChildren())
    {
      return false;
    }

    bool bChangedSelection = false;

    // See if it's the first item.
    if (childSelectionIndex <= 0)
    {
      if (childWrap || bOverrideWrap)
      {
        // If wrap is enabled, move to first, otherwise do nothing.
        childSelectionIndex = static_cast<int>(children.size() - 1);
        bChangedSelection = true;
      }
    }
    else
    {
      --childSelectionIndex;
      bChangedSelection = true;
    }
    return bChangedSelection;
  }


  bool GMenuItem::selectPreviousChild(bool bOverrideWrap)
  {
    bool bChangedSelection = selectPreviousChildBase(bOverrideWrap);
    if (bChangedSelection)
    {
      setSelectedChild(childSelectionIndex);
      if (ptrToMenu->getListener() != NULL)
      {
        ptrToMenu->getListener()->OnMenuSelectionChange(this);
        ptrToMenu->getListener()->OnMenuShouldRedraw(ptrToMenu);
      }
    }
    return bChangedSelection;
  }


  void GMenuItem::setParent(GMenuItem *newParent)
  {
    // remove the connection to the child's existing parent.
    if (ptrToParent != NULL)
    {
      ptrToParent->removeChild(this);
    }

    ptrToParent = newParent;
  }


  void GMenuItem::setSelectedChild(int nNewChildSelectionIndex)
  {
    if (nNewChildSelectionIndex <0 || nNewChildSelectionIndex >= static_cast<int>(children.size()))
    {
      return;
    }

    for (GMenuItemList::iterator it = children.begin(); it!= children.end(); ++it)
    {
      if ((*it)->isSelected())
      {
        (*it)->setSelected(false);
      }
    }

    children[nNewChildSelectionIndex]->setSelected(true);
    this->childSelectionIndex = nNewChildSelectionIndex;
  }

  void GMenuItem::setSelectedChild(GMenuItem *aChild)
  {
    int idx = 0;
    for (GMenuItemList::iterator it = children.begin(); it!= children.end(); ++it)
    {
      if ((*it) == aChild)
      {
        setSelectedChild(idx);
        return;
      }

      idx++;
    }
  }

  void GMenuItem::startAction()
  {
    // Do not allow going into an empty submenu
    ptrToMenu->setActiveItem(this);
    ptrToMenu->setVisibleItem(this);

    if (ptrToMenu->isResetPositionOnLevelChange())
    {
      setSelectedChild(0);
    }

    // Called from root item or parent menu item.
    if (ptrToMenu->getListener() != NULL)
    {
      ptrToMenu->getListener()->OnMenuDepthChange(ptrToMenu, true);
      ptrToMenu->getListener()->OnMenuShouldRedraw(ptrToMenu);
    }
  }

  // ----------------------- end of base menu item ----------------------------------------------------

  GMenuSubMenu::GMenuSubMenu(const GMenu * masterMenu, const std::string &name, const std::string &capt)
    : GMenuItem(masterMenu, name, capt)
  {
  }


  GMenuSubMenu::~GMenuSubMenu()
  {
  }

  // ----------

  GMenuAction::GMenuAction(const GMenu *masterMenu, const std::string &name, const std::string &capt)
    :GMenuItem(masterMenu, name, capt)
  {
  }

  GMenuAction::~GMenuAction()
  {
  }

  void GMenuAction::startAction()
  {
    returnControlToParent(true);
    if (ptrToMenu->getListener() != NULL)
    {
        ptrToMenu->getListener()->OnMenuAction(this);
    }
  }

  // ----------

  GMenuBack::GMenuBack(const GMenu *masterMenu, const std::string &name, const std::string &capt)
    :GMenuItem(masterMenu, name, capt)
  {
  }

  GMenuBack::~GMenuBack()
  {
  }

  void GMenuBack::startAction()
  {
    if (ptrToMenu->getListener() != NULL)
    {
      bool bResult = ptrToMenu->getListener()->OnMenuBack(this);
      if (ptrToParent && bResult)
      {
        ptrToParent->returnControlToParent(true);
      }
    }
  }

  // -------------------------- end of actions

  // ----------------------------- options --------------------

  GMenuOption::GMenuOption(const GMenu *masterMenu, const std::string &name, const std::string &capt)
    :GMenuItem(masterMenu, name, capt)
  {
    initialSelection = 0;
  }
  GMenuOption::~GMenuOption()
  {
  }

  std::string GMenuOption::getCaption()
  {
    std::string sOutputString;
    sOutputString = caption;
    if (this->getSelectedChild() != NULL)
    {
      sOutputString = sOutputString + std::string(" : ") + getSelectedChild()->getCaption();
    }

    return sOutputString;
  }

  void GMenuOption::reactToCommand(GMenuCommand cmd)
  {
    switch (cmd)
    {
    case cmdActionIn:
      returnControlToParent(false);
      break;
    case cmdActionRight:
      selectNextChild();
      break;
    case cmdActionLeft:
      selectPreviousChild();
      break;
    case cmdActionEsc:
      returnControlToParent();
      break;
    default:
      {
      }
    }
  }


  void GMenuOption::returnControlToParent(bool bTriggeredByEsc)
  {
    if (this->ptrToParent == NULL)
    {
      return;
    }

    ptrToMenu->setActiveItem(ptrToParent);
    if (ptrToMenu->getListener() != NULL)
    {
      ptrToMenu->getListener()->OnMenuOptionStopEdit(this, getSelectedChild(), bTriggeredByEsc);
      if (bTriggeredByEsc)
      {
        setSelectedChild(initialSelection);
      }

      ptrToMenu->getListener()->OnMenuShouldRedraw(ptrToMenu);
    }
  }


  bool GMenuOption::selectNextChild(bool bOverrideWrap)
  {
    bool bTemp = selectNextChildBase(bOverrideWrap);
    if (bTemp)
    {
      if (ptrToMenu->getListener() != NULL)
      {
        ptrToMenu->getListener()->OnMenuOptionValueChange(this, getSelectedChild());
        ptrToMenu->getListener()->OnMenuShouldRedraw(ptrToMenu);
      }
    }
    return bTemp;
  }

  bool GMenuOption::selectPreviousChild(bool bOverrideWrap)
  {
    bool bTemp = selectPreviousChildBase(bOverrideWrap);
    if (bTemp)
    {
      if (ptrToMenu->getListener() != NULL)
      {
        ptrToMenu->getListener()->OnMenuOptionValueChange(this, getSelectedChild());
        ptrToMenu->getListener()->OnMenuShouldRedraw(ptrToMenu);
      }
    }
    return bTemp;
  }

  void GMenuOption::setInitialSelection(int idx)
  {
    this->initialSelection = idx;
  }

  void GMenuOption::startAction()
  {
    ptrToMenu->setActiveItem(this);
    this->setInitialSelection(this->getSelectedChildIndex());

    if (ptrToMenu->getListener() != NULL)
    {
      ptrToMenu->getListener()->OnMenuShouldRedraw(ptrToMenu);
      ptrToMenu->getListener()->OnMenuOptionStartEdit(this, getSelectedChild());
    }
  }

  // ----------------------- end of options -------------------


  GMenu::GMenu()
    : resetPosOnLvlChange_(true),
      listener(NULL)
  {
    rootItem = new GMenuSubMenu(this, "root", "Main Menu");
    activeItem = rootItem;
    visibleItem = rootItem;

    keyMap[cmdActionIn]  =13;    // enter
    keyMap[cmdMenuDown] = 40;    // arrow-down
    keyMap[cmdMenuUp] = 38;    // arrow-up
    keyMap[cmdActionLeft] = 37;  // arrow-left
    keyMap[cmdActionRight] = 39;  // arrow-right
    keyMap[cmdActionEsc] = 27;  // escape
  }


  GMenu::~GMenu()
  {
    if (0 != rootItem)
    {
      delete rootItem;
    }
  }

  void GMenu::clearKeyMap()
  {
    keyMap.clear();
  }


  void GMenu::reactToCommand(GMenuCommand cmd)
  {
    if (this->activeItem != NULL)
    {
      activeItem->reactToCommand(cmd);
    }
  }

  void GMenu::reactToKeyPress(int keyCode)
  {
    for (GusKeyMapping::iterator it = keyMap.begin(); it != keyMap.end(); ++it)
    {
      if ((*it).second == keyCode)
      {
        reactToCommand((*it).first);
      }
    }
  }

  GMenuItem * GMenu::searchForItemByName(std::string itemName, GMenuItem * itemToSearchFrom)
  {
    if (itemToSearchFrom == NULL)
    {
      if (rootItem == NULL)
      {
        return NULL;
      }

      return searchForItemByName(itemName, rootItem);
    }
    else
    {
      // see if this is the item searched for.
      if (itemToSearchFrom->getName() == itemName)
      {
        return itemToSearchFrom;
      }

      // if it's not, maybe a sub-item is.
      GMenuItemList aList = itemToSearchFrom->getChildren();
      for (GMenuItemList::iterator it = aList.begin(); it!= aList.end(); ++it)
      {
        GMenuItem *temp = searchForItemByName(itemName, (*it));
        if (temp)
        {
          return temp;
        }
      }

      // not found.
      return NULL;
    }
  }

  void GMenu::setKeyMap(GMenuCommand cmd, int keyCode)
  {
    keyMap[cmd] = keyCode;
  }


  void GMenu::setListener(const GMenuListener *ptrListener)
  {
    listener = const_cast<GMenuListener*>(ptrListener);
  }
}
