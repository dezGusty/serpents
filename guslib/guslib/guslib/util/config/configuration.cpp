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
//   Last change:  $LastChangedDate: 2014-09-11 20:14:06 +0200 (J, 11 sep. 2014) $
//   Revision:    $Revision: 672 $

//
// Includes
//

//
// Own header
//
#include "guslib/util/config/configuration.h"

//
// C++ system headers
//
#include <map>
#include <memory>
#include <string>
#include <vector>

//
// This libraries' headers
//
#include "guslib/common/simpleexception.h"

#include "guslib/trace/trace.h"
#include "guslib/util/config/loader.h"
#include "guslib/util/config.h"
#include "guslib/util/stringutil.h"



namespace guslib
{
  namespace config
  {
    class IniLoader;


    //
    //  ------------------------------------ Configuration Internals ------------------------------------------------
    //

    /**
      Opaque pointer: private implementation of scene.
      */
    class Configuration::Impl
    {
    public:
      std::shared_ptr<Loader> loaderPtr_;
      //SharedPtr_;

      std::map <std::string, PropertyGroup> groups_;

      // the default group to use for setting/retrieving properties.
      std::string defaultGroup_;

      /**
        Constructor.
        */
      Impl()
        : loaderPtr_(nullptr)
        , defaultGroup_("general")
        , groups_(std::map <std::string, PropertyGroup>())
      {
        GTRACE(3, "Config-Internal CTOR");
      }

      Impl(const Impl& rhs)
        : loaderPtr_(rhs.loaderPtr_)
          //loaderPtr_(nullptr)
        , defaultGroup_(rhs.defaultGroup_)
        , groups_(rhs.groups_)
      {
        GTRACE(3, "Config-Internal COPY CTOR");
        /*if (nullptr != rhs.loaderPtr_)
        {
          loaderPtr_ = rhs.loaderPtr_->clone();
        }*/
      }

      ~Impl()
      {
        GTRACE(3, "Config-Internal DTOR");
        //this->loaderSharedPtr_ = nullptr;
        //this->groups_.clear();
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
      GTRACE(3, "Created Configuration. Impl addr: " << impl_);
    }

    /**
      Copy Constructor.
      */
    Configuration::Configuration(const Configuration& rhs)
      : impl_(new Configuration::Impl(*rhs.impl_))
    {
      GTRACE(3, "copied Configuration. Impl addr: " << impl_);
    }

    /**
      Destructor.
      */
    Configuration::~Configuration()
    {
      GTRACE(3, "Destroying Configuration. Impl addr: " << impl_);
      delete impl_;
      GTRACE(3, "Destroyed.");
    }

    const std::map <std::string, PropertyGroup>& Configuration::getGroups()
    {
      return impl_->groups_;
    }

    PropertyGroup& Configuration::getGroup(const std::string& name)
    {
      std::map <std::string, PropertyGroup>::iterator it = impl_->groups_.find(name);

      if (it != impl_->groups_.end())
      {
        return it->second;
      }

      // error scenario
      // a group was requested; this was previously unknown.
      // add it to the group list?

      addGroup(name);
      it = impl_->groups_.find(name);
      if (it != impl_->groups_.end())
      {
        return it->second;
      }

      // exception scenario... we just added it to the map... what gives?
      throw guslib::SimpleException("Get group: could not add a group");
    }

    Configuration& Configuration::operator = (const Configuration& rhs)
    {
      if (this != &rhs)
      {
        if (this->impl_)
        {
          delete this->impl_;
        }

        this->impl_ = new Configuration::Impl(*rhs.impl_);
      }

      return *this;
    }

    PropertyGroup& Configuration::operator[](const std::string& name)
    {
      return getGroup(name);
    }

    void Configuration::clear()
    {
      impl_->groups_.clear();
    }

    void Configuration::addGroup(const std::string& name)
    {
      impl_->groups_.insert(std::make_pair(name, PropertyGroup(name)));
    }

    void Configuration::addGroup(const PropertyGroup & group)
    {
      impl_->groups_.insert(std::make_pair(group.getName(), group));
    }

    bool Configuration::load(const std::string & fileName)
    {
      bool result{ false };

      GTRACE(3, "Configuration - loading from " << fileName);
      std::string fileExtension = stringutil::GetExtensionFromFileName(fileName);   // E.g. "ini";

      // Use the loader based on the extension.

      //impl_->loaderSharedPtr_ = std::shared_ptr < guslib::config::Loader > { config::LoaderFactory::getPtr()->CreateObject(fileExtension)};
      impl_->loaderPtr_.reset(config::LoaderFactory::getPtr()->CreateObject(fileExtension));
      //impl_->loaderPtr_ = config::LoaderFactory::getPtr()->CreateObject(fileExtension);
      result = impl_->loaderPtr_->load(*this, fileName);

      GTRACE(5, "Configuration - DONE loading from " << fileName);

      return result;
    }


    void Configuration::save()
    {
      if (nullptr == impl_->loaderPtr_)
      {
        GTRACE(3, "Tried to save configuration, but no loader is available");
        return;
      }

      GTRACE(5, "Configuration - saving");
      impl_->loaderPtr_->save(*this);
    }

    void Configuration::saveAs(const std::string& fileName)
    {
      if (nullptr == impl_->loaderPtr_)
      {
        GTRACE(3, "Tried to save configuration, but no loader is available");
        return;
      }

      GTRACE(5, "Configuration - saving as...");
      impl_->loaderPtr_->save(*this, fileName);
    }

  }
}
