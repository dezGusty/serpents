#ifndef GUSLIB_SYSTEM_SYSTEMMUTEX_H
#define GUSLIB_SYSTEM_SYSTEMMUTEX_H

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
//   System Mutex handling
//
//   Last change:  $LastChangedDate: 2014-04-16 00:16:54 +0300 (Mi, 16 apr. 2014) $
//   Revision:    $Revision: 648 $

#include <guslib/guslibbuildopts.h>

#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
#include <windows.h>
#endif  // GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS

#include <string>

namespace guslib
{
  namespace mutex
  {
#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
  typedef HANDLE MutexHandle;
#else
  // LINUX
  typedef int MutexHandle;
#endif  // _WINDOWS

    GUSLIB_EXPORT_SYMBOL MutexHandle CreateNamedMutex(const std::string &name, bool & alreadyExists, bool owner = TRUE);

    GUSLIB_EXPORT_SYMBOL void DestroyMutex(MutexHandle handle);

    GUSLIB_EXPORT_SYMBOL void LockMutex(MutexHandle handle);

    GUSLIB_EXPORT_SYMBOL void TryLockMutex(MutexHandle handle, bool & timeout, int timeoutDuration = 0);

    GUSLIB_EXPORT_SYMBOL void UnlockMutex(MutexHandle handle);

    GUSLIB_EXPORT_SYMBOL bool TryToCreateProcessForCurrentApp(
        const std::string & restartParam, const std::string & additionalParams);
  }
}

#endif  // GUSLIB_SYSTEM_SYSTEMMUTEX_H
