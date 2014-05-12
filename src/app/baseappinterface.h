#pragma once

//   This file is part of Gusty's Serpents, licensed under the terms of the MIT License
//   (further described below).
//
//   Copyright (C) 2009-2014  Augustin Preda (thegusty999@gmail.com)
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
//
//    Application base class, supporting restart.
//
//    Last change:  $LastChangedDate: 2014-04-26 23:40:46 +0300 (S, 26 apr. 2014) $
//    Revision:    $Revision: 650 $

namespace app
{
  /**
    A base class for an application.
    Supports application restarting and will handle it transparently. Some customizing options can be defined
    in the header "RestartOpts.h". Alternatively, the symbols defined there can be predefined as preprocessor
    options for the project.
  */
  class BaseAppInterface
  {
  public:
    /**
      The constructor will also take care of the finishing steps for any application restarts.
      Please ensure that this is called from the derived class.
    */
    BaseAppInterface();

    /**
      Destructor. Also takes care of necessary steps for the restart of the application.
    */
    virtual ~BaseAppInterface();

    /**
      Main function. Must implement in inheriting classes.
    */
    virtual void run() = 0;
  };
}
