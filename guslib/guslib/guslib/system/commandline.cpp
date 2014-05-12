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
//   Command line handling utility
//
//   Last change:  $LastChangedDate: 2014-04-26 23:40:46 +0300 (S, 26 apr. 2014) $
//   Revision:    $Revision: 650 $

//
// Includes
//

//
// Own header
//
#include "guslib/system/commandline.h"

//
// C system headers.
//

// Include OS specific headers.
#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
#include <windows.h>
#endif  // GUSLIB_PLATFORM_TYPE

//
// C++ system includes
//

#include <string>
#include <vector>

//
// This library's headers
//

// Tracing
#include "guslib/trace/trace.h"

// String utility functions.
#include "guslib/util/stringutil.h"


namespace guslib
{
  const std::string CommandLine::getProgramArguments()
  {
    std::string returnVal("");
#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
    LPTSTR szCmdLine = GetCommandLine();

    std::wstring myWide = szCmdLine;
    returnVal = stringutil::WStringToString(myWide);
#else

    // Not implemented. TODO(Augustin Preda), 2014.04.15: implement!
#endif  // GUSLIB_PLATFORM_TYPE
    return returnVal;
  }


  const std::vector <std::string> CommandLine::getListOfProgramArguments(bool allowGroupingByQuotation)
  {
    GTRACE(5, "Getting list of program arguments");
    std::string cmdLine = getProgramArguments();
    return getListOfArguments(cmdLine, allowGroupingByQuotation);
  }


  const std::vector <std::string> CommandLine::getListOfArguments(
      const std::string & arguments, bool allowGroupingByQuotation)
  {
    GTRACE(5, "Getting list of arguments for: [" << arguments
      << "]; quotation marks enabled: " << allowGroupingByQuotation);
    // The value to return;
    std::vector <std::string> returnValue;

    std::string cmdLine = arguments;
    std::string::size_type location = cmdLine.find_first_of(" ");
    std::string temp("");

    // These are only used when we require grouping of items due to double quotation marks being present.
    std::string accumulator;
    bool quotationStarted = false;

    // Go through all space signs.
    while (location != std::string::npos)
    {
      // store the parameter read until white space is encountered.
      temp = cmdLine.substr(0, location);
      cmdLine = cmdLine.substr(location + 1);
      location = cmdLine.find_first_of(" ");

      if (allowGroupingByQuotation)
      {
        // Check to see if there is any double quotation mark present.
        std::string::size_type subLocation = temp.find_first_of("\"");
        if (subLocation != std::string::npos)
        {
          std::string subTemp(temp.substr(0, subLocation));
          while (subLocation != std::string::npos)
          {
            GTRACE(6, "subtemp= [" << subTemp << "]");
            if (quotationStarted)
            {
              // Add a space if it isn't the first sub item
              if (accumulator.length() > 0)
              {
                accumulator += std::string(" ");
              }

              // Add the sub-item.
              accumulator += subTemp;
              if (accumulator.length() > 0)
              {
                returnValue.push_back(accumulator);
              }
              accumulator.clear();
            }
            else
            {
              // No quotation is started, just add the items.
              accumulator = subTemp;
            }
            quotationStarted = !quotationStarted;

            temp = temp.substr(subLocation + 1);
            subLocation = temp.find_first_of("\"");
            subTemp = temp.substr(0, subLocation);
          }

          if (subTemp.length() > 0)
          {
            accumulator += subTemp;
          }
        }
        else
        {
          // No quotation mark present.
          if (quotationStarted)
          {
            // We started reading something with quotation marks for some time now.
            accumulator += temp;
          }
          else
          {
            // add the parameters, as usual
            if (temp.length() >0)
            {
              returnValue.push_back(temp);
            }
          }
        }
      }
      else
      {
        if (temp.length() > 0)
        {
          returnValue.push_back(temp);
        }
      }
    }

    if (cmdLine.length() > 0)
    {
      returnValue.push_back(cmdLine);
    }

    return returnValue;
  }

  void CommandLine::readProgramArgumentsAndCount(char *** inArguments, int & argCount)
  {
    std::vector <std::string> arglist;
    arglist = getListOfProgramArguments();

    * inArguments = new char *[arglist.size()];
    for (unsigned i = 0; i < arglist.size(); ++i)
    {
      char * chTemp = new char[arglist[i].length() + 1];
      chTemp[arglist[i].length()] = '\0';

      GTRACE(7, "allocated (+1):" << arglist[i].length() + 1);
      GTRACE(7, "lsize:" << arglist[i].size());
      GTRACE(7, "copying to it: [" << arglist[i].c_str() << "]");

      strcpy_s(chTemp, arglist[i].length(), arglist[i].c_str());
      GTRACE(5, "args= ["<< chTemp << "]");

      (*inArguments)[i] = chTemp;
    }

    GTRACE(5, "args="<< inArguments);
    argCount = arglist.size();
  }
}
