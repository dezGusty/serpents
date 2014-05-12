#ifndef GUSSOUND_BUILDOPTS_H
#define GUSSOUND_BUILDOPTS_H

//   This file is part of the gussound library, licensed under the terms of the MIT License.
//
//   The MIT License
//   Copyright (C) 2010-2013  Augustin Preda (thegusty999@gmail.com)
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
//   Build options.
//
//   Last change:  $LastChangedDate: 2013-06-20 22:36:33 +0200 (J, 20 iun. 2013) $
//   Revision:    $Revision: 573 $

// Version info
#include <gussound_version.hpp>

#ifdef GUSSOUND_EXP
#define GUSSOUND_EXPORT_SYMBOL __declspec (dllexport)
#define GUSSOUND_EXPIMP_TEMPLATE
#else
#ifdef GUSSOUND_IMP
#define GUSSOUND_EXPORT_SYMBOL __declspec (dllimport)
#define GUSSOUND_EXPIMP_TEMPLATE extern
#else
#define GUSSOUND_EXPORT_SYMBOL 
#define GUSSOUND_EXPIMP_TEMPLATE 
#endif
#endif // GUSSOUND_EXP


#endif // GUSSOUND_BUILDOPTS_H
