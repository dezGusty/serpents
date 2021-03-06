#ifndef GUS_LIB_SINGLETONHOLDER_H
#define GUS_LIB_SINGLETONHOLDER_H

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
//   Last change:  $LastChangedDate: 2014-09-04 08:43:29 +0300 (J, 04 sep. 2014) $
//   Revision:    $Revision: 670 $

//
// Includes
//

//
// Platform specific definitions and overall build options for the library.
//
#include <guslib/guslibbuildopts.h>

#if GUSLIB_THREAD_SUPPORT == 1
# include <thread>
#endif

#include "guslib/common/simpleexception.h"

namespace guslib
{
  // Utility class to encapsulate a singleton.
  // This doesn not create the singleton, but holds onto it and CAN destroy it.
  // Limitation: This can only handle singletons with public, default constructors.
  // Note: a double checked locking is attempted at the object's destruction, if multithreaded
  // synchronization is enabled (it will depend on 3rd party libraries for thread utilities, such
  // as boost or Poco). To enable the multithreaded synchronization, you MUST define
  // GUSLIB_FLAG_MULTITHREAD to 1.
  //
  // Note on storing these classes in a DLL:
  // In order to have an actual singleton, with a single instance, you must export the symbol to the DLL.
  // Make sure you do this by defining the flag "GUSLIB_EXP" in the DLL. The calling code must use the
  // 'GUSLIB_IMP' preprocessor flag.
  template <class T> class SingletonHolder
  {
  private:
    static T * objectPtr_;

  protected:
#if GUSLIB_THREAD_SUPPORT == 1
    static std::recursive_mutex creationMutex_;
#endif

  public:
    // Get a pointer to the SingletonHolder instance.
    static T * getPtr()
    {
      // first check if it was created. If the mutex would be placed first, a performance hit would occur, since
      // all operations would create and release a lock.
      if (nullptr == objectPtr_)
      {
        throw guslib::SimpleException("SingletonHolder getting nullptr");
      }
      return objectPtr_;
    }   //  getptr

    // Assign the pointer to the singleton instance.
    static void setPtr(T * ptr)
    {
      if (nullptr == ptr)
      {
        throw guslib::SimpleException("SingletonHolder being assigned nullptr");
      }
#if GUSLIB_THREAD_SUPPORT == 1
      std::lock_guard<std::recursive_mutex> lg{ creationMutex_ };
#endif
      objectPtr_ = ptr;
    }

    // Destroy the singleton instance. Again, try a double checked lock.
    static void destroy()
    {
      if (objectPtr_)
      {
#if GUSLIB_THREAD_SUPPORT == 1
        std::lock_guard<std::recursive_mutex> lg{ creationMutex_ };
#endif
        if (objectPtr_)
        {
          delete objectPtr_;
          objectPtr_ = nullptr;
        }
      }
    }   //  destroy
  };

#if GUSLIB_FLAG_SINGLETONINST != 0
  template <class T> T* SingletonHolder<T>::objectPtr_ = nullptr;
#endif

#if GUSLIB_THREAD_SUPPORT == 1
  template <class T> std::recursive_mutex SingletonHolder<T>::creationMutex_;
#endif

}   // namespace end

#endif  // GUS_LIB_SINGLETONHOLDER_H
