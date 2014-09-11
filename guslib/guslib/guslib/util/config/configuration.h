#ifndef GUS_LIB_CONFIGURATION_H
#define GUS_LIB_CONFIGURATION_H

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
//   Last change:  $LastChangedDate: 2014-09-10 22:37:41 +0200 (Mi, 10 sep. 2014) $
//   Revision:    $Revision: 671 $

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

// Using the std::string
#include <string>

// Using the std::vector
#include <vector>

// Using the std::map
#include <map>

//
// This library's headers.
//

// using the abstract factory
#include "guslib/common/abstractfactory.hpp"

// using the singleton
#include "guslib/common/singleton.hpp"

//#include "guslib/util/config/loader.h"
#include "guslib/util/config/property.h"
#include "guslib/util/config/propertygroup.h"

namespace guslib
{
  namespace config
  {
    //
    // Fwd declarations.
    //
    class Property;
    class PropertyGroup;
    class Loader;
    class Configuration;

    /**
      Stores a configuration. This consists of several groups, each with their own properties.
      TODO: property links (same property, available in 2 different groups)?
      */
    class GUSLIB_EXPORT_SYMBOL Configuration
    {
    private:
      class Impl;
      Configuration::Impl* impl_;

    public:
      /**
        Constructor.
        */
      Configuration();

      /**
        Copy constructor. Needed to copy the pimpl.
      */
      Configuration(const Configuration& rhs);

      /**
        Destructor.
        */
      virtual ~Configuration();

      Configuration& operator=(const Configuration &); // disable

      PropertyGroup& operator[](const std::string& name);
      PropertyGroup& getGroup(const std::string& name);

      const std::map <std::string, PropertyGroup>& getGroups();

      // clear the configuration.
      virtual void clear();

      virtual void addGroup(const std::string& name);
      virtual void addGroup(const PropertyGroup & group);

      // Load the configuration from a file.
      // @return true if loaded; false if something went wrong.
      virtual bool load(const std::string & fileName);

      // Save into the same source as the one loaded from.
      // If not loaded from a source - will throw an exception.
      virtual void save();

      virtual void saveAs(const std::string& fileName);
    };

    typedef guslib::Singleton <
      guslib::AbstractFactory < config::Loader, std::string, config::Loader*(*)() >
    > LoaderFactory;

  }
}   // end namespace guslib

#endif  // GUS_LIB_CONFIGURATION_H
