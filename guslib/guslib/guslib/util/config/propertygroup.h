#ifndef GUS_LIB_CONFIGURATION_PROPERTY_GROUP_H
#define GUS_LIB_CONFIGURATION_PROPERTY_GROUP_H

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

#include "guslib/util/config/property.h"

namespace guslib
{
  namespace config
  {
    /**
      A group of configuration properties.
      E.g. A set of network related application options could be a group. A set of UI related options could be
      another group.
      */
    class GUSLIB_EXPORT_SYMBOL PropertyGroup
    {
    private:
      class Impl;
      Impl* impl_;

    public:
      PropertyGroup();
      explicit PropertyGroup(const std::string &name);

      /**
        Copy constructor. Needed to copy the pimpl.
      */
      PropertyGroup(const PropertyGroup& rhs);
      ~PropertyGroup();

      Property& operator[](const std::string& name);

      void addProperty(const Property & prop);

      const std::string& getName() const;

      const std::vector <Property>& getProperties() const;

      const bool isSavable() const;

      void setSavable(bool rhs);
    };
  }
}   // end namespace guslib

#endif  // GUS_LIB_CONFIGURATION_PROPERTY_H
