Introduction
~~~~~~~~~~~~~
Guslib is a small C++ utility library. It includes other small libraries to provide basic functionality for:
- handling XML files
- handling INI files 
- logging/tracing
- smart pointer definitions
- clock and timer

3rd party libraries
~~~~~~~~~~~~~~~~~~~~
- SimpleIni (http://code.jellycan.com/simpleini/) 
      - version: 4.14
      - license: MIT 
- TinyXML (http://www.grinninglizard.com/tinyxml/)
      - version: 2.6.2
      - license: zlib/libpng - compatible
- Yasper (http://yasper.sourceforge.net/)
      - version: 1.0.4
      - license: zlib/libpng

License
~~~~~~~~
Guslib is licensed under the terms of the MIT License.
 
The MIT License
Copyright (C) 2010-2012  Augustin Preda (thegusty999@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

Author(s)
~~~~~~~~~~
Augustin Preda


Using the library
~~~~~~~~~~~~~~~~~~
The library can be used either in a project:
- by including the code directly into the project
- by linking the library statically into the project. One needs to generate the library (or obtain a generated copy), 
then to include the required headers in the project and also link to the library (.lib) file.
      (see the example project [directGusLibTest]. 
       Note: the provided example projects also require the boost library.
       $(BOOST_ROOT)\lib\ needs to be present in the additional directories to load the libraries from.
       The environment variable BOOST_ROOT must also be defined.)

If you want to create your own DLL that exports symbols from the library, have a look at the example projects:
    - guslibTestDLL (creates a DLL) 
    - guslibtest (uses the DLL created by the previous project).

Version history
~~~~~~~~~~~~~~~~
v.1.0.0 - 2012.01.28 : added version information.
v.1.0.1 - 2012.01.29 : attempted unicode addition via wstring and basic STL functionality. So far, unsuccessful.
v.1.1.0 - 2013.05.07 : moved version information to separate file, and formatted similar to boost. Should be easier to
        be used by tools such as CMake. Additional conversion options added to the string utility class.
        
        
