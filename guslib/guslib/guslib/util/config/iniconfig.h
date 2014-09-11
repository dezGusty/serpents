#ifndef GUS_LIB_INICONFIG_H
#define GUS_LIB_INICONFIG_H

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
//   Last change:  $LastChangedDate: 2014-09-10 22:37:41 +0200 (Mi, 10 sep. 2014) $
//   Revision:    $Revision: 671 $

//
// Includes
//

//
// C++ system headers.
//

#include <string>

//
// This library's headers
//

// using the abstract factory
#include "guslib/common/abstractfactory.hpp"

// Make sure that whenever simple ini is used, it is using iostreams.
#define SI_SUPPORT_IOSTREAMS
#include "guslib/thirdparty/SimpleIni.h"

#include "guslib/util/config/configuration.h"
#include "guslib/util/config/loader.h"

namespace guslib
{
  namespace config
  {
    class IniLoader : public Loader
    {
    protected:
      CSimpleIniA ini;

    public:
      static config::Loader* createLoader();

      IniLoader();
      IniLoader(const IniLoader& rhs);
      virtual ~IniLoader();

      virtual bool load(config::Configuration & configToLoad, const std::string& fileName);
      virtual void save(config::Configuration & configToSave, const std::string& fileName);

      //virtual Loader* clone();
    };
  }

}  // end namespace guslib

#endif  // GUS_LIB_INICONFIG_H
