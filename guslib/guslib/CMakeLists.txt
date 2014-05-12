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
SET (GUSLIB_PROJECT_NAME guslib)
PROJECT (${GUSLIB_PROJECT_NAME})

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../../cmake/;${CMAKE_MODULE_PATH}")
SET(GUSLIB_LIB_TYPE SHARED)

# Include a small macro file.
INCLUDE (GusLibUtils)

# Write some messages to the log.
MESSAGE (STATUS "... building guslib ...")
MESSAGE (STATUS "")

# Specify the preferred build type to be unicode. It's strange, but CMake seems to default to Multibyte. At least on Windows machines.
# Specify that the GUSLIB_EXP symbol is defined. Also define the _USRDLL symbol, needed for regular DLLs.
# Regular as in "not an Extension DLL". Gee, thanks a lot MFC!
ADD_DEFINITIONS(-DUNICODE -D_UNICODE -DGUSLIB_EXP -D_USRDLL)
REMOVE_DEFINITIONS(-DGUSLIB_IMP)

# ADD_DEFINITIONS(-D_MT)
 

# 
# Specify some user configurable options
# 
# OPTION(GUSLIB_FLAG_MULTITHREAD_SUPPORT "Use a limited support for multithreading (mainly by adding some mutexes to the singleton class)" ON)
OPTION(GUSLIB_FLAG_SINGLETONINSTANCE "When using the singleton, if this option is NOT enabled, you need to instantiate the singleton yourself " ON)
OPTION(GUSLIB_FLAG_USE_BOOST_SMART_POINTERS "Link to the boost smart (shared) pointers" OFF)
OPTION(GUSLIB_FLAG_USE_YASPER_SMART_POINTER "Link to the yasper smart (shared) pointers" ON)
OPTION(GUSLIB_FLAG_USE_BOOST_RANDOM "Use the boost random number generator instead of the basic implementation" OFF)
OPTION(GUSLIB_THREAD_SUPPORT "Specify whether to use threading support or not." ON)
OPTION(GUSLIB_USE_OGRE_TIMERS "Specify whether to use OGRE timers instead of custom timers." OFF)

#
# Identify the threading options
#

# Check to see whether we use boost threads or not. Will be used for linking.
SET (GUSLIB_FLAG_USE_BOOST_THREADS OFF)
SET (GUSLIB_THREAD_PROVIDER_RECOGNIZED 0)
SET (GUSLIB_THREAD_PROVIDER_NUM 0)
SET(GUSLIB_THREAD_DEFAULT_PROVIDER "tthread")
SET(GUSLIB_THREAD_PROVIDER ${GUSLIB_THREAD_DEFAULT_PROVIDER} CACHE STRING
	"Select the library to use for thread support. Possible values:
  off     - no threading support
	C11     - C++ 11 threads
	tthread - TinyThread++ library
	boost   - Boost thread library."
)

# Verify C++11 scenario
IF ("${GUSLIB_THREAD_PROVIDER}" STREQUAL "C11")
  SET (GUSLIB_THREAD_PROVIDER_RECOGNIZED 1)
  SET (GUSLIB_THREAD_PROVIDER_NUM 1)
ENDIF ()

# Verify TinyThread++ scenario
IF ("${GUSLIB_THREAD_PROVIDER}" STREQUAL "tthread")
  SET (GUSLIB_THREAD_PROVIDER_RECOGNIZED 1)
  SET (GUSLIB_THREAD_PROVIDER_NUM 2)
ENDIF ()

# Verify boost scenario
IF ("${GUSLIB_THREAD_PROVIDER}" STREQUAL "boost")
  SET (GUSLIB_THREAD_PROVIDER_RECOGNIZED 1)
  SET (GUSLIB_THREAD_PROVIDER_NUM 3)
  SET (GUSLIB_FLAG_USE_BOOST_THREADS ON)
ENDIF ()

# Perform sanity check.
IF ("${GUSLIB_THREAD_PROVIDER_RECOGNIZED}" EQUAL "0")
  MESSAGE ("Could not recognize thread provider. Disabling thread support")
  SET (GUSLIB_THREAD_SUPPORT OFF)
ENDIF ()


SET (list_of_boost_libs_to_link "")
GUSLIB_ADD_BOOST_DEPS(list_of_boost_libs_to_link GUSLIB_FLAG_USE_BOOST_THREADS GUSLIB_FLAG_USE_BOOST_SMART_POINTERS GUSLIB_FLAG_USE_BOOST_RANDOM)


#
# -------------------------------- Configure header with options specified by the user ---------------------------------
#

# Call the function to create binary (0/1) variables from options.
# Each variable name shall have the name of the option with the _NUM suffix appended to it.
# E.g. For option "GUSLIB_FLAG_USE_BOOST_THREADS" => variable "GUSLIB_FLAG_USE_BOOST_THREADS_NUM"
CREATE_BINARY_VAR_FROM_OPTION (GUSLIB_FLAG_USE_BOOST_THREADS)
CREATE_BINARY_VAR_FROM_OPTION (GUSLIB_FLAG_SINGLETONINSTANCE)
CREATE_BINARY_VAR_FROM_OPTION (GUSLIB_FLAG_USE_BOOST_SMART_POINTERS)
CREATE_BINARY_VAR_FROM_OPTION (GUSLIB_FLAG_USE_YASPER_SMART_POINTER)
CREATE_BINARY_VAR_FROM_OPTION (GUSLIB_FLAG_USE_BOOST_RANDOM)
CREATE_BINARY_VAR_FROM_OPTION (GUSLIB_THREAD_SUPPORT)
CREATE_BINARY_VAR_FROM_OPTION (GUSLIB_USE_OGRE_TIMERS)

MESSAGE (STATUS "GUSLIB_FLAG_USE_BOOST_THREADS_NUM = ${GUSLIB_FLAG_MULTITHREAD_SUPPORT_NUM}")

# Configure the file (search and replace variable names).
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/guslib/guslibbuildopts.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/guslib/guslibbuildopts.h")

# 
# -------------------------------------------------- File section ------------------------------------------------------
#

# Just keep this for future references
SET (SOURCE_FILES_COMMON
  ./guslib/common/abstractfactory.hpp
  ./guslib/common/simpleexception.cpp
  ./guslib/common/simpleexception.h
  ./guslib/common/singleton.hpp
  ./guslib/common/singletonholder.hpp
  ./guslib/common/smartptr.h
)

SET (SOURCE_FILES_SYSTEM
  ./guslib/system/apprestart.h
  ./guslib/system/commandline.h
  ./guslib/system/random.cpp
  ./guslib/system/random.h
  ./guslib/system/systemmutex.cpp
  ./guslib/system/systemmutex.h
  ./guslib/system/thread.h
  ./guslib/system/threadingdefsboost.h
  ./guslib/system/threadingdefstthread.h
  ./guslib/system/timer.cpp
  ./guslib/system/timer.h
  ./guslib/system/commandline.cpp
)

SET (SOURCE_FILES_THIRDPARTY
  ./guslib/thirdparty/fast_mutex.h
  ./guslib/thirdparty/SimpleIni.h
  ./guslib/thirdparty/tinystr.cpp
  ./guslib/thirdparty/tinystr.h
  ./guslib/thirdparty/tinythread.cpp
  ./guslib/thirdparty/tinythread.h
  ./guslib/thirdparty/tinyxml.cpp
  ./guslib/thirdparty/tinyxml.h
  ./guslib/thirdparty/tinyxmlerror.cpp
  ./guslib/thirdparty/tinyxmlparser.cpp
  ./guslib/thirdparty/yasper.h
)

SET (SOURCE_FILES_TRACE
  ./guslib/trace/trace.cpp
  ./guslib/trace/trace.h
)

SET (SOURCE_FILES_UTIL
  ./guslib/util/config.cpp
  ./guslib/util/config.h
  ./guslib/util/configuration.cpp
  ./guslib/util/configuration.h
  ./guslib/util/consolemenu.cpp
  ./guslib/util/consolemenu.h
  ./guslib/util/filehelper.cpp
  ./guslib/util/filehelper.h
  ./guslib/util/iniconfig.cpp
  ./guslib/util/stringutil.cpp
  ./guslib/util/iniconfig.h
  ./guslib/util/inifile.h
  ./guslib/util/stringutil.h
)

SET (SOURCE_FILES_BASE_FOLDER
  ./guslib/guslib_version.hpp
  ./guslib/guslibbuildopts.h
  ./guslib/guslib_platform.h
)


SET (SOURCE_FILES
  ${SOURCE_FILES_COMMON}
  ${SOURCE_FILES_SYSTEM}
  ${SOURCE_FILES_THIRDPARTY}
  ${SOURCE_FILES_TRACE}
  ${SOURCE_FILES_UTIL}
  ${SOURCE_FILES_BASE_FOLDER}
)

# If building for windows based systems, the resource file (containing 
# the manifest, icon and other resources) should also be included
IF (WIN32)
  SET (SOURCE_FILES_RESOURCE_FOLDER
    ./guslib/resource/guslib.rc
  )
  
  SET (SOURCE_FILES
    ${SOURCE_FILES}
    ${SOURCE_FILES_RESOURCE_FOLDER}
  )
ENDIF()
  

# 
# Group the files into file tabs (specific to Visual Studio)
# The files must be already defined in the header or source list.
#

# The common group
SOURCE_GROUP("guslib\\common" FILES
  ${SOURCE_FILES_COMMON}
)

SOURCE_GROUP("guslib\\system" FILES
  ${SOURCE_FILES_SYSTEM}
)

SOURCE_GROUP("guslib\\thirdparty" FILES
  ${SOURCE_FILES_THIRDPARTY}
)

SOURCE_GROUP("guslib\\trace" FILES
  ${SOURCE_FILES_TRACE}
)

SOURCE_GROUP("guslib\\util" FILES
  ${SOURCE_FILES_UTIL}
)

IF (WIN32)
  SOURCE_GROUP("guslib\\resource" FILES
    ${SOURCE_FILES_RESOURCE_FOLDER}
  )
ENDIF()

# The parent folder 
SOURCE_GROUP("guslib" FILES
  ${SOURCE_FILES_BASE_FOLDER}
)


#
# Additional include directories.
# Add the current folder (CMAKE_CURRENT_SOURCE_DIR)
# If you don't build into the same directory, also add CMAKE_CURRENT_BINARY_DIR.
#
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

#
# Specify the library to be built from the source files.
#
add_library(${GUSLIB_PROJECT_NAME}
    ${GUSLIB_LIB_TYPE}
    ${SOURCE_FILES})

# TODO: define link options (static/non-static) and dll-export symbols (GUSLIB_EXP)/GUSLIB_IMP definitions.
# set_target_properties(${GUSLIB_PROJECT_NAME}
#     PROPERTIES
#     COMPILE_DEFINITIONS _LIB
#     COMPILE_DEFINITIONS GUSLIB_LIB_BUILD
# )

  
#
# Link to dependencies
#
IF (Boost_FOUND)
  TARGET_LINK_LIBRARIES(${GUSLIB_PROJECT_NAME} ${list_of_boost_libs_to_link})
ENDIF ()
 

# if (APPLE AND NOT OGRE_BUILD_PLATFORM_APPLE_IOS)
#     # Set the INSTALL_PATH so that Components can be installed in the application package
#     set_target_properties(guslib
#        PROPERTIES BUILD_WITH_INSTALL_RPATH 1
#        INSTALL_NAME_DIR "@executable_path/../Components"
#     )
# 
# 	# Copy headers into the main Ogre framework
# 	add_custom_command(TARGET OgreRTShaderSystem POST_BUILD
# 	  COMMAND ditto ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h ${OGRE_BINARY_DIR}/lib/$(CONFIGURATION)/Ogre.framework/Headers/
# 	)
# endif()


install(FILES ${SOURCE_FILES}
  DESTINATION include/guslib
)

#
# Set the default location of the output executable file.
#
IF (WIN32)
    IF (CMAKE_BUILD_TYPE STREQUAL "Release")
      SET (GUSLIB_DLL_OUTPUT_DIR "../../Release")
    ELSE ()
      IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
        SET (GUSLIB_DLL_OUTPUT_DIR "../../Debug")
      ELSE ()
        SET (GUSLIB_DLL_OUTPUT_DIR "../../")
      ENDIF ()
    ENDIF ()
    SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GUSLIB_DLL_OUTPUT_DIR})
ELSE()
    SET (GUSLIB_DLL_OUTPUT_DIR "bin")
ENDIF()

SET_TARGET_PROPERTIES (${GUSLIB_PROJECT_NAME}
    PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY ${GUSLIB_DLL_OUTPUT_DIR}
)

# install the build binary
install(TARGETS ${GUSLIB_PROJECT_NAME}
  RUNTIME DESTINATION "${GUSLIB_DLL_OUTPUT_DIR}")

MESSAGE (STATUS "Set runtime dest of ${GUSLIB_PROJECT_NAME} to : ${GUSLIB_DLL_OUTPUT_DIR}")
