#ifndef GUS_LIB_THREAD_H
#define GUS_LIB_THREAD_H

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
//   Last change:  $LastChangedDate: 2014-04-13 22:58:12 +0300 (D, 13 apr. 2014) $
//   Revision:    $Revision: 647 $

#include <guslib/guslibbuildopts.h>

// Disable a conversion warning for boost thread sleep.
#pragma warning(disable:4244)

#if GUSLIB_THREAD_SUPPORT == 0
// No thread support
#elif GUSLIB_THREAD_SUPPORT == 1
#  if GUSLIB_THREAD_PROVIDER == 2
#    include <guslib/system/threadingdefstthread.h>
#    include <guslib/thirdparty/tinythread.h>
#  elif GUSLIB_THREAD_PROVIDER == 3
#    include <guslib/system/threadingdefsboost.h>

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

namespace guslib
{
    typedef boost::mutex::scoped_lock GScopedLock;
    typedef boost::mutex GMutex;
    typedef boost::thread GThread;

    static void yield()
    {
        boost::this_thread::yield();
    }
    static void sleep(unsigned millis)
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(millis));
    }
}

#  else

#pragma message("Warning: Multithreaded synchronization disabled! Define GUSLIB_FLAG_MULTITHREAD to enable it")
#pragma message("(requires boost libs).")

namespace guslib
{
    static void yield()
    {
        ::Sleep(0);
    }
    static void sleep(unsigned millis)
    {
        ::Sleep(millis);
    }

    class GMutex
    {
    };

    class GScopedLock
    {
    public:
        explicit GScopedLock(const GMutex&)
        {
        }
    };

    class GThread
    {
    };
}
#  endif  // GUSLIB_THREAD_PROVIDER

#endif  // GUSLIB_THREAD_SUPPORT

#endif  // GUS_LIB_THREAD_H
