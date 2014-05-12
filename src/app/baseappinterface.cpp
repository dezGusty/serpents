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
//    Application base class, supporting restart.
//
//    Last change:  $LastChangedDate: 2014-04-26 23:40:46 +0300 (S, 26 apr. 2014) $
//    Revision:    $Revision: 650 $

//
// Includes
//

//
// Own header
//
#include "app/baseappinterface.h"

//
// Other libraries' headers
//

// App. restart utility.
#include "guslib/system/apprestart.h"

//
// This project's headers
//

// Macro defines for the parameters.
#include "common/RestartOpts.h"

namespace app
{
  /**
    The constructor will also take care of the finishing steps for any application restarts.
    Please ensure that this is called from the derived class.
  */
  BaseAppInterface::BaseAppInterface()
  {
    // Initialize restart code and parameters.
    guslib::AppRestarter::getPtr()->setRestartParameter(APP_RESTART_PARAMETER_NAME);
    guslib::AppRestarter::getPtr()->setMutexName(APP_MUTEX_PARAMETER_NAME);

    // Check if this instance is restarted and
    // wait while the previos instance finishes
    if (guslib::AppRestarter::getPtr()->restartProcessStarted())
    {
      // This is a restarted version. Wait for the previous one to close before proceeding.
      guslib::AppRestarter::getPtr()->waitForPreviousProcessToFinish();
    }
  }


  /**
    Destructor. Also takes care of necessary steps for the restart of the application.
  */
  BaseAppInterface::~BaseAppInterface()
  {
    // Finish restarting process if needed
    guslib::AppRestarter::getPtr()->finishRestart();
  }
}
