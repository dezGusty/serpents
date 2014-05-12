#-------------------------------------------------------------------
# This file is part of the CMake build system for the guslib library
#-------------------------------------------------------------------
# The MIT License
# Copyright (C) 2010-2013  Augustin Preda (thegusty999@gmail.com)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#-------------------------------------------------------------------

cmake_minimum_required(VERSION 2.6)
SET (guslibtestexe_project_name guslibtest)
PROJECT (${guslibtestexe_project_name})

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../../cmake/;${CMAKE_MODULE_PATH}")

MESSAGE(STATUS "Building ${guslibtestexe_project_name}")
MESSAGE(STATUS "")

Set (guslibtestexe_project_DEPENDENCIES guslib guslibTestDLL)


SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/;${CMAKE_MODULE_PATH}")
SET(CMAKE_DEBUG_POSTFIX "_d")

# Specify that the GUSLIB_IMP symbol is defined, importing the items from the guslib.dll
ADD_DEFINITIONS(-DUNICODE -D_UNICODE -DGUSLIB_IMP)

SET (list_of_boost_libs_to_link "")
# Add the boost dependencies (this adds the both the include directories and the library linkage).
GUSLIB_ADD_BOOST_DEPS(list_of_boost_libs_to_link GUSLIB_FLAG_MULTITHREAD_SUPPORT GUSLIB_FLAG_USE_BOOST_SMART_POINTERS GUSLIB_FLAG_USE_BOOST_RANDOM)
MESSAGE (STATUS "Should link to: ${list_of_boost_libs_to_link}")


# More include directories.
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../guslibTestDLL/code)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../guslib)

SET(SRCS
  ./main.cpp
)
 

ADD_EXECUTABLE(${guslibtestexe_project_name} 
    ${guslibtest_project_type} 
    ${SRCS})

#
# Link to dependencies
#
IF (Boost_FOUND)
  SET(guslibtestexe_project_DEPENDENCIES 
        ${guslibtestexe_project_DEPENDENCIES} 
        ${list_of_boost_libs_to_link}
        )
ENDIF ()

TARGET_LINK_LIBRARIES(${guslibtestexe_project_name} 
      ${guslibtestexe_project_DEPENDENCIES} 
      )

#
# Set the default location of the output executable file.
#
IF (WIN32)
    IF (CMAKE_BUILD_TYPE STREQUAL "Release")
      SET (guslibtestexe_out_dir "../../Release")
    ELSE ()
      IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
        SET (guslibtestexe_out_dir "../../Debug")
      ELSE ()
        SET (guslibtestexe_out_dir "../../")
      ENDIF ()
    ENDIF ()
ELSE()
    SET (guslibtestexe_out_dir "bin")
ENDIF()

SET_TARGET_PROPERTIES (${guslibtestexe_project_name} 
    PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY ${guslibtestexe_out_dir}
)

# only CPack code below this line
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "guslib test")
SET(CPACK_PACKAGE_VENDOR "Augustin Preda")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_VERSION_MAJOR "${SERPENTS_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${SERPENTS_VERSION_MINOR_3DIGITS}")
SET(CPACK_PACKAGE_VERSION_PATCH "${SERPENTS_VERSION_PATCH_2DIGITS}")

SET(CPACK_SOURCE_GENERATOR TBZ2 ZIP)
SET(CPACK_GENERATOR NSIS)

set(CPACK_SOURCE_PACKAGE_FILE_NAME "guslibDLLtest" CACHE INTERNAL "tarball basename")

set(CPACK_SOURCE_IGNORE_FILES
 # repository history should not be in source tarballs
"\\\\.hg.*"
"\\\\.svn.*"

# the rest is modelled after .hgignore
"build/"
"~$"

"\\\\.project"
"\\\\.cproject"
"\\\\.pydevproject"

"serpents$"
"serpents.exe$"

"\\\\.serpents"
)

# INCLUDE(CPack)
