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
//   Last change:  $LastChangedDate$
//   Revision:    $Revision$

//
// Includes
//

// Own header
#include "guslib/system/processrunner.h"

//
// C system files
//

#include <stdlib.h>

//
// C++ system files
//

// Allow the use of std::string
#include <string>

//
// Other libraries' headers
//

// Support for launching of external applications.
#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_LINUX
#include <unistd.h>
#endif  // GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_LINUX

//
// This library's headers
//

// String utility functions.
#include <guslib/common/simpleexception.h>

// String utility functions.
#include <guslib/util/stringutil.h>


namespace guslib
{
  namespace procrun
  {
  /**
    Execute a process (external application). The output is not captured.
    @param process_to_execute The external application or command to run.
    @return The return code of the external application. (Forwarded).
  */
  int ExecuteProcessAndWait(const std::string& process_to_execute, int maximum_timeout_in_millis)
  {
    int call_result = 0;

    // Forward the call. Ignore the return value, as we don't care about the captured output.
    ExecuteProcessWithOutputCaptureAndWait(
        process_to_execute, false, false, maximum_timeout_in_millis, call_result);

    return call_result;
  }

  /**
    Execute a process (external application) and capture the outptu. The main goal is to launch
    external applications or tools that output data to the console. That output is captured and
    provided to the user.
    @param process_to_execute The external application or command to run.
    @param process_visible Specifies whether the launched process is shown to the user or not (if it has a GUI/console)
    @param retrieve_output Specifies whether the console output of the launched process needs to be captured.
    @param maximum_timeout_in_millis The maximum amount of time that the process is allowed to run for.
           Using "-1" results in an infinite value being used.
    @param[out] output_code The return code of the external application. (Forwarded).
    @return The captured output (if captured). An empty string is returned if no capturing is taking place.
  */
  std::string ExecuteProcessWithOutputCaptureAndWait(
    const std::string& process_to_execute,
    bool process_visible,
    bool retrieve_output,
    int maximum_timeout_in_millis,
    int& output_code)
  {
#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
    return ExecuteProcessWithOutputCaptureAndWaitWindows(
        process_to_execute, process_visible, retrieve_output, maximum_timeout_in_millis, output_code);
#else
    return ExecuteProcessWithOutputCaptureAndWaitLinux(
        process_to_execute, retrieve_output, maximum_timeout_in_millis, output_code);
#endif
  }

  /**
    Get the host name of the local computer.
  */
  std::string GetLocalHostName()
  {
#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
    return GetLocalHostNameWindows();
#else
    return GetLocalHostNameLinux();
#endif
  }

  //
  // ------------------------------- Windows specific implementation ------------------------------------
  //

#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS

  /**
    Execute a process (external application) and capture the outptu. The main goal is to launch
    external applications or tools that output data to the console. That output is captured and
    provided to the user.
    @param process_to_execute The external application or command to run.
    @param process_visible Specifies whether the launched process is shown to the user or not (if it has a GUI/console)
    @param retrieve_output Specifies whether the console output of the launched process needs to be captured.
    @param maximum_timeout_in_millis The maximum amount of time that the process is allowed to run for.
           Using "-1" results in an infinite value being used.
    @param[out] output_code The return code of the external application. (Forwarded).
    @return The captured output (if captured). An empty string is returned if no capturing is taking place.
  */
  std::string ExecuteProcessWithOutputCaptureAndWaitWindows(
      const std::string& process_to_execute,
      bool process_visible,
      bool retrieve_output,
      int maximum_timeout_in_millis,
      int& output_code)
  {
    // Note: the functionality for capturing the console output of the executed process is based on the example
    // "Creating a Child Process with Redirected Input and Output"
    // http://msdn.microsoft.com/en-us/library/ms682499%28v=vs.85%29.aspx
    std::string result;

    PROCESS_INFORMATION process_information_structure;
    SECURITY_ATTRIBUTES sec_attribs;

    HANDLE hdl_child_std_out_read = NULL;
    HANDLE hdl_child_std_out_write = NULL;
    HANDLE hdl_child_std_in_read = NULL;
    HANDLE hdl_child_std_in_write = NULL;

    if (retrieve_output)
    {
      sec_attribs.nLength = sizeof(SECURITY_ATTRIBUTES);
      sec_attribs.bInheritHandle = TRUE;
      sec_attribs.lpSecurityDescriptor = NULL;

      // Create a pipe for the child process's STDOUT
      if (false == CreatePipe(&hdl_child_std_out_read, &hdl_child_std_out_write, &sec_attribs, 0))
      {
        return std::string("");
      }

      // Ensure that the read handle to the pipe for STDOUT is not inherited.
      if (false == SetHandleInformation(hdl_child_std_out_read, HANDLE_FLAG_INHERIT, 0))
      {
        return std::string("");
      }

      // Create a pipe for the child process's STDIN.
      if (false == CreatePipe(&hdl_child_std_in_read, &hdl_child_std_in_write, &sec_attribs, 0))
      {
        return std::string("");
      }

      // Ensure the write handle to the pipe for STDIN is not inherited.
      if (false == SetHandleInformation(hdl_child_std_in_read, HANDLE_FLAG_INHERIT, 0) )
      {
        return std::string("");
      }
    }

    std::string hidden_command(process_to_execute);

    // Create a process

    // First, create the structures for the process control
    STARTUPINFO si;

    // Initialize the structures.
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    if (retrieve_output)
    {
      si.hStdError = hdl_child_std_out_write;
      si.hStdOutput = hdl_child_std_out_write;
      si.hStdInput = hdl_child_std_in_read;
      si.dwFlags |= STARTF_USESTDHANDLES;
    }

    si.wShowWindow = SW_HIDE;

    ZeroMemory(&process_information_structure, sizeof(process_information_structure));

    // Set the creation flag in accordance with the call mode.
    DWORD dwCreationFlags = 0;
    dwCreationFlags = 0;
    if (false == process_visible)
    {
      dwCreationFlags = CREATE_NO_WINDOW;
    }

    BOOL inheritance_handle = FALSE;

    if (retrieve_output)
    {
      inheritance_handle = TRUE;
    }

    // Start the child process.
    if (false == CreateProcess(
      NULL,   // No module name (use command line)
#ifdef UNICODE
      (LPWSTR)(guslib::stringutil::StringToWString(hidden_command).c_str()),        // Command line
#else
      (LPSTR)(hidden_command.c_str()),        // Command line
#endif  // UNICODE
      NULL,                 // Security attributesProcess handle not inheritable
      NULL,                 // Thread handle not inheritable
      inheritance_handle,   // Set handle inheritance
      dwCreationFlags,      // Creation flags; use 0 for no special flags;
      //  CREATE_NEW_CONSOLE allows the current console to be free of
      //  parenting a new process.
      NULL,                 // Use parent's environment block
      NULL,                 // Use parent's starting directory
      &si,                  // Pointer to STARTUPINFO structure
      &process_information_structure)   // Pointer to PROCESS_INFORMATION structure
      )
    {
      DWORD encountered_error = GetLastError();
      return guslib::stringutil::LongLongToString(encountered_error);
    }

    // Wait for the process to finish.
    WaitForProcessTerminationByHandle(process_information_structure.hProcess, maximum_timeout_in_millis);

    // Get the exit code from the process.
    output_code = GetProcessReturnCode(process_information_structure.hProcess);

    // Read from pipe
    if (retrieve_output)
    {
      // Parent process needs to close handles that it passes to child process after the CreateProcess() call.
      // Not doing so will cause the parent to wait forever on ReadFile().
      CloseHandle(hdl_child_std_out_write);

      // Note(Augustin Preda, 2014.04.15): capturing output and no longer pushing content to the standard output.
      // Any console output of the process is now blocked.
      result = ReadPipeContent(hdl_child_std_out_read, false);
    }

    // Close process and thread handles.
    CloseHandle(process_information_structure.hProcess);
    CloseHandle(process_information_structure.hThread);

    // Close the pipe handles.
    if (retrieve_output)
    {
      CloseHandle(hdl_child_std_out_read);
      CloseHandle(hdl_child_std_in_read);
      CloseHandle(hdl_child_std_in_write);
    }

    return result;
  }

  /**
    Wait for a process to be terminated.
    @param process_handle The HANDLE object to wait for.
    @param timeout_in_millis The maximum allowed timeout duration. If set to -1, an infinite timeout is considered.
  */
  void WaitForProcessTerminationByHandle(HANDLE process_handle, int timeout_in_millis)
  {
    // Wait until child process exits.
    DWORD millis_amount = timeout_in_millis;
    if (millis_amount == -1)
    {
      millis_amount = INFINITE;
    }

    DWORD result_of_waiting = WaitForSingleObject(process_handle, millis_amount);
    switch (result_of_waiting)
    {
    case WAIT_ABANDONED:
      // TODO: return a special code.
      break;
    case WAIT_OBJECT_0:
      // TODO: return a special code.
      break;
    case WAIT_TIMEOUT:
      // TODO: return a special code.
      break;
    case WAIT_FAILED:
      // TODO: return a special code.
      break;
    }
  }

  /**
    Retrieve the return code of the process.
    (everything must already be set-up properly by the time you call this function)
  */
  int GetProcessReturnCode(HANDLE process_handle)
  {
    // Get the exit code from the process. This is different from the console output of the process.
    DWORD exitCode;
    if (GetExitCodeProcess(process_handle, &exitCode))
    {
      if (STILL_ACTIVE == exitCode)
      {
        // TODO: return or log error of [GetExitCodeProcess] because it is still running.
      }
      else
      {
        // finished
        if (exitCode == -1073741502)  // TODO: place in a constant
        {
          // TODO: return or log error of [GetExitCodeProcess] via [GetLastError()];
        }

        // Place the numerical exit code into the output value.
        return static_cast<int>(exitCode);
      }
    }
    else
    {
      // TODO: return or log error of [GetExitCodeProcess] via [GetLastError()];
    }

    return -1;
  }

  /**
    Retrieve the content of the pipe, reading from the given handle.
    (everything must already be set-up properly by the time you call this function)
    @param process_handle The Handle to the process to read from.
    @param keep_standard_output If set to TRUE, the pipe content will also be pushed to
     the standard output (console). If set to FALSE, the pipe content will be completely
     captured.
  */
  std::string ReadPipeContent(HANDLE process_handle, bool keep_standard_output)
  {
    std::string result("");

    const int kBufferSize = 1024;
    DWORD dwRead, dwWritten;
    CHAR chBuf[kBufferSize + 1];
    BOOL bSuccess = FALSE;
    HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    for (;;)
    {
      memset(chBuf, 0, kBufferSize + 1);
      bSuccess = ReadFile(process_handle, chBuf, kBufferSize, &dwRead, NULL);
      if (false == bSuccess || dwRead == 0)
      {
        break;
      }

      result.append(chBuf);

      if (keep_standard_output)
      {
        bSuccess = WriteFile(hParentStdOut, chBuf, dwRead, &dwWritten, NULL);
        if (false == bSuccess )
        {
          break;
        }
      }
    }

    return result;
  }

  /**
    Get the machine name of the current host.
  */
  std::string GetLocalHostNameWindows()
  {
    std::string result("");

    wchar_t buf[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD dwCompNameLen = MAX_COMPUTERNAME_LENGTH;
    if (0 != GetComputerNameW(buf, &dwCompNameLen))
    {
      result = guslib::stringutil::WStringToString(buf);
    }

    return result;
  }

#elif GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_LINUX

  //
  // ------------------------------- Linux specific implementation --------------------------------------
  //

  /**
    Execute a process (external application).
    @param process_to_execute The external application or command to run.
    @return Error code.
  */
  int ExecuteProcessAndWaitLinux(const std::string& process_to_execute)
  {
    std::string hidden_command(process_to_execute);
    return system(hidden_command.c_str());
  }

  /**
    Execute a process (external application) and capture the output. The main goal is to launch
    external applications or tools that output data to the console. That output is captured and
    provided to the user.
    @param process_to_execute The external application or command to run.
    @param[out] output_code The return code of the external application. (Forwarded).
    @return The captured output.
  */
  std::string ExecuteProcessWithOutputCaptureAndWaitLinux(
      const std::string& process_to_execute,
      bool retrieve_output,
      int maximum_timeout_in_millis,
      int& output_code)
  {
    std::string outputData = "";
    output_code = 0;

    throw guslib::SimpleException("Function [ExecuteProcessWithOutputCaptureAndWaitLinux] is not yet implemented for this OS");

    return outputData;
  }

  /**
    Get the machine name of the current host.
  */
  std::string GetLocalHostNameLinux()
  {
    char name[255];
    memset(name, 0, 255);
    gethostname(name, 255);
    std::string result(name);
    return result;
  }

#endif  // GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_LINUX
  }  // namespace procrun
}  // namespace guslib
