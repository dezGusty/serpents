#ifndef GUSLIB_SYSTEM_RANDOM_H
#define GUSLIB_SYSTEM_RANDOM_H

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
//   Last change:  $LastChangedDate: 2014-04-13 22:58:12 +0300 (D, 13 apr. 2014) $
//   Revision:    $Revision: 647 $

//
// Includes
//

//
// Platform specific definitions and overall build options for the library.
//
#include <guslib/guslibbuildopts.h>

#if GUSLIB_USE_BOOST_RANDOM
// Boost includes for random number generating functions.
#include <boost/random.hpp>
#include <boost/random/lagged_fibonacci.hpp>
#else
#include <stdlib.h>
#endif  //  GUSLIB_USE_BOOST_RANDOM

//
// This libraries' headers
//
#include <guslib/common/singleton.hpp>


namespace guslib
{
  #define GMINRANDOM 0
  #define GMAXRANDOM 10000

  /// Common interface for random numbers.
  class GUSLIB_EXPORT_SYMBOL AbstractRandomInterface
  {
  public:
    /// Destructor.
    virtual ~AbstractRandomInterface()
    {
    }

    /// Get a value between 0 and maxValue
    virtual long getValueUpTo(long maxValue) = 0;

    /// Get a value between value1 and value2. If the two values are equal, the common value is returned.
    /// It does not matter if value1 is larger than value2, or vice-versa.
    virtual long getValueBetween(long value1, long value2) = 0;
  };

#if GUSLIB_USE_BOOST_RANDOM

  // Defines for the boost generators and distributors.
  #define GGENERATORTYPE boost::mt19937
  #define GDISTRIBUTORTYPE boost::uniform_int<>
  #define GADVGENERATORTYPE boost::variate_generator<GGENERATORTYPE&, GDISTRIBUTORTYPE>

  ///  Simple class for random values.
  /// It uses the boost random libs internally, with a simple number generator and an integer number distribution.
  class GUSLIB_EXPORT_SYMBOL GBoostRandom : public AbstractRandomInterface
  {
  protected:
    // boost random stuff
    GGENERATORTYPE *generator;
    GDISTRIBUTORTYPE *distributor;
    GADVGENERATORTYPE *vGenerator;

    /// Initialization.
    virtual void initialize();

  public:
    /// constructor.
    GBoostRandom();
    virtual ~GBoostRandom();

    virtual long getValueUpTo(long maxValue);
    virtual long getValueBetween(long value1, long value2);
  };
#endif

  /// If boost is not available have a fallback random generator.
  /// Please note that this isn't really random and the results are quite non-random.
  class GUSLIB_EXPORT_SYMBOL BasicRandom : public AbstractRandomInterface
  {
  protected:
    /// Initialization.
    virtual void initialize();

  public:
    /// constructor.
    BasicRandom();

    virtual long getValueUpTo(long maxValue);
    virtual long getValueBetween(long value1, long value2);
  };

#if GUSLIB_USE_BOOST_RANDOM
  typedef GUSLIB_EXPORT_SYMBOL GBoostRandom RandomUtil;
#else
  typedef GUSLIB_EXPORT_SYMBOL BasicRandom RandomUtil;
#endif
  typedef GUSLIB_EXPORT_SYMBOL guslib::Singleton <guslib::RandomUtil> Random;
}

#endif  // GUSLIB_SYSTEM_RANDOM_H
