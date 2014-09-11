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
//   Last change:  $LastChangedDate: 2014-09-11 20:14:06 +0200 (J, 11 sep. 2014) $
//   Revision:    $Revision: 672 $

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
  //
  // ----------------------------- Menu item internals ---------------------------------
  //

  class GMenuItem::Impl
  {
  public:
    std::string name_;
    std::string caption_;
    GMenuItemList children_;

    GMenuItem::Impl(const std::string& name, const std::string& caption)
      : name_(name),
        caption_(caption)
    {
    }
  };

  //
  // -------------------------------- Menu item class -----------------------------------
  //

  GMenuItem::GMenuItem(const GMenu * gmMaster, const std::string& name, const std::string& caption)
    : impl_(new GMenuItem::Impl(name, caption)),
      childSelectionIndex(0)
  {
    ptrToMenu = const_cast<GMenu *>(gmMaster);
    ptrToParent = nullptr;
    selected = false;
    itemDepth = 0;
    auxData = nullptr;
    childWrap = MENU_ITEMS_WRAP_DEFAULT;
  }

  GMenuItem::GMenuItem(const GMenuItem& rhs)
    : impl_(new GMenuItem::Impl(*rhs.impl_))
  {

  }

  GMenuItem::~GMenuItem()
  {
    GTRACE(7, "Destruction of menu item : " << impl_->name_);
    delete impl_;
  }

  const std::string& GMenuItem::getCaption()
  {
    return impl_->caption_;
  }

  const std::string& GMenuItem::getName() const
  {
    return impl_->name_;
  }

  void GMenuItem::setCaption(const std::string& caption)
  {
    impl_->caption_ = caption;
  }

  void GMenuItem::setName(const std::string& name)
  {
    impl_->name_ = name;
  }

  const GMenuItemList& GMenuItem::getChildren() const
  {
    return impl_->children_;
  }

  void GMenuItem::addChild(GMenuItem *newItem)
  {
    impl_->children_.push_back(newItem);
    // if this is the first added item, select it.
    if (impl_->children_.size() <= 1)
    {
      this->setSelectedChild(0);
    }

    impl_->children_[impl_->children_.size()-1]->itemDepth = this->itemDepth + 1;
    impl_->children_[impl_->children_.size()-1]->ptrToParent = this;
  }


  GMenuItem * GMenuItem::getSelectedChild()
  {
    if (false == hasChildren())
    {
      return nullptr;
    }

    if (childSelectionIndex < 0
      || childSelectionIndex >= static_cast<int>(impl_->children_.size()))
    {
      childSelectionIndex = 0;
    }

    return impl_->children_[childSelectionIndex];
  }


  int GMenuItem::getSelectedChildIndex()
  {
    return childSelectionIndex;
  }


  void GMenuItem::reactToCommand(GMenuCommand cmd)
  {
    switch (cmd)
    {
    case GMenuCommand::cmdActionIn:
      {
        // If the item is a menu, it may be possible for it to allow having no child elements.
        GMenuItem * tempPtr = getSelectedChild();
        if (tempPtr != nullptr)
        {
          tempPtr->startAction();
        }
        break;
      }
    case GMenuCommand::cmdMenuDown:
      selectNextChild();
      break;
    case GMenuCommand::cmdMenuUp:
      selectPreviousChild();
      break;
    case GMenuCommand::cmdActionEsc:
      returnControlToParent();
      break;
    default:
      {
      }
    }
  }

  void GMenuItem::removeAllChildren()
  {
    impl_->children_.clear();
  }

  void GMenuItem::removeChild(const GMenuItem * child)
  {
    if (child == nullptr)
    {
      return;
    }

    for (GMenuItemList::iterator it = impl_->children_.begin(); it!= impl_->children_.end(); ++it)
    {
      if ((*it)->getName() == child->getName())
      {
        impl_->children_.erase(it);
        return;
      }
    }
  }


  void GMenuItem::returnControlToParent(bool triggeredByEsc)
  {
    if (this->ptrToParent == nullptr)
    {
      // If there is no parent, call the callback with a null parameter, to allow special handling in listener.
      if (ptrToMenu->getListener() != nullptr)
      {
        ptrToMenu->getListener()->OnMenuDepthChange(nullptr, false);
      }

      return;
    }

    ptrToMenu->setActiveItem(ptrToParent);
    ptrToMenu->setVisibleItem(ptrToParent);

    // Called from root item or parent menu item.
    if (ptrToMenu->getListener() != nullptr)
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
    if (childSelectionIndex >= static_cast<int>(impl_->children_.size() - 1))
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
      if (ptrToMenu->getListener() != nullptr)
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
        childSelectionIndex = static_cast<int>(impl_->children_.size() - 1);
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
      if (ptrToMenu->getListener() != nullptr)
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
    if (ptrToParent != nullptr)
    {
      ptrToParent->removeChild(this);
    }

    ptrToParent = newParent;
  }


  void GMenuItem::setSelectedChild(int nNewChildSelectionIndex)
  {
    if (nNewChildSelectionIndex <0 || nNewChildSelectionIndex >= static_cast<int>(impl_->children_.size()))
    {
      return;
    }

    for (GMenuItemList::iterator it = impl_->children_.begin(); it!= impl_->children_.end(); ++it)
    {
      if ((*it)->isSelected())
      {
        (*it)->setSelected(false);
      }
    }

    impl_->children_[nNewChildSelectionIndex]->setSelected(true);
    this->childSelectionIndex = nNewChildSelectionIndex;
  }

  void GMenuItem::setSelectedChild(GMenuItem *aChild)
  {
    int idx = 0;
    for (GMenuItemList::iterator it = impl_->children_.begin(); it!= impl_->children_.end(); ++it)
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
    if (ptrToMenu->getListener() != nullptr)
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
    if (ptrToMenu->getListener() != nullptr)
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
    if (ptrToMenu->getListener() != nullptr)
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

  //
  // ----------------------------- Options menu item internals ---------------------------------
  //

  class GMenuOption::Impl
  {
  public:
    std::string type_;
    std::string combinedCaption_;

    GMenuOption::Impl(const std::string& type)
      : type_(type),
        combinedCaption_("")
    {
    }
  };

  GMenuOption::GMenuOption(const GMenu *masterMenu, const std::string &name, const std::string &capt)
    : GMenuItem(masterMenu, name, capt),
      impl_(new GMenuOption::Impl("option"))
  {
    initialSelection = 0;
  }
  GMenuOption::~GMenuOption()
  {
    delete impl_;
  }

  const std::string& GMenuOption::getType() const
  {
    return impl_->type_;
  }

  const std::string& GMenuOption::getCaption()
  {
    // Call the base implementation.
    impl_->combinedCaption_ = GMenuItem::getCaption();

    if (this->getSelectedChild() != nullptr)
    {
      impl_->combinedCaption_.append(" : ");
      impl_->combinedCaption_.append(getSelectedChild()->getCaption());
    }

    return impl_->combinedCaption_;
  }

  void GMenuOption::reactToCommand(GMenuCommand cmd)
  {
    switch (cmd)
    {
    case GMenuCommand::cmdActionIn:
      returnControlToParent(false);
      break;
    case GMenuCommand::cmdActionRight:
      selectNextChild();
      break;
    case GMenuCommand::cmdActionLeft:
      selectPreviousChild();
      break;
    case GMenuCommand::cmdActionEsc:
      returnControlToParent();
      break;
    default:
      {
      }
    }
  }


  void GMenuOption::returnControlToParent(bool bTriggeredByEsc)
  {
    if (this->ptrToParent == nullptr)
    {
      return;
    }

    ptrToMenu->setActiveItem(ptrToParent);
    if (ptrToMenu->getListener() != nullptr)
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
      if (ptrToMenu->getListener() != nullptr)
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
      if (ptrToMenu->getListener() != nullptr)
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

    if (ptrToMenu->getListener() != nullptr)
    {
      ptrToMenu->getListener()->OnMenuShouldRedraw(ptrToMenu);
      ptrToMenu->getListener()->OnMenuOptionStartEdit(this, getSelectedChild());
    }
  }

  // ----------------------- end of options -------------------


  class GMenu::Impl
  {
  public:
    std::map<GMenuCommand, int> keyMap;
  };

  GMenu::GMenu()
    : resetPosOnLvlChange_(true)
    , listener(nullptr)
    , rootItem(nullptr)
    , impl_(new GMenu::Impl())
  {
    rootItem = new GMenuSubMenu(this, "root", "Main Menu");
    activeItem = rootItem;
    visibleItem = rootItem;

    impl_->keyMap[GMenuCommand::cmdActionIn] = 13;    // enter
    impl_->keyMap[GMenuCommand::cmdMenuDown] = 40;    // arrow-down
    impl_->keyMap[GMenuCommand::cmdMenuUp] = 38;    // arrow-up
    impl_->keyMap[GMenuCommand::cmdActionLeft] = 37;  // arrow-left
    impl_->keyMap[GMenuCommand::cmdActionRight] = 39;  // arrow-right
    impl_->keyMap[GMenuCommand::cmdActionEsc] = 27;  // escape
  }

  GMenu::GMenu(const GMenu& rhs)
    : impl_(new GMenu::Impl(*rhs.impl_))
    , resetPosOnLvlChange_(rhs.resetPosOnLvlChange_)
    , rootItem(new GMenuItem(*rhs.rootItem))
    , activeItem(rhs.activeItem)
    , visibleItem(rhs.visibleItem)
    , listener(rhs.listener)
  {
  }

  GMenu::~GMenu()
  {
    if (nullptr != rootItem)
    {
      delete rootItem;
    }

    if (nullptr != impl_)
    {
      delete impl_;
    }
  }

  void GMenu::clearKeyMap()
  {
    impl_->keyMap.clear();
  }


  void GMenu::reactToCommand(GMenuCommand cmd)
  {
    if (this->activeItem != nullptr)
    {
      activeItem->reactToCommand(cmd);
    }
  }

  void GMenu::reactToKeyPress(int keyCode)
  {
    for (auto it : impl_->keyMap)
    {
      if (it.second == keyCode)
      {
        reactToCommand(it.first);
      }
    }
  }

  GMenuItem * GMenu::searchForItemByName(std::string itemName, GMenuItem * itemToSearchFrom)
  {
    if (itemToSearchFrom == nullptr)
    {
      if (rootItem == nullptr)
      {
        return nullptr;
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
      return nullptr;
    }
  }

  void GMenu::setKeyMap(GMenuCommand cmd, int keyCode)
  {
    impl_->keyMap[cmd] = keyCode;
  }


  void GMenu::setListener(const GMenuListener *ptrListener)
  {
    listener = const_cast<GMenuListener*>(ptrListener);
  }
}
