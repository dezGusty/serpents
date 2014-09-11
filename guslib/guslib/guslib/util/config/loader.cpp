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
//   Last change:  $LastChangedDate$
//   Revision:    $Revision$

//
// Includes
//

//
// Own header.
//
#include "guslib/util/config/loader.h"

//
// C++ system includes
//

namespace guslib
{
  namespace config
  {
    class Loader::Impl
    {
    public:
      std::string loadedFileName_;

      Loader::Impl(const std::string& loadedFileName)
        : loadedFileName_(loadedFileName)
      {

      }
    };

    Loader::Loader()
      : impl_(new Loader::Impl(""))
    {
    }

    Loader::Loader(const Loader& rhs)
      : impl_(new Loader::Impl(rhs.impl_->loadedFileName_))
    {
    }


    Loader::~Loader()
    {
      delete impl_;
    }

    void Loader::storeLoadedFileName_(const std::string& fileName)
    {
      impl_->loadedFileName_ = fileName;
    }


    void Loader::save(Configuration & configToSave)
    {
      save(configToSave, impl_->loadedFileName_);
    }
  }
}   // end namespace guslib

