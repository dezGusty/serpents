#ifndef GUSLIB_SYSTEM_APPRESTART_H
#define GUSLIB_SYSTEM_APPRESTART_H

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
//   Application restart utility.
//   Separate between Windows and other OS restart classes.
//
//   Last change:  $LastChangedDate: 2014-04-16 00:16:54 +0300 (Mi, 16 apr. 2014) $
//   Revision:    $Revision: 648 $


// Command line switch for restarted application

//
// Includes
//

//
// Platform specific definitions and overall build options for the library.
//
#include <guslib/guslibbuildopts.h>

//
// C system headers
//
#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
# include <windows.h>
#else
# pragma message("Warning: AppRestart not defined for non-windows environments");
#endif

//
// C++ system headers
//
#include <string>

//
// This libraries' headers
//
#include "guslib/common/singleton.hpp"
#include "guslib/system/commandline.h"
#include "guslib/system/systemmutex.h"

namespace guslib
{
  /// The base class for restarting the application.
  class AbstractRestartInterface
  {
  public:
    /// Destructor.
    virtual ~AbstractRestartInterface()
    {
    }

    /// Check process command line for restart switch
    /// This function is called to check that the current instance is a restarted one.
    virtual bool restartProcessStarted() = 0;

    /// Function that waits until the process of the previous instance finishes.
    virtual void waitForPreviousProcessToFinish() = 0;

    /// This function will restart the current application.
    /// Keep in mind that for this to work, you need to close the current instance
    /// of the application (which will become inactive).
    /// You must close the current instance as soon as possible.
    virtual bool restart(std::string additionalParams="") = 0;

    /// Clear the locking restart mutex.
    /// You must call this function before exiting the application in a non-restart manner.
    /// Otherwise, you may find out that only 1 restart will work.
    /// (The residue mutexes will point to something else).
    virtual void finishRestart() = 0;
  };


  /// Class to hold the restart related functions.
  /// It's used as a namespace to group the functions together.
  class AppRestartUtil
    : public AbstractRestartInterface
  {
  protected:
    guslib::mutex::MutexHandle restarterMutex_;  // Mutex
    bool bWasRestarted;      // Restarted Flag

    std::string restartParam_;
    std::string mutexName_;

  public:
    /// Constructor; used only for data initialization.
    AppRestartUtil()
      : restarterMutex_(0),  // Mutex
        bWasRestarted(false),
        restartParam_("--Restart"),
        mutexName_("guslib--rr")
    {
    }

  public:
    virtual bool processWasRestarted()
    {
      return bWasRestarted;
    }

    virtual void setRestartParameter(const std::string & param)
    {
      restartParam_ = param;
    }

    virtual void setMutexName(const std::string & param)
    {
      mutexName_ = param;
    }

    /// Check process command line for restart switch
    /// This function is called to check that the current instance is a restarted one.
    virtual bool restartProcessStarted()
    {
      std::string parameters = CommandLine::getProgramArguments();
      return (parameters.find(restartParam_) != std::string::npos);
    }

    /// Function that waits until the process of the previous instance finishes.
    virtual void waitForPreviousProcessToFinish()
    {
      // App restarting
      bool alreadyRunning;

      // Try to Create Mutex
      restarterMutex_ = guslib::mutex::CreateNamedMutex(mutexName_, alreadyRunning, false);

      if (alreadyRunning )
      {
        // Waiting for previous instance release mutex
        guslib::mutex::LockMutex(restarterMutex_);
        guslib::mutex::UnlockMutex(restarterMutex_);
        bWasRestarted = TRUE;
      }

      guslib::mutex::DestroyMutex(restarterMutex_);
      restarterMutex_ = 0;
    }

    /// This function will restart the current application.
    /// Keep in mind that for this to work, you need to close the current instance
    /// of the application (which will become inactive).
    /// You must close the current instance as soon as possible.
    virtual bool restart(std::string additionalParams="")
    {
      // Restart App
      bool alreadyRunning;
      restarterMutex_ = guslib::mutex::CreateNamedMutex(mutexName_, alreadyRunning, true);

      if (alreadyRunning)
      {
        guslib::mutex::LockMutex(restarterMutex_);
        guslib::mutex::UnlockMutex(restarterMutex_);
        guslib::mutex::DestroyMutex(restarterMutex_);
        return false;
      }

      return guslib::mutex::TryToCreateProcessForCurrentApp(restartParam_, additionalParams);
    }

    /// Clear the locking restart mutex.
    /// You must call this function before exiting the application in a non-restart manner.
    /// Otherwise, you may find out that only 1 restart will work.
    /// (The residue mutexes will point to something else).
    virtual void finishRestart()
    {
      // Releasing mutex signal that process finished
      bool timeout = false;
      guslib::mutex::TryLockMutex(restarterMutex_, timeout);
      if (timeout)
      {
        guslib::mutex::UnlockMutex(restarterMutex_);
      }
      guslib::mutex::DestroyMutex(restarterMutex_);
      restarterMutex_ = NULL;
    }
  };  // end of class AppRestartUtil

  typedef guslib::Singleton <guslib::AppRestartUtil> AppRestarter;

  // use selectany to have the instance appear only once.
  // __declspec(selectany) guslib::GAppRestart * guslib::GAppRestart::internalInstance = NULL;
}   // namespace guslib

#endif  // GUSLIB_SYSTEM_APPRESTART_H
