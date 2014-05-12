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
//   Application configuration utility.
//
//   Last change:  $LastChangedDate: 2014-04-13 22:58:12 +0300 (D, 13 apr. 2014) $
//   Revision:    $Revision: 647 $

//
// Includes
//

//
// Own header.
//
#include "guslib/util/config.h"

//
// This library's headers
//

// Add the "ini" configuration to the factory.
#include "guslib/util/iniconfig.h"

// Add the configuration class.
#include "guslib/util/configuration.h"

namespace guslib
{
  bool Config::calledInit_ = false;

  /**
    Initialize the factory.
  */
  void Config::init()
  {
    if (calledInit_)
    {
      // ran once, no need to run again!
      return;
    }

    calledInit_ = true;
    ConfigLoaderFactory::getPtr()->Register("ini", IniLoader::createLoader);
  }
}
