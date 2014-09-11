#ifndef GUS_LIB_CONFIGURATION_PROPERTY_H
#define GUS_LIB_CONFIGURATION_PROPERTY_H

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

namespace guslib
{
  namespace config
  {
    /// TODO: do we really need this?
    enum class PropertyType
    {
      CFG_UNDEFINED,  // 0
      CFG_INT,    // 1
      CFG_STRING,    // 2
      CFG_DOUBLE,    // 3
      CFG_BOOL,    // 4
      CFG_LST_STRING,  // 5
      CFG_NUM_TYPES
    };

    //
    // These are the classes that shall be defined in this file.
    //
    class Property;

    /**
      A configuration property
      */
    class GUSLIB_EXPORT_SYMBOL Property
    {
    private:
      PropertyType propType_;

      class Impl;
      Impl* impl_;
    public:
      Property();
      explicit Property(const std::string &name);

      /**
        Copy constructor. Needed to copy the pimpl.
      */
      Property(const Property& rhs);
      virtual ~Property();

      const std::string& getName() const;

      PropertyType getType() const
      {
        return propType_;
      }

      virtual bool getAsBool() const;
      virtual int getAsInt() const;
      virtual double getAsDouble() const;
      virtual std::string getAsString() const;  // create a copy
      virtual std::vector<std::string> getAsStringList() const;

      virtual bool getAsBoolOrDefault(bool defaultVal) const;
      virtual int getAsIntOrDefaultVal(int defaultVal) const;
      virtual double getAsDoubleOrDefaultVal(double defaultVal) const;
      virtual std::string getAsStringOrDefaultVal(const std::string& defaultVal) const;  // create a copy

      virtual void setAsBool(bool valueToSet);
      virtual void setAsInt(int valueToSet);
      virtual void setAsDouble(double valueToSet);
      virtual void setAsString(const std::string& valueToSet);
      virtual void setAsVector(const std::vector <std::string>&  valueToSet);

      virtual void setPropType(PropertyType propType)
      {
        propType_ = propType;
      }
    };
  }
}   // end namespace guslib

#endif  // GUS_LIB_CONFIGURATION_PROPERTY_H
