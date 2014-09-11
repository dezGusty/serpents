#pragma once

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
//   External process handling helper class.
//
//   Last change:  $LastChangedDate$
//   Revision:    $Revision$

//
// Includes
//

//
// Platform specific definitions and overall build options for the library.
//
#include <guslib/guslibbuildopts.h>

//
// C system files
//

#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
// Add support for reading registry keys
#include <guslib/system/systemwindowsfwd.h>
#endif  // GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS

//
// C++ system files
//

// Allow use of std::string
#include <string>

namespace guslib
{
  ///
  ///  Helper class to store the names used by WinCC OA
  ///
  namespace procrun
  {
    /**
      Execute a process (external application). The output is not captured.
      @param process_to_execute The external application or command to run.
      @param maximum_timeout_in_millis The maximum amount of time that the process is allowed to run for.
             Using "-1" results in an infinite value being used.
      @return The return code of the external application. (Forwarded).
    */
    GUSLIB_EXPORT_SYMBOL int ExecuteProcessAndWait(const std::string& process_to_execute, int maximum_timeout_in_millis);

    /**
      Execute a process (external application) and capture the outptu. The main goal is to launch
      external applications or tools that output data to the console. That output is captured and
      provided to the user.
      @param process_to_execute The external application or command to run.
      @param process_visible Specifies whether the launched process is shown to the user or not
             (if it has a GUI/console)
      @param retrieve_output Specifies whether the console output of the launched process needs to be captured.
      @param maximum_timeout_in_millis The maximum amount of time that the process is allowed to run for.
             Using "-1" results in an infinite value being used.
      @param[out] output_code The return code of the external application. (Forwarded).
      @return The captured output (if captured). An empty string is returned if no capturing is taking place.
    */
    GUSLIB_EXPORT_SYMBOL std::string ExecuteProcessWithOutputCaptureAndWait(
        const std::string& process_to_execute,
        bool process_visible,
        bool retrieve_output,
        int maximum_timeout_in_millis,
        int& output_code);

    /**
      Get the host name of the local computer.
    */
    GUSLIB_EXPORT_SYMBOL std::string GetLocalHostName();

#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
    /**
      Execute a process (external application) and capture the outptu. The main goal is to launch
      external applications or tools that output data to the console. That output is captured and
      provided to the user.
      @param process_to_execute The external application or command to run.
      @param process_visible Specifies whether the launched process is shown to the user or not
             (if it has a GUI/console)
      @param retrieve_output Specifies whether the console output of the launched process needs to be captured.
      @param maximum_timeout_in_millis The maximum amount of time that the process is allowed to run for.
             Using "-1" results in an infinite value being used.
      @param[out] output_code The return code of the external application. (Forwarded).
      @return The captured output (if captured). An empty string is returned if no capturing is taking place.
    */
    GUSLIB_EXPORT_SYMBOL std::string ExecuteProcessWithOutputCaptureAndWaitWindows(
        const std::string& process_to_execute,
        bool process_visible,
        bool retrieve_output,
        int maximum_timeout_in_millis,
        int& output_code);

    /**
      Wait for a process to be terminated.
      @param process_handle The HANDLE object to wait for.
      @param timeout_in_millis The maximum allowed timeout duration. If set to -1, an infinite timeout is considered.
    */
    GUSLIB_EXPORT_SYMBOL void WaitForProcessTerminationByHandle(HANDLE process_handle, int timeout_in_millis);

    /**
      Retrieve the return code of the process.
      (everything must already be set-up properly by the time you call this function)
    */
    GUSLIB_EXPORT_SYMBOL int GetProcessReturnCode(HANDLE process_handle);

    /**
      Retrieve the content of the pipe, reading from the given handle.
      (everything must already be set-up properly by the time you call this function)
      @param process_handle The Handle to the process to read from.
      @param keep_standard_output If set to TRUE, the pipe content will also be pushed to
      the standard output (console). If set to FALSE, the pipe content will be completely
      captured.
    */
    GUSLIB_EXPORT_SYMBOL std::string ReadPipeContent(HANDLE process_handle, bool keep_standard_output);

    /**
      Get the machine name of the current host.
    */
    GUSLIB_EXPORT_SYMBOL std::string GetLocalHostNameWindows();

#elif GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_LINUX
    GUSLIB_EXPORT_SYMBOL std::string ExecuteProcessWithOutputCaptureAndWaitLinux(
        const std::string& process_to_execute,
        bool retrieve_output,
        int maximum_timeout_in_millis,
        int& output_code);

    GUSLIB_EXPORT_SYMBOL int ExecuteProcessAndWaitLinux(const std::string& process_to_execute);

    /**
      Get the machine name of the current host.
    */
    GUSLIB_EXPORT_SYMBOL std::string GetLocalHostNameLinux();

#endif  // GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_LINUX
  };
}
