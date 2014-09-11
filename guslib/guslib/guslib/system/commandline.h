#ifndef GUS_LIB_CMDLINE_H
#define GUS_LIB_CMDLINE_H

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
//   Last change:  $LastChangedDate: 2014-09-02 21:32:49 +0300 (Ma, 02 sep. 2014) $
//   Revision:    $Revision: 669 $

//
// Includes
//

//
// Platform specific definitions and overall build options for the library.
//
#include <guslib/guslibbuildopts.h>

//
// C system includes
//

// Include OS specific headers.
#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
#include <guslib/system/systemwindowsfwd.h>
#endif  // GUSLIB_PLATFORM_TYPE

//
// C++ system includes
//
#include <vector>
#include <string>

namespace guslib
{
  class GUSLIB_EXPORT_SYMBOL CommandLine
  {
  public:
    /**
      Get the full string describing the program arguments.
    */
    static const std::string getProgramArguments();


    /**
      Get a list of program arguments, with the 1st being the actual call to the program itself.
      @param allowGroupingByQuotation (Default: true). Specifies that the quotation marks are 
      grouping arguments together. As an example for: [app param1 "app param 2"]
      getListOfProgramArguments (false) will return 5 items: app, param1, "app, param, 2"
      getListOfProgramArguments (true) will return 3 items: app, param1, "app param 2"
    */
    static const std::vector <std::string> getListOfProgramArguments(bool allowGroupingByQuotation = true);

    /**
      Get a list of program arguments, with the 1st being the actual call to the program itself.
      @param arguments Custom argument list.
      @param allowGroupingByQuotation (Default: true). 
      @see getListOfProgramArguments
    */
    static const std::vector <std::string> getListOfArguments(
      const std::string & arguments, bool allowGroupingByQuotation = true);


    /**
      Get the program arguments and argument count in a typical classic (int main) mode.
      @param[out] arguments The retrieved arguments grouped into a single string.
      @param[out] argCount The retrieved number of arguments.
    */
    static void readProgramArgumentsAndCount(char *** arguments, int & argCount);
  };
}

#endif  // GUS_LIB_CMDLINE_H
