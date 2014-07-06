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
// Own header
//
#include "guslib/util/configuration.h"

//
// C++ system headers
//
#include <map>
#include <string>
#include <vector>

//
// This libraries' headers
//
#include "guslib/common/simpleexception.h"

// Include the smart pointer.
#include "guslib/thirdparty/yasper.h"

#include "guslib/trace/trace.h"
#include "guslib/util/config.h"
#include "guslib/util/stringutil.h"



namespace guslib
{
  class IniLoader;

  std::string Property::empty_ = "";

  // ----------------------------------------------------------------------------

  Property::Property(const std::string& name)
    : name_(name),
      propType_(CFG_UNDEFINED)
  {
    GTRACE(7, "Property CTor [" << getName() << "]");
  }

  Property::Property()
    : name_("property"),
      propType_(CFG_UNDEFINED)
  {
    GTRACE(7, "Property CTor [" << getName() << "]");
  }

  Property::~Property()
  {
    GTRACE(7, "Property DTor [" << getName() << "]");
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
    if (content_.size() <= 0)
    {
      return empty_;
    }

    return content_[0];
  }

  std::vector<std::string> Property::getAsStringList() const
  {
    return content_;
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
    if (content_.size() == 1)
    {
      content_[0] = stringutil::BoolToString(value);
    }
    else
    {
      content_.clear();
      content_.push_back(stringutil::BoolToString(value));
    }

    propType_ = CFG_BOOL;
  }


  void Property::setAsInt(int value)
  {
    if (content_.size() == 1)
    {
      content_[0] = stringutil::IntToString(value);
    }
    else
    {
      content_.clear();
      content_.push_back(stringutil::IntToString(value));
    }

    propType_ = CFG_INT;
  }


  void Property::setAsDouble(double value)
  {
    if (content_.size() == 1)
    {
      content_[0] = stringutil::DoubleToString(value);
    }
    else
    {
      content_.clear();
      content_.push_back(stringutil::DoubleToString(value));
    }
    propType_ = CFG_DOUBLE;
  }


  void Property::setAsString(const std::string & value)
  {
    GTRACE(6, "Prop set as string; " << value);
    if (content_.size() == 1)
    {
      content_[0] = value;
    }
    else
    {
      content_.clear();
      content_.push_back(value);
    }
    propType_ = CFG_STRING;
  }


  void Property::setAsVector(const std::vector <std::string> & values)
  {
    content_ = values;
    propType_ = CFG_LST_STRING;
  }



  PropertyGroup::PropertyGroup(const std::string& name)
    : name_(name),
      savable_(true)
  {
    GTRACE(7, "Property Group CTor [" << getName() << "]");
  }

  PropertyGroup::PropertyGroup()
    : name_("general"),
      savable_(true)
  {
    GTRACE(7, "Property Group CTor [" << getName() << "]");
  }

  PropertyGroup::~PropertyGroup()
  {
    GTRACE(7, "Property Group DTor [" << getName() << "]");
  }


  void PropertyGroup::addProperty(const Property & prop)
  {
    propGroup_.push_back(prop);
  }


  Property& PropertyGroup::operator[](const std::string& name)
  {
    for (std::vector <Property>::iterator it = propGroup_.begin();
      it != propGroup_.end(); ++it)
    {
      if (it->getName() == name)
      {
        return *it;
      }
    }

    // if this region is reached, no such property was found... what to do?
    // create an empty one.

    propGroup_.push_back(Property(name));
    return propGroup_.back();
  }

  //
  //  ------------------------------------ Configuration Internals ------------------------------------------------
  //

  /**
    Opaque pointer: private implementation of scene.
  */
  class Configuration::Impl
  {
  public:
    yasper::ptr<ConfigLoader> loaderSharedPtr_;

    /**
      Constructor.
    */
    Configuration::Impl()
      : loaderSharedPtr_(0)
    {
    }

    /**
      Copy this
    */
    Impl(const Impl& rhs)
    {
      this->loaderSharedPtr_ = rhs.loaderSharedPtr_;
      if (false == rhs.loaderSharedPtr_.IsValid())
      {
        GTRACE(3, "ERROR: Configuration copy constructor: Invalid shared ptr!");
      }
    }
  };

  //
  // -------------------- Configuration main class ----------------------------
  //

  /**
    Constructor.
  */
  Configuration::Configuration()
    : impl_(new Configuration::Impl())
  {
    Config::init();
    GTRACE(3, "Created Configuration");
  }

  /**
    Copy Constructor.
  */
  Configuration::Configuration(const Configuration& rhs)
  {
    this->defaultGroup_ = rhs.defaultGroup_;
    this->groups_ = rhs.groups_;
    this->impl_ = new Configuration::Impl(*rhs.impl_);
    
    GTRACE(3, "copied Configuration");
  }

  /**
    Destructor.
  */
  Configuration::~Configuration()
  {
    delete impl_;
    GTRACE(3, "Destroyed Configuration");
  }

  PropertyGroup& Configuration::getGroup(const std::string& name)
  {
    std::map <std::string, PropertyGroup>::iterator it = groups_.find(name);

    if (it != groups_.end())
    {
      return it->second;
    }

    // error scenario
    // a group was requested; this was previously unknown.
    // add it to the group list?

    addGroup(name);
    it = groups_.find(name);
    if (it != groups_.end())
    {
      return it->second;
    }

    // exception scenario... we just added it to the map... what gives?
    throw guslib::SimpleException("Get group: could not add a group");
  }

  PropertyGroup& Configuration::operator[](const std::string& name)
  {
    return getGroup(name);
  }


  void Configuration::clear()
  {
    groups_.clear();
  }

  void Configuration::addGroup(const std::string& name)
  {
    groups_.insert(std::make_pair(name, PropertyGroup(name)));
  }

  void Configuration::addGroup(const PropertyGroup & group)
  {
    groups_.insert(std::make_pair(group.getName(), group));
  }

  bool Configuration::load(const std::string & fileName)
  {
    bool bReturn(false);

    GTRACE(3, "Configuration - loading from " << fileName);
    std::string fileExtension = stringutil::GetExtensionFromFileName(fileName);   // E.g. "ini";

    // Use the loader based on the extension.
    impl_->loaderSharedPtr_ = ConfigLoaderFactory::getPtr()->CreateObject(fileExtension);
    if (impl_->loaderSharedPtr_)
    {
      bReturn = impl_->loaderSharedPtr_->load(*this, fileName);
    }

    GTRACE(5, "Configuration - DONE loading from " << fileName);

    return bReturn;
  }


  void Configuration::save()
  {
    if (NULL == impl_->loaderSharedPtr_)
    {
      GTRACE(3, "Tried to save configuration, but no loader is available");
      return;
    }

    GTRACE(5, "Configuration - saving");
    impl_->loaderSharedPtr_->save(*this);
  }

  void Configuration::saveAs(const std::string& fileName)
  {
    if (NULL == impl_->loaderSharedPtr_)
    {
      GTRACE(3, "Tried to save configuration, but no loader is available");
      return;
    }

    GTRACE(5, "Configuration - saving as...");
    impl_->loaderSharedPtr_->save(*this, fileName);
  }

  ConfigLoader::ConfigLoader()
  {
  }

  ConfigLoader::~ConfigLoader()
  {
  }

  void ConfigLoader::save(Configuration & configToSave)
  {
    save(configToSave, loadedFileName_);
  }
}
