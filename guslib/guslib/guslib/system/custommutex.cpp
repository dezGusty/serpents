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
//   Last change:  $LastChangedDate$
//   Revision:    $Revision$

//
// Includes
//

// Own header
#include "guslib/system/custommutex.h"

//
// C++ system headers
//
#include <mutex>

namespace guslib
{
  class CustomMutex::Impl
  {
  public:
    std::recursive_mutex creationMutex_;
  };

  CustomMutex::CustomMutex()
    : impl_(new Impl())
  {
  }

  CustomMutex::~CustomMutex()
  {
    delete impl_;
  }

  void CustomMutex::lock()
  {
    impl_->creationMutex_.lock();
  }

  void CustomMutex::unlock()
  {
    impl_->creationMutex_.unlock();
  }


  CustomLock::CustomLock(CustomMutex& mutex)
    : mutex_(mutex)
  {
    mutex.lock();
  }

  CustomLock::~CustomLock()
  {
    mutex_.unlock();
  }


}


