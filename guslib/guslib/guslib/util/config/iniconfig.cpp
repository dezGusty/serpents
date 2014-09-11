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
// Own header
//
#include "guslib/util/config/iniconfig.h"

//
// C++ system headers
//

#include <map>
#include <string>
#include <vector>

//
// This library's headers.
//

#include "guslib/trace/trace.h"

namespace guslib
{
  namespace config
  {
    IniLoader::IniLoader()
      : config::Loader()
    {
    }

    IniLoader::IniLoader(const IniLoader& rhs)
      : config::Loader()
    {
    }

    IniLoader::~IniLoader()
    {
      GTRACE(3, "Destroying Ini Loader");
    }

    config::Loader* IniLoader::createLoader()
    {
      return new IniLoader();
    }

    //config::Loader* IniLoader::clone()
    //{
    //  return new IniLoader(*this);
    //}

    bool IniLoader::load(Configuration & configToLoad, const std::string& fileName)
    {
      this->storeLoadedFileName_(fileName);

      ini.SetMultiKey(true);
      SI_Error myLoadError;

      myLoadError = ini.LoadFile(fileName.c_str());
      if (myLoadError != SI_OK)
      {
        GTRACE(2, "ERROR: Failed to load configFile: [" << fileName << "]");
        return false;
      }

      CSimpleIniA::TNamesDepend sections;
      ini.GetAllSections(sections);

      for (CSimpleIniA::TNamesDepend::iterator it = sections.begin(); it != sections.end(); ++it)
      {
        std::string sectionName{ it->pItem };
        configToLoad.addGroup(sectionName);

        CSimpleIniA::TNamesDepend keys;
        ini.GetAllKeys(sectionName.c_str(), keys);

        for (CSimpleIniA::TNamesDepend::iterator it2 = keys.begin(); it2 != keys.end(); ++it2)
        {
          std::string keyName(it2->pItem);

          CSimpleIniA::TNamesDepend values;
          ini.GetAllValues(sectionName.c_str(), keyName.c_str(), values);

          std::vector <std::string> valuesVector;
          for (CSimpleIniA::TNamesDepend::iterator it3 = values.begin(); it3 != values.end(); ++it3)
          {
            valuesVector.push_back(it3->pItem);
          }

          if (valuesVector.size() == 1)
          {
            configToLoad.getGroup(sectionName)[keyName].setAsString(valuesVector[0]);
            GTRACE(8, "Loaded: [" << sectionName << "][" << keyName << "] = [" << valuesVector[0] << "]");
          }
          else
          {
            GTRACE(8, "Loaded: [" << sectionName << "][" << keyName << "] = {vector}");
            configToLoad.getGroup(sectionName)[keyName].setAsVector(valuesVector);
          }
        }
      }

      return true;
    }


    void IniLoader::save(Configuration & configToSave, const std::string& fileName)
    {
      GTRACE(3, "IniLoader --> save config to: " << fileName);

      std::map <std::string, PropertyGroup> groups = configToSave.getGroups();
      for (std::map <std::string, PropertyGroup>::const_iterator it = groups.begin();
        it != groups.end(); ++it)
      {
        // If this group is marked as not-savable, skip over it.
        if (false == it->second.isSavable())
        {
          continue;
        }

        std::string groupName{ it->second.getName() };
        GTRACE(5, "SAVE - Group : " << groupName);

        std::vector <Property> props = it->second.getProperties();
        for (std::vector <Property>::iterator it2 = props.begin();
          it2 != props.end(); ++it2)
        {
          GTRACE(5, "SAVE - Prop " << it2->getName() << " - " << it2->getAsString());
          std::string propName{ it2->getName() };
          PropertyType propType{ it2->getType() };
          std::string propValue;

          switch (propType)
          {
          case PropertyType::CFG_STRING:
          case PropertyType::CFG_BOOL:
          case PropertyType::CFG_INT:
          case PropertyType::CFG_DOUBLE:
            propValue = it2->getAsString();
            ini.SetValue(groupName.c_str(), propName.c_str(), propValue.c_str(), nullptr, true);
            break;
          case PropertyType::CFG_LST_STRING:
          {
            std::vector<std::string> vec{ it2->getAsStringList() };
            for (std::vector<std::string>::const_iterator it3 = vec.begin();
              it3 != vec.end(); ++it3)
            {
              propValue = *it3;
              if (it3 == vec.begin())
              {
                ini.SetValue(groupName.c_str(), propName.c_str(), propValue.c_str(), nullptr, true);
              }
              else
              {
                ini.SetValue(groupName.c_str(), propName.c_str(), propValue.c_str(), nullptr, false);
              }
            }
          }
            break;
          default:
            break;
          }
        }
      }

      SI_Error err = ini.SaveFile(fileName.c_str());
    }
  }
}  // end namespace guslib
