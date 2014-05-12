#ifndef GUS_LIB_THREADINGDEFSTTHREAD_H
#define GUS_LIB_THREADINGDEFSTTHREAD_H

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
//   Last change:  $LastChangedDate: 2013-06-20 23:19:27 +0200 (J, 20 iun. 2013) $
//   Revision:    $Revision: 574 $

//   Mutex and threading definitions - boost specific. Modelled after the Ogre3D library.

#define GUS_LOCK_MUTEX(name) tthread::lock_guard<tthread::mutex> guslibnameLock(name)

#define GUS_MUTEX(name) tthread::mutex name

#define GUS_MUTEX_TYPE tthread::mutex

#define GUS_THREAD_TYPE tthread::thread

#define GUS_THREAD_CURRENT_ID tthread::this_thread::get_id ()

#define GUS_THREAD_SLEEP(ms) tthread::this_thread::sleep_for (tthread::chrono::milliseconds (ms))

#endif  // GUS_LIB_THREADINGDEFSTTHREAD_H
