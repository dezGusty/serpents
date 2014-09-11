#ifndef GUS_LIB_TRACE_H
#define GUS_LIB_TRACE_H

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
// Platform specific definitions and overall build options for the library.
//
#include <guslib/guslibbuildopts.h>

//
// C++ system headers
//

// output stream
#include <ostream>

//
// This libraries' headers
//

#if GUSLIB_THREAD_SUPPORT == 1
// Use thread mutexes if required
#include <thread>
#endif

// Use the phoenix singleton implementation. This is a useful singleton pattern for logging.
#include "guslib/common/singleton.hpp"

// Default to using line in trace file (can be overridden)
#ifndef GUS_USE_LINE_IN_TRACE
#define GUS_USE_LINE_IN_TRACE 1
#endif

#ifndef GUS_USE_TIMER_IN_TRACE
#define GUS_USE_TIMER_IN_TRACE 1
#endif

#if GUS_USE_TIMER_IN_TRACE
#include "guslib/system/Timer.h"
#endif

// Note: you may want to define the additional MACROS:
// GUSLIB_FLAG_MULTITHREAD - will add multithreaded synchronization
// for creating the Singleton and for writing to the stream.
// This will require the inclusion of GMutex.h

namespace guslib
{
#ifndef GUSLIB_NOTRACE

#pragma warning(push)
#pragma warning(disable:4251)

  enum class TraceEncoding
  {
    ASCII = 0,
    UTF_8 = 1
  };

  /// @brief
  /// Trace logging utility class.
  /// @details
  ///  To disable the tracing class (TraceUtil) being built into a project, the GUSLIB_NOTRACE macro must be defined.
  /// The TraceUtil class should be thread safe.
  /// TraceUtil should be made to follow the singleton design pattern.
  /// @note Please consider using the utility macros instead of accessing the functions directly, since it
  /// will be much more convenient.
  /// @note The current implementation ignores caching, so the flush operation is pointless.
  /// @page Tracer
  /// some trace page
  ///
  /// Usage example, through the use of macros
  /// @code
  /// std::string myFileName = "c:/temp/test.txt";
  /// int traceLevel = 4; // Level of tracing
  /// GSTARTTRACING(myFileName, traceLevel);
  /// GTRACE(1, "The trace started successfully");
  /// GTRACE(5, "This message is not written, because it's sent with level 5, but the trace level is 4");
  /// GDISABLETRACE; // the trace was disabled.
  /// GTRACE(1, "this will also not be written, because the tracing is disabled);
  /// GENABLETRACE(); // trace was re-enabled.
  /// GTRACE(4, "This message is written.");
  /// GSETTRACELEVEL(3); // the trace level was changed.
  /// GTRACE(4, "This message is not written, because it's sent with level 4, but the trace level is 3");");
  /// GTRACE(3, "It is possible to "<<"group"<<" more texts using the << operator.");
  /// GTRACE(-1, "It's possible to use negative levels also for even higher priority.");
  /// GSTOPTRACING(); // The trace object static instance is destroyed; file handle closed.
  /// // The trace object is cleanly removed when the program exits, so this is optional.
  /// @endcode
  class GUSLIB_EXPORT_SYMBOL TraceUtil
  {
  private:
    bool enabled_;
    int level_;
    TraceEncoding encoding_;  // the encoding that is set by the user, according to the preferrence.
    TraceEncoding encodingUsed_;  //  the encoding that was used when opening.

    class Impl;
    Impl* impl_;

    ///
    ///  DISABLED. Copy constructor.
    ///
    TraceUtil(const TraceUtil& rhs) = delete;

    ///
    /// DISABLED. Assignment operator.
    ///
    TraceUtil& operator=(const TraceUtil&) = delete;

  protected:
    bool fileIsOpened;

    ///
    /// Close the file handle. Override this if you define a child class with a different stream.
    ///
    virtual void closeFileHandle();

    ///
    /// Open the file handle. Override this if you define a child class with a different stream.
    ///
    virtual void openFileHandle();

#if GUSLIB_THREAD_SUPPORT == 1
    /// mutex used in multithreaded trace access.
    std::recursive_mutex writeMutex;
#endif

    ///
    /// The constructor.
    ///
    TraceUtil();

  public:
    ///
    /// Destructor; simply close the file handle.
    ///
    virtual ~TraceUtil();

    virtual std::ostream& operator<<(std::streambuf * ss);

    ///
    /// Get the encoding that was selected upon file open
    ///
    virtual TraceEncoding getEncodingUsed() const
    {
      return encodingUsed_;
    }

    ///
    /// Get the encoding that was selected upon file open
    ///
    virtual TraceEncoding getEncodingPreferred() const
    {
      return encoding_;
    }

    ///
    /// Direct access to the output stream. This can be much quicker than
    /// using the writeLine function, if you need to pass something other
    /// than plain text.
    ///
    virtual std::ostream& getOutputStream();

#if GUSLIB_THREAD_SUPPORT == 1
    /// mutex used in multithreaded trace access.
    virtual std::recursive_mutex& getWriteMutex()
    {
      return writeMutex;
    }
#endif

    ///
    /// Getter for enabled/disabled status
    ///
    virtual bool inline isEnabled() const
    {
      return enabled_;
    }

    ///
    /// Specify whether a trace with the given level can be written.
    ///
    virtual bool inline canTraceWithLevel(int level)
    {
      return enabled_ && level <= level_;
    }

    ///
    /// Getter for the trace level.
    ///
    virtual int inline getLevel() const
    {
      return level_;
    }

    ///
    /// Initialization with the file name to use.
    ///
    virtual void initialize(const char* fileNameToUse);

    ///
    /// Enable or disable the logging alltogether.
    ///
    virtual void setEnabled(bool value = true)
    {
      enabled_ = value;
    }


    ///
    /// Set the encoding to use. Only has effect for files opened after this operation.
    ///
    virtual void setEncoding(TraceEncoding newEncoding)
    {
      encoding_ = newEncoding;
    }


    ///
    /// Setter for the filename. This also opens the file for writing.
    ///
    virtual void setFileName(const char* fileNameToUse);


    ///
    /// Setter for the tracing level.
    ///
    virtual void setLevel(int lvl)
    {
      level_ = lvl;
    }


    ///
    /// Add a line to write into the log file (using a string).
    /// Writes a line to the file. The file is expected to be opened; the text is expected to be non-empty.
    ///
    virtual void inline writeLine(const char* textToWrite);

    // Allow the singleton template to have access to the constructor.
    friend class guslib::Singleton <TraceUtil>;
  };


  // Redefine the singleton; makes it easier to write code.
  typedef guslib::Singleton <guslib::TraceUtil> Tracer;


#if GUSLIB_FLAG_SINGLETONINST == 0
  // If the singleton is not set to create instances.
  guslib::TraceUtil * guslib::Singleton <guslib::TraceUtil>::objectPtr_ = nullptr;
#endif


  // Also explicitly instantiate the template class.
  // TODO(Augustin Preda, 2015.04.13): this may be only microsoft specific
  // so make it at least connected to a windows environment var.
#ifndef GUSLIB_EXPLICIT_INST_TRACE
#define GUSLIB_EXPLICIT_INST_TRACE
  GUSLIB_EXPIMP_TEMPLATE template class GUSLIB_EXPORT_SYMBOL guslib::Singleton <guslib::TraceUtil>;
#endif  // GUSLIB_EXPLICIT_INST_TRACE

#pragma warning(pop)

#endif  // GUSLIB_NOTRACE

#ifndef GUSLIB_NOTRACE

#if GUS_USE_LINE_IN_TRACE
#if GUS_USE_TIMER_IN_TRACE
#define GUSLIB_TRACE_TEXT(level, text) level << "\t|\t" << guslib::SysClock::get() << "\t|\t" << text << "\t\t(" << __FILE__ << ", line " << __LINE__ << ")" << std::endl
#else
#define GUSLIB_TRACE_TEXT(level, text) level << "\t|\t" << text << "\t\t(" << __FILE__ << ", line " << __LINE__ << ")" << std::endl
#endif  // GUS_USE_TIMER_IN_TRACE
#else  // no code line in trace
#if GUS_USE_TIMER_IN_TRACE
#define GUSLIB_TRACE_TEXT(level, text) level << "\t|\t" <<guslib::SysClock::get() << "\t|\t" << text << std::endl;
#else
#define GUSLIB_TRACE_TEXT(level, text) level << "\t|\t" << text << "\n"
#endif
#endif  // GUS_USE_LINE_IN_TRACE

#if GUSLIB_THREAD_SUPPORT == 1
#define GTRACE(level, text)\
  if (guslib::Tracer::getPtr()->canTraceWithLevel(level))\
  {\
    std::lock_guard<std::recursive_mutex> lg{guslib::Tracer::getPtr()->getWriteMutex()};\
    guslib::Tracer::getPtr()->getOutputStream() << GUSLIB_TRACE_TEXT(level, text);\
  }
#else
#define GTRACE(level, text)\
  if (guslib::Tracer::getPtr()->canTraceWithLevel(level))\
  {\
    guslib::Tracer::getPtr()->getOutputStream() << GUSLIB_TRACE_TEXT(level, text);\
  }
#endif

#define GSTARTTRACING(file, level) \
  guslib::Tracer::getPtr()->setFileName(file);\
  guslib::Tracer::getPtr()->setLevel(level);\
  guslib::Tracer::getPtr()->setEnabled();


// call this before quiting the app.
#define GSTOPTRACING() guslib::Tracer::destroy()

#define GDISABLETRACE() guslib::Tracer::getPtr()->setEnabled(false)

#define GENABLETRACE() guslib::Tracer::getPtr()->setEnabled(true)

#define GSETTRACELEVEL(level) guslib::Tracer::getPtr()->setLevel(level)

#else  // GUSLIB_NOTRACE
#define GTRACE(level, text)
#define GSTARTTRACING(file, level)
#define GDISABLETRACE()
#define GENABLETRACE()
#define GSETTRACELEVEL(level)
#define GSTOPTRACING()
#endif    // GUSLIB_NOTRACE
}  // namespace guslib

#endif  // GUS_LIB_TRACE_H
