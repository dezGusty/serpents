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
//   Last change:  $LastChangedDate: 2014-04-13 22:58:12 +0300 (D, 13 apr. 2014) $
//   Revision:    $Revision: 647 $

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

// Include the smart pointer.
#include "guslib/thirdparty/yasper.h"


namespace guslib
{
  /// TODO: do we really need this?
  typedef enum
  {
    CFG_UNDEFINED,  // 0
    CFG_INT,    // 1
    CFG_STRING,    // 2
    CFG_DOUBLE,    // 3
    CFG_BOOL,    // 4
    CFG_LST_STRING,  // 5
    CFG_NUM_TYPES
  }PropertyType;

  //
  // These are the classes that shall be defined in this file.
  //
  class Property;
  class PropertyGroup;
  class ConfigLoader;
  class Configuration;


#pragma warning(push)
#pragma warning(disable:4251)
  /**
    A configuration property
  */
  class GUSLIB_EXPORT_SYMBOL Property
  {
  private:
    std::vector <std::string> content_;
    std::string name_;

    static std::string empty_;
    PropertyType propType_;
  public:
    Property();
    explicit Property(const std::string &name);
    virtual ~Property();

    const std::string& getName() const
    {
      return name_;
    }

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

#pragma warning(pop)


#pragma warning(push)
#pragma warning(disable:4251)
  /**
    A group of configuration properties.
    E.g. A set of network related application options could be a group. A set of UI related options could be 
    another group.
  */
  class GUSLIB_EXPORT_SYMBOL PropertyGroup
  {
  private:
    std::string name_;

    std::vector <Property> propGroup_;

    bool savable_;

  public:
    PropertyGroup();
    explicit PropertyGroup(const std::string &name);
    ~PropertyGroup();

    Property& operator[](const std::string& name);

    void addProperty(const Property & prop);

    const std::string& getName() const
    {
      return name_;
    }

    const std::vector <Property>& getProperties() const
    {
      return propGroup_;
    }

    const bool isSavable() const
    {
      return savable_;
    }

    void setSavable(bool rhs)
    {
      savable_ = rhs;
    }
  };
#pragma warning(pop)



#pragma warning(push)
#pragma warning(disable:4251)
  /**
    Holds the specifics of the save and load implementations. Could be an ini, XML, registry, etc.
  */
  class GUSLIB_EXPORT_SYMBOL ConfigLoader
  {
  protected:
    std::string loadedFileName_;

  public:
    ConfigLoader();
    virtual ~ConfigLoader();

    virtual bool load(Configuration & configToLoad, const std::string& fileName) = 0;
    virtual void save(Configuration & configToSave, const std::string& fileName) = 0;
    virtual void save(Configuration & configToSave);

    static ConfigLoader * createLoader();
  };
#pragma warning(pop)


#pragma warning(push)
#pragma warning(disable:4251)
  /**
    Stores a configuration. This consists of several groups, each with their own properties.
    TODO: property links (same property, available in 2 different groups)?
  */
  class GUSLIB_EXPORT_SYMBOL Configuration
  {
  private:
    std::map <std::string, PropertyGroup> groups_;

    // the default group to use for setting/retrieving properties.
    std::string defaultGroup_;

    yasper::ptr<ConfigLoader> loaderSharedPtr_;

  public:
    Configuration();
    Configuration(const Configuration& rhs);
    virtual ~Configuration();

    PropertyGroup& operator[](const std::string& name);
    PropertyGroup& getGroup(const std::string& name);

    const std::map <std::string, PropertyGroup>& getGroups()
    {
      return groups_;
    }

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
#pragma warning(pop)

  typedef guslib::Singleton<
    guslib::AbstractFactory<ConfigLoader, std::string, ConfigLoader*(*)()>
  > ConfigLoaderFactory;

}   // end namespace guslib

#endif  // GUS_LIB_CONFIGURATION_H
