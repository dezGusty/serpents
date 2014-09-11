#ifndef GUS_LIB_TIMER_H
#define GUS_LIB_TIMER_H

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
//   Timer utility.
//
//   Last change:  $LastChangedDate: 2014-09-02 21:32:49 +0300 (Ma, 02 sep. 2014) $
//   Revision:    $Revision: 669 $

//
// Includes
//

//
// Platform specific definitions and overall build options for the library.
//
#include <guslib/guslibbuildopts.h>

//
// C system headers
//

// none

//
// C++ system headers
//
#include <ostream>
#include <string>

//
// This project's headers
//

#include "guslib/common/singleton.hpp"


#if GUSLIB_USE_OGRE_TIMERS == 1
#  include "OgreTimer.h"
#endif

namespace guslib
{
  typedef long long TimeUnits;


  /// Abstract timer class, has the entire timer functionality and logic, but has no access
  /// to time passing functions.
  class GUSLIB_EXPORT_SYMBOL AbstractTimer
  {
  protected:
    TimeUnits  latestTime;       // latest accessed time

  public:
    /// Constructor.
    AbstractTimer();

    /// Destructor.
    virtual ~AbstractTimer();

    /// Access the system time units. Override in child classes.
    virtual TimeUnits getCurrentTimeUnits() = 0;

    /// Resets the timer duration to zero
    virtual void reset();

    /// Gets the time interval from the last reset operation
    virtual TimeUnits elapsed();

    /// Gets the time interval from the last reset operation and then resets the timer
    virtual TimeUnits renew();
  };

#if GUSLIB_USE_OGRE_TIMERS == 1
  /// Ogre implementation using the Ogre timers. Requires the OGRE3D library!
  class GusLibOgreTimer
    : public AbstractTimer
  {
  protected:
    Ogre::Timer timer_;
  public:
    GusLibOgreTimer();
    virtual ~GusLibOgreTimer();
    virtual TimeUnits getCurrentTimeUnits();
    virtual TimeUnits getTimeSinceMidnight() const;
    /// Resets the timer duration to zero
    virtual void reset();
  };
#endif  // GUSLIB_USE_OGRE_TIMERS == 1


#if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
  /// Windows implementation for the abstract timer.
  class GUSLIB_EXPORT_SYMBOL WinTimer : public AbstractTimer
  {
  protected:
  public:
    WinTimer();
    virtual ~WinTimer();
    virtual TimeUnits getCurrentTimeUnits();
    virtual TimeUnits getTimeSinceMidnight() const;
  };
#endif


  // Redirection of the timer.
#if GUSLIB_USE_OGRE_TIMERS == 1
  // The Ogre timer will be the standard accessible timer, through the Timer name.
  typedef GusLibOgreTimer Timer;
#else
#  if GUSLIB_PLATFORM_TYPE == GUSLIB_PLATFORM_TYPE_WINDOWS
    // The windows timer will be the standard accessible timer, through the Timer name.
    typedef WinTimer Timer;
#  else
    // TODO: define a unix timer?
#  endif  // GUSLIB_PLATFORM_TYPE
#endif  // GUSLIB_USE_OGRE_TIMERS == 1


  class GUSLIB_EXPORT_SYMBOL ApplicationClockUtil;


  ///
  /// The application clock utility class.
  ///
  class GUSLIB_EXPORT_SYMBOL ApplicationClockUtil
    : public Timer
  {
  protected:
    ApplicationClockUtil();
    TimeUnits  initialTime_;

  public:
    virtual ~ApplicationClockUtil();
    friend class guslib::Singleton <ApplicationClockUtil>;

    virtual TimeUnits getTimeFromStart();

    /**
      Make the class accessible to ostreams (e.g. cout);
      Because this can be used very often in the code, it needs to be very fast.
    */
    inline friend std::ostream & operator<<(std::ostream& s, const ApplicationClockUtil & cl)
    {
      TimeUnits temp(const_cast<ApplicationClockUtil&>(cl).getTimeFromStart());

      int nMillis = temp % 1000;
      temp = (temp - nMillis) / 1000;

      int nSec = temp % 60;
      temp = (temp - nSec) / 60;

      int nMin = temp % 60;
      temp = (temp - nMin) / 60;

      int nHour = temp % 24;
      temp = (temp - nHour) / 24;

      char caster[4];
      caster[3] = '\0';

      caster[0] = nHour / 10 + '0';
      caster[1] = nHour % 10 + '0';
      caster[2] = '\0';
      s << caster << ":";

      caster[0] = nMin / 10 + '0';
      caster[1] = nMin % 10 + '0';
      caster[2] = '\0';
      s << caster << ":";

      caster[0] = nSec / 10 + '0';
      caster[1] = nSec % 10 + '0';
      caster[2] = '\0';
      s << caster << ".";

      caster[0] = nMillis / 100;
      caster[2] = nMillis % 10 + '0';
      caster[1] = (nMillis - caster[0] * 100) / 10 + '0';
      caster[0] += '0';
      caster[3] = '\0';

      s << caster;
      return s;
    }

    virtual std::string getTimeAsString();
  };


  typedef GUSLIB_EXPORT_SYMBOL guslib::Singleton <ApplicationClockUtil> AppClock;


  ///
  /// The system clock utility class.
  ///
  class GUSLIB_EXPORT_SYMBOL SystemClockUtil
    : public ApplicationClockUtil
  {
  protected:
    SystemClockUtil();

  public:
    friend class guslib::Singleton <SystemClockUtil>;

    // gets the time since the start of day.
    virtual TimeUnits getTimeFromStart();

    /**
      Make the class accessible to ostreams (e.g. cout);
      Because this can be used very often in the code, it needs to be very fast.
    */
    inline friend std::ostream & operator<<(std::ostream& s, const SystemClockUtil & cl)
    {
      TimeUnits temp(const_cast<SystemClockUtil&>(cl).getTimeFromStart());

      int nMillis = temp % 1000;
      temp = (temp - nMillis) / 1000;

      int nSec = temp % 60;
      temp = (temp - nSec) / 60;

      int nMin = temp % 60;
      temp = (temp - nMin) / 60;

      int nHour = temp % 24;
      temp = (temp - nHour) / 24;

      char caster[4];
      caster[3] = '\0';

      caster[0] = nHour / 10 + '0';
      caster[1] = nHour % 10 + '0';
      caster[2] = '\0';
      s << caster << ":";

      caster[0] = nMin / 10 + '0';
      caster[1] = nMin % 10 + '0';
      caster[2] = '\0';
      s << caster << ":";

      caster[0] = nSec / 10 + '0';
      caster[1] = nSec % 10 + '0';
      caster[2] = '\0';
      s << caster << ".";

      caster[0] = nMillis / 100;
      caster[2] = nMillis % 10 + '0';
      caster[1] = (nMillis - caster[0] * 100) / 10 + '0';
      caster[0] += '0';
      caster[3] = '\0';

      s << caster;
      return s;
    }

    /**
      Make the class accessible to ostreams (e.g. cout);
      Because this can be used very often in the code, it needs to be very fast.
    */
    inline friend std::wostream & operator<<(std::wostream& s, const SystemClockUtil & cl)
    {
      TimeUnits temp(const_cast<SystemClockUtil&>(cl).getTimeFromStart());

      int nMillis = temp % 1000;
      temp = (temp - nMillis) / 1000;

      int nSec = temp % 60;
      temp = (temp - nSec) / 60;

      int nMin = temp % 60;
      temp = (temp - nMin) / 60;

      int nHour = temp % 24;
      temp = (temp - nHour) / 24;

      char caster[4];
      caster[3] = '\0';

      caster[0] = nHour / 10 + '0';
      caster[1] = nHour % 10 + '0';
      caster[2] = '\0';
      s << caster << ":";

      caster[0] = nMin / 10 + '0';
      caster[1] = nMin % 10 + '0';
      caster[2] = '\0';
      s << caster << ":";

      caster[0] = nSec / 10 + '0';
      caster[1] = nSec % 10 + '0';
      caster[2] = '\0';
      s << caster << ".";

      caster[0] = nMillis / 100;
      caster[2] = nMillis % 10 + '0';
      caster[1] = (nMillis - caster[0] * 100) / 10 + '0';
      caster[0] += '0';
      caster[3] = '\0';

      s << caster;
      return s;
    }
  };


  typedef GUSLIB_EXPORT_SYMBOL guslib::Singleton <SystemClockUtil> SysClock;
}

#endif  // GUS_LIB_TIMER_H
