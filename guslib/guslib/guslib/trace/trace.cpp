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
//  Last change:  $LastChangedDate: 2014-06-27 22:54:16 +0300 (V, 27 iun. 2014) $
//  Revision:    $Revision: 657 $

//
// Includes
//

//
// Own header
//
#include <guslib/trace/trace.h>

//
// C++ system headers.
//
#include <fstream>
#include <string>

#ifndef GUSLIB_FLAG_MULTITHREAD
#pragma message("Warning: Multithreaded synchronization disabled! Define GUSLIB_FLAG_MULTITHREAD to enable it (requires boost libs).")
#endif  // GUSLIB_FLAG_MULTITHREAD

// ------------------- Implementation ------------------------

#ifndef GUSLIB_NOTRACE
namespace guslib
{
#ifndef GUSLIB_EXPLICIT_INST_TRACE
#define GUSLIB_EXPLICIT_INST_TRACE
  GUSLIB_EXPIMP_TEMPLATE template class GUSLIB_EXPORT_SYMBOL guslib::Singleton <guslib::TraceUtil>;
#endif  // GUSLIB_EXPLICIT_INST_TRACE

  /**
    Opaque pointer: implementation of trace utility functions.
  */
  class TraceUtil::Impl
  {
  public:
    std::string fileName_;
    std::ofstream of_;  // the output filestream; used for file operations.
    std::ostream customOut_;  // the output stream; used for write operations.

    Impl():
      fileName_(""),
      of_(std::ofstream("")),
      customOut_(NULL)
    {
    }
  };

  ///
  /// The constructor.
  ///
  TraceUtil::TraceUtil()
    : impl_(new TraceUtil::Impl()),
      fileIsOpened(false),
      level_(10),
      enabled_(true),
      encoding_(ASCII),
      encodingUsed_(ASCII)
  {
  }


  ///
  /// Destructor; simply close the file handle.
  ///
  TraceUtil::~TraceUtil()
  {
    closeFileHandle();
    delete impl_;
  }

  std::ostream& TraceUtil::operator<<(std::streambuf * ss)
  {
    impl_->customOut_ << ss;
    return impl_->customOut_;
  }


  ///
  /// Direct access to the output stream. This can be much quicker than
  /// using the writeLine function, if you need to pass something other
  /// than plain text.
  ///
  std::ostream& TraceUtil::getOutputStream()
  {
    return impl_->customOut_;
  }

  ///
  /// Close the file handle. Override this if you define a child class with a different stream.
  ///
  void TraceUtil::closeFileHandle()
  {
    // Close the file stream.
    impl_->of_.close();
    fileIsOpened = false;
  }

  ///
  /// Initialization with the file name to use.
  ///
  void TraceUtil::initialize(const char* fileNameToUse)
  {
    impl_->fileName_ = fileNameToUse;

    // Open the file, deleting its old contents
    openFileHandle();
  }

  void TraceUtil::openFileHandle()
  {
    try
    {
      // Open the file stream
      impl_->of_.open(impl_->fileName_.c_str());

      // Store the file stream's buffer in the output stream
      impl_->customOut_.rdbuf(impl_->of_.rdbuf());

      encodingUsed_ = encoding_;

      // Keep track of the opened status.
      fileIsOpened = true;
    }
    catch (std::exception &)
    {
      fileIsOpened = false;
    }
  }

  void TraceUtil::setFileName(const char* fileNameToUse)
  {
    std::string temporaryString(fileNameToUse);
    if (temporaryString.length() <= 0)
    {
      return;
    }

    // Close the existing file handle if one is already opened.
    closeFileHandle();

    // Open or reopen the file handle.
    impl_->fileName_ = temporaryString;
    openFileHandle();
  }

  ///
  /// Add a line to write into the log file (using a string).
  /// Writes a line to the file. The file is expected to be opened; the text is expected to be non-empty.
  ///
  void inline TraceUtil::writeLine(const char* textToWrite)
  {
#if GUSLIB_THREAD_SUPPORT == 1
    GUS_LOCK_MUTEX(writeMutex);
#endif
    if (false == fileIsOpened)
    {
      return;
    }

    if (false == isEnabled())
    {
      return;
    }

    impl_->customOut_ << textToWrite << std::endl;
  }

}   //  namespace guslib

#endif  // GUSLIB_NOTRACE
