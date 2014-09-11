#ifndef GUSLIB_CONFIG_LOADER_H
#define GUSLIB_CONFIG_LOADER_H

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
//   Last change:  $LastChangedDate: 2014-07-04 08:56:45 +0300 (V, 04 iul. 2014) $
//   Revision:    $Revision: 663 $

//
// Includes
//

//
// Platform specific definitions and overall build options for the library.
//
#include <guslib/guslibbuildopts.h>

//
// C++ system includes
//

#include <string>
#include <vector>

#include "guslib/util/config/configuration.h"
#include "guslib/util/config/property.h"

namespace guslib
{
  namespace config
  {
    /**
      Holds the specifics of the save and load implementations. Could be an ini, XML, registry, etc.
      */
    class GUSLIB_EXPORT_SYMBOL Loader
    {
    private:
      class Impl;
      Impl* impl_;

    protected:
      void storeLoadedFileName_(const std::string& fileName);

    public:
      Loader();
      Loader(const Loader& rhs);
      virtual ~Loader();

      virtual bool load(Configuration & configToLoad, const std::string& fileName) = 0;
      virtual void save(Configuration & configToSave, const std::string& fileName) = 0;
      virtual void save(Configuration & configToSave);

      //virtual Loader* clone() = 0;
      static Loader* createLoader();
    };
  }
}   // end namespace guslib

#endif  // GUSLIB_CONFIG_LOADER_H
