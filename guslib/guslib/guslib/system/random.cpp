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
//   Random number generating utility.
//
//   Last change:  $LastChangedDate: 2014-09-04 08:43:29 +0300 (J, 04 sep. 2014) $
//   Revision:    $Revision: 670 $

//
// Includes
//

//
// Own header
//
#include <guslib/system/random.h>

//
// C++ system includes
//

// Time functions
#include <ctime>

#if GUSLIB_USE_BOOST_RANDOM != 1
#pragma message("Info: Boost random disabled! Define GUSLIB_USE_BOOST_RANDOM as 1 to enable it (requires boost).")
#endif

//
// This libraries' headers.
//
#include "guslib/system/timer.h"


namespace guslib
{
  // Different implementation according to the used base type.

#if GUSLIB_USE_BOOST_RANDOM

  GBoostRandom::GBoostRandom()
  {
    initialize();
  }


  GBoostRandom::~GBoostRandom()
  {
    //  nothing to delete;
  }


  void GBoostRandom::initialize()
  {
    // Create a number generator. This one has a 2^19937-1 cycle length, and a decent speed.
    generator = new GGENERATORTYPE();

    // Create the distribution.
    distributor = new GDISTRIBUTORTYPE(GMINRANDOM, GMAXRANDOM);

    vGenerator = new GADVGENERATORTYPE(*generator, *distributor);

    // std::time(0) may not be the best seed, but it is enough for the needs the kind of which this class is intended.
    generator->seed(static_cast<unsigned int>(std::time(0)));
  }


  long GBoostRandom::getValueUpTo(long maxValue)
  {
    if (maxValue == 0)
    {
      return 0;
    }

    return vGenerator->operator()() % maxValue;
  }

  long GBoostRandom::getValueBetween(long value1, long value2)
  {
    if (value1 == value2)
    {
      return value1;
    }

    long min, max;
    if (value1 > value2)
    {
      min = value2;
      max = value1;
    }
    else
    {
      min = value1;
      max = value2;
    }

    long interval = max - min + 1;
    return getValueUpTo(interval) + min;
  }

#else   //  GUSLIB_USE_BOOST_RANDOM

  long BasicRandom::getValueUpTo(long maxValue)
  {
    if (maxValue <= 0)
    {
      return 0;
    }

    return rand() % maxValue;
  }

  long BasicRandom::getValueBetween(long value1, long value2)
  {
    if (value1 == value2)
    {
      return value1;
    }

    long min, max;
    if (value1 > value2)
    {
      min = value2;
      max = value1;
    }
    else
    {
      min = value1;
      max = value2;
    }

    long interval = max-min + 1;
    return getValueUpTo(interval) + min;
  }


  BasicRandom::BasicRandom()
  {
    initialize();
  }

  void BasicRandom::initialize()
  {
    Timer timer;
    unsigned seed = (unsigned)timer.getCurrentTimeUnits();
    srand(seed);
  }
#endif  //  GUSLIB_USE_BOOST_RANDOM

#if GUSLIB_FLAG_SINGLETONINST == 0
  // If the singleton is not set to create instances.
  guslib::RandomUtil * guslib::Singleton <guslib::RandomUtil>::objectPtr_ = nullptr;
#endif
}
