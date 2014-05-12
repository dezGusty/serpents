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

//
// Includes
//

//
// Own header
//
#include "guslib/system/systemmutex.h"

//
// C++ system headers.
//
#include <string>

//
// This libraries' headers
//
#include "guslib/util/stringutil.h"

namespace guslib
{
  namespace mutex
  {
    MutexHandle CreateNamedMutex(const std::string &name, bool & alreadyExists, bool owner)
    {
      MutexHandle returnValue;
#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
      std::wstring myWide = stringutil::StringToWString(name);
      returnValue = CreateMutex(NULL, owner, myWide.c_str());
      DWORD dwLastError = GetLastError();
      alreadyExists = (dwLastError == ERROR_ALREADY_EXISTS || dwLastError == ERROR_ACCESS_DENIED);
#else
      // TODO(Augustin Preda, 2014.04.13): implement:
      pthread_mutex_init(&mutex, NULL);
#endif  // _WINDOWS
      return returnValue;
    }


    void DestroyMutex(MutexHandle handle)
    {
#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
      CloseHandle(handle);
#else
      // TODO(Augustin Preda, 2014.04.13): implement
#endif
    }


    void UnlockMutex(MutexHandle handle)
    {
#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
      ReleaseMutex(handle);
#else
      // TODO(Augustin Preda, 2014.04.13): implement:
#endif
    }

    void LockMutex(MutexHandle handle)
    {
#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
      WaitForSingleObject(handle, INFINITE);
#else
      // TODO(Augustin Preda, 2014.04.13): implement:
#endif
    }

    void TryLockMutex(MutexHandle handle, bool & timeout, int timeoutDuration)
    {
#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
      DWORD dwWaitResult = WaitForSingleObject(handle, timeoutDuration);
      if (dwWaitResult == WAIT_TIMEOUT)
      {
        timeout = true;
      }
#else
      // TODO(Augustin Preda, 2014.04.13): implement:
#endif
    }


    bool TryToCreateProcessForCurrentApp(const std::string & restartParam, const std::string & additionalParams)
    {
      TCHAR szAppPath[MAX_PATH] = {0};
      GetModuleFileName(NULL, szAppPath, MAX_PATH);

      STARTUPINFO        si = {0};
      PROCESS_INFORMATION    pi = {0};
      si.cb = sizeof(STARTUPINFO);
      // Create New Instance command line
      // Copy the application path, including the application's name itself.
      // If the path includes white space, use of qutation marks is required.
      bool bSpace = false;
      int nLen = static_cast<int>(wcslen(szAppPath));
      for (int i = 0; i < nLen && !bSpace; ++i)
      {
        if (szAppPath[i] == L' ')
        {
          bSpace = true;
        }
      }

      if (bSpace)
      {
        for (int i = nLen + 1; i > 1 && i < MAX_PATH; --i)
        {
          szAppPath[i] = szAppPath[i-1];
        }
        szAppPath[nLen+1] = '\"';
        szAppPath[0]='\"';
      }

      // Now add the application's parameters, separated by space.

      wcscat_s(szAppPath, MAX_PATH, L" ");
      wcscat_s(szAppPath, MAX_PATH, stringutil::StringToWString(restartParam).c_str());

      wchar_t wszTempString[MAX_PATH];
      size_t numCharsWritten;
      mbstowcs_s(&numCharsWritten, wszTempString, MAX_PATH, additionalParams.c_str(), additionalParams.length());

      wcscat_s(szAppPath, MAX_PATH, wszTempString);

      // Create another copy of processS
      BOOL bResult = CreateProcess(NULL, szAppPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
      return (bResult == TRUE);
    }
  }
}
