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
// Own header.
//
#include "guslib/util/config/property.h"

//
// C++ system includes
//

#include <string>
#include <vector>

//
// This libraries' headers
//
//#include "guslib/common/simpleexception.h"
#include "guslib/trace/trace.h"
#include "guslib/util/stringutil.h"

namespace guslib
{
  namespace config
  {
    class Property::Impl
    {
    public:
      static std::string empty_;

      std::vector <std::string> content_;
      std::string name_;

      Property::Impl(const std::string& name)
        : name_(name)
      {

      }
    };

    std::string Property::Impl::empty_ = "";


    Property::Property(const std::string& name)
      : impl_(new Property::Impl(name))
      , propType_(PropertyType::CFG_UNDEFINED)
    {
      GTRACE(7, "Property CTor [" << getName() << "]");
    }

    Property::Property()
      : impl_(new Property::Impl("property"))
      , propType_(PropertyType::CFG_UNDEFINED)
    {
      GTRACE(7, "Property CTor [" << getName() << "]");
    }

    Property::Property(const Property& rhs)
    {
      this->impl_ = new Property::Impl(*rhs.impl_);
      this->propType_ = rhs.propType_;
    }

    Property::~Property()
    {
      GTRACE(7, "Property DTor [" << getName() << "]");
      delete impl_;
    }

    const std::string& Property::getName() const
    {
      return impl_->name_;
    }

    bool Property::getAsBool() const
    {
      return stringutil::StringToBool(getAsString());
    }

    bool Property::getAsBoolOrDefault(bool defaultVal) const
    {
      if (getAsString().length() > 0)
      {
        return getAsBool();
      }
      return defaultVal;
    }

    double Property::getAsDouble() const
    {
      return stringutil::StringToDouble(getAsString());
    }

    double Property::getAsDoubleOrDefaultVal(double defaultVal) const
    {
      if (getAsString().length() > 0)
      {
        return getAsDouble();
      }
      return defaultVal;
    }

    int Property::getAsIntOrDefaultVal(int defaultVal) const
    {
      if (getAsString().length() > 0)
      {
        return getAsInt();
      }
      return defaultVal;
    }

    int Property::getAsInt() const
    {
      return stringutil::StringToInt(getAsString());
    };

    std::string Property::getAsString() const
    {
      if (impl_->content_.size() <= 0)
      {
        return impl_->empty_;
      }

      return impl_->content_[0];
    }

    std::vector<std::string> Property::getAsStringList() const
    {
      return impl_->content_;
    }

    std::string Property::getAsStringOrDefaultVal(const std::string& defaultVal) const
    {
      if (getAsString().length() > 0)
      {
        return getAsString();
      }
      return defaultVal;
    }


    void Property::setAsBool(bool value)
    {
      if (impl_->content_.size() == 1)
      {
        impl_->content_[0] = stringutil::BoolToString(value);
      }
      else
      {
        impl_->content_.clear();
        impl_->content_.push_back(stringutil::BoolToString(value));
      }

      propType_ = PropertyType::CFG_BOOL;
    }


    void Property::setAsInt(int value)
    {
      if (impl_->content_.size() == 1)
      {
        impl_->content_[0] = stringutil::IntToString(value);
      }
      else
      {
        impl_->content_.clear();
        impl_->content_.push_back(stringutil::IntToString(value));
      }

      propType_ = PropertyType::CFG_INT;
    }


    void Property::setAsDouble(double value)
    {
      if (impl_->content_.size() == 1)
      {
        impl_->content_[0] = stringutil::DoubleToString(value);
      }
      else
      {
        impl_->content_.clear();
        impl_->content_.push_back(stringutil::DoubleToString(value));
      }

      propType_ = PropertyType::CFG_DOUBLE;
    }


    void Property::setAsString(const std::string & value)
    {
      GTRACE(6, "Prop set as string; " << value);
      if (impl_->content_.size() == 1)
      {
        impl_->content_[0] = value;
      }
      else
      {
        impl_->content_.clear();
        impl_->content_.push_back(value);
      }

      propType_ = PropertyType::CFG_STRING;
    }


    void Property::setAsVector(const std::vector <std::string> & values)
    {
      impl_->content_ = values;
      propType_ = PropertyType::CFG_LST_STRING;
    }

  }
}   // end namespace guslib

