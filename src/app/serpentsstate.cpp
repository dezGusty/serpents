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
#include "app/serpentsstate.h"


//
// C++ system files
//

#include <string>

//
// Other libraries' headers
//

// Add tracing
#include "guslib/trace/trace.h"

namespace app
{
  SerpStateParams::SerpStateParams()
    : cfg_(guslib::Configuration())
  {
  }

  SerpStateParams::SerpStateParams(const guslib::Configuration& rhs)
    : cfg_(rhs)
  {
  }

  SerpState::~SerpState()
  {
  }

  SerpStateManagerListener::~SerpStateManagerListener()
  {
  }

  SerpStateManagerUtil::SerpStateManagerUtil()
    : listenerPtr_(0),
      nextPopState_(0),
      nextPushState_("")
  {
    GTRACE(2, "Serpent State Manager(util) created");
  }


  SerpStateManagerUtil::~SerpStateManagerUtil()
  {
    GTRACE(3, "SerpStateManagerUtil destruction");

    // clear the states vector.
    states_.clear();

    while (false == stateStack_.empty())
    {
      stateStack_.pop();
    }
  }


  void SerpStateManagerUtil::setListener(const SerpStateManagerListener * aListener)
  {
    listenerPtr_ = const_cast<SerpStateManagerListener*>(aListener);
  }


  bool SerpStateManagerUtil::addState(const SerpState* statePtr)
  {
    GTRACE(4, "Adding a state.");

    if (NULL == statePtr)
    {
      return false;
    }

    SerpStateMap::iterator it = states_.find(statePtr->getName());
    if (it != states_.end())
    {
      GTRACE(1, "Tried to add a state named [" << statePtr->getName() << "] but that name is already in use!");
      return false;
    }

    states_[statePtr->getName()] = const_cast<SerpState*>(statePtr);
    return true;
  }


  void SerpStateManagerUtil::internalPopState()
  {
    GTRACE(4, "(internal) popping a state.");

    // first remove the current state
    if (false == stateStack_.empty())
    {
      states_[stateStack_.top()]->exit();
      stateStack_.pop();
    }

    // also resume the previous state if it was suspended.
    if (false == stateStack_.empty())
    {
      states_[stateStack_.top()]->resume();
    }
    else
    {
      if (listenerPtr_)
      {
        listenerPtr_->OnStateStackEmpty();
      }
    }
  }


  void SerpStateManagerUtil::internalSetState(const std::string& state, const SerpStateParams& params)
  {
    if (false == stateStack_.empty())
    {
      states_[stateStack_.top()]->exit();
      stateStack_.pop();
    }

    GTRACE(4, "SerpStateManagerUtil::setState popped one state; inserting another one");
    stateStack_.push(state);
    states_[stateStack_.top()]->enter(params);
  }


  void SerpStateManagerUtil::internalPushState(const std::string& state, const SerpStateParams& params)
  {
    // first pause the current state

    if (false == stateStack_.empty())
    {
      states_[stateStack_.top()]->pause();
    }

    // now push the new one onto the stack and activate it.
    stateStack_.push(state);
    states_[stateStack_.top()]->enter(params);
  }


  void SerpStateManagerUtil::popStateNextTick()
  {
    ++nextPopState_;
    GTRACE(3, "Number of states to pop at the next tick:" << nextPopState_);
  }

  void SerpStateManagerUtil::pushStateNextTick(const std::string& state, const SerpStateParams& params)
  {
    SerpStateMap::iterator it = states_.find(state);
    if (it == states_.end())
    {
      GTRACE(2, "Unable to push state [" << state << "]. State doe not exist!");
      return;
    }
    nextPushState_ = state;
    paramsForNextState_ = params;
  }


  void SerpStateManagerUtil::setStateNextTick(const std::string& state, const SerpStateParams& params)
  {
    SerpStateMap::iterator it = states_.find(state);
    if (it == states_.end())
    {
      GTRACE(2, "Unable to push state [" << state << "]. State doe not exist!");
      return;
    }

    nextSetState_ = state;
    paramsForNextState_ = params;
  }


  void SerpStateManagerUtil::reactToKeyPress(const OIS::KeyCode& keycode)
  {
    GTRACE(5, "SerpStateManagerUtil::reactToKeyPress");
    if (false == stateStack_.empty() )
    {
      states_[stateStack_.top()]->reactToKeyPress(keycode);
    }
  }


  void SerpStateManagerUtil::reactToKeyRelease(const OIS::KeyCode& keycode)
  {
    GTRACE(5, "SerpStateManagerUtil::reactToKeyRelease");
    if (false == stateStack_.empty())
    {
      states_[stateStack_.top()]->reactToKeyRelease(keycode);
    }
  }


  void SerpStateManagerUtil::reactToMouseMoved(int x, int y)
  {
    if (false == stateStack_.empty())
    {
      states_[stateStack_.top()]->reactToMouseMoved(x, y);
    }
  }


  void SerpStateManagerUtil::reactToMousePressed(const OIS::MouseButtonID& btn, int x, int y)
  {
    if (false == stateStack_.empty())
    {
      states_[stateStack_.top()]->reactToMousePressed(btn, x, y);
    }
  }


  void SerpStateManagerUtil::reactToMouseReleased(const OIS::MouseButtonID& btn, int x, int y)
  {
    if (false == stateStack_.empty())
    {
      states_[stateStack_.top()]->reactToMouseReleased(btn, x, y);
    }
  }


  bool SerpStateManagerUtil::handleLogicThreadLoop()
  {
    GTRACE(6, "SerpStateManagerUtil: handling logic loop");

    if (nextPushState_ != "")
    {
      internalPushState(nextPushState_, paramsForNextState_);
      nextPushState_ = "";
    }
    else if (nextSetState_ != "")
    {
      internalSetState(nextSetState_, paramsForNextState_);
      nextSetState_ = "";
    }

    // Pop the states...
    while (nextPopState_ > 0)
    {
      GTRACE(3, "Will pop some state(s)");
      --nextPopState_;
      internalPopState();
    }

    if (false == stateStack_.empty())
    {
      // Take a pointer to the current state and release the mutex, allowing the state to be changed.
      // Regardless of whether the state was changed or not, call the logic handling function on the
      // stored state. This is necessary especially in order to prevent deadlocks due to the fact
      // that the logic thread may block other threads.
      SerpState * state = states_[stateStack_.top()];
      if (state)
      {
        bool retVal = state->handleLogicThreadLoop();
        return retVal;
      }
      else
      {
        return false;
      }
    }

    return false;
  }


  bool SerpStateManagerUtil::renderScene()
  {
    GTRACE(5, "SerpStateManagerUtil::renderScene()");
    if (false == stateStack_.empty())
    {
      // The rendering mutex can be released since the render operation will be called;
      // (and that operation will also create a lock which will block state changes until the frame
      // rendering will be finished).

      bool retVal = states_[stateStack_.top()]->renderScene();

      return retVal;
    }

    GTRACE(6, "SerpStateManagerUtil: renderScene has no states!.");

    return false;
  }


  // similar to renderScene; same comments should apply.
  bool SerpStateManagerUtil::renderGUI()
  {
    GTRACE(5, "SerpStateManagerUtil::renderGUI()");
    if (false == stateStack_.empty())
    {
      return states_[stateStack_.top()]->renderGUI();
    }

    return false;
  }


  void SerpStateManagerUtil::setShutdown()
  {
    // set the listener ptr to null.
    listenerPtr_ = 0;
  }

}  // namespace app
