#-------------------------------------------------------------------
# This file is part of the CMake build system for the gussound library
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
SET (gussound_project_name gussound)
PROJECT (${gussound_project_name})

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../cmake/;${CMAKE_MODULE_PATH}")
SET(CMAKE_DEBUG_POSTFIX "_d")

SET(gussound_project_type SHARED)

# 
# Specify some user configurable options
# 
OPTION (GUSSOUND_FLAG_USE_AUDIERE "Build the gussound Audiere linkage" ON)

# Include a small macro file.
INCLUDE (GusLibUtils)

MESSAGE(STATUS "Building ${gussound_project_name}")
MESSAGE(STATUS "")

MESSAGE (STATUS "Will use cmake modules in: ${CMAKE_MODULE_PATH}")

Set (gussound_project_DEPENDENCIES guslib)


# Specify that the GUSLIB_IMP symbol is defined, importing the items from the guslib.dll
ADD_DEFINITIONS(-DUNICODE -D_UNICODE -DGUSLIBTESTDLL_EXPORTS -DGUSLIB_EXP -DGUSSOUND_EXP -D_USRDLL)

SET (list_of_boost_libs_to_link "")
# Add the boost dependencies (this adds the both the include directories and the library linkage).
GUSLIB_ADD_BOOST_DEPS(list_of_boost_libs_to_link GUSLIB_FLAG_MULTITHREAD_SUPPORT GUSLIB_FLAG_USE_BOOST_SMART_POINTERS GUSLIB_FLAG_USE_BOOST_RANDOM)
MESSAGE (STATUS "Should link to: ${list_of_boost_libs_to_link}")


#
# ------------------------------------------------- Package discovery --------------------------------------------------
#

FIND_PACKAGE(OpenAL REQUIRED)
FIND_PACKAGE(ALUT REQUIRED)
FIND_PACKAGE(Ogg REQUIRED)
FIND_PACKAGE(Vorbis REQUIRED)

IF (GUSSOUND_FLAG_USE_AUDIERE)
  FIND_PACKAGE(Audiere REQUIRED)
ENDIF ()


# 
# -------------------------------------------------- File section ------------------------------------------------------
#

SET(SOURCE_FILES
  ./include/audiere.h
  ./include/gaudiere.h
  ./include/goggstreamedsound.h
  ./include/gopenal.h
  ./include/gussound.h
  ./include/gussound_version.hpp
  ./include/gussoundbuildopts.h
  ./src/gaudiere.cpp
  ./src/goggstreamedsound.cpp
  ./src/gopenal.cpp
  ./src/gussound.cpp
)

# If building for windows based systems, the resource file (containing 
# the manifest, icon and other resources) should also be included
IF (WIN32)
  SET (SOURCE_FILES_RESOURCE_FOLDER
    ./include/gussound/resource/gussound.rc
  )
  
  SET (SOURCE_FILES
    ${SOURCE_FILES}
    ${SOURCE_FILES_RESOURCE_FOLDER}
  )
ENDIF()


SOURCE_GROUP("src" FILES
  ./src/gaudiere.cpp
  ./src/goggstreamedsound.cpp
  ./src/gopenal.cpp
  ./src/gussound.cpp
) 

SOURCE_GROUP("include" FILES
  ./include/audiere.h
  ./include/gaudiere.h
  ./include/goggstreamedsound.h
  ./include/gopenal.h
  ./include/gussound.h
) 

IF (WIN32)
  SOURCE_GROUP("resource" FILES
    ${SOURCE_FILES_RESOURCE_FOLDER}
  )
ENDIF()


# More include directories.
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include/al)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../guslib/guslib)


add_library(${gussound_project_name} 
    ${gussound_project_type} 
    ${SOURCE_FILES})

IF (GUSSOUND_FLAG_USE_AUDIERE)
  SET (gussound_project_DEPENDENCIES 
      ${gussound_project_DEPENDENCIES} 
      ${Audiere_LIBRARY}
      ${OGG_LIBRARY}
      ${OPENAL_LIBRARY}
      ${ALUT_LIBRARY}
      ${VORBIS_LIBRARIES}
      )  
ENDIF ()

#
# Link to dependencies
#
TARGET_LINK_LIBRARIES(${gussound_project_name} 
      ${gussound_project_DEPENDENCIES} 
      ${list_of_boost_libs_to_link}
      )

#
# Set the default location of the output executable file.
#
IF (WIN32)
    IF (CMAKE_BUILD_TYPE STREQUAL "Release")
      SET (gussound_project_out_dir "../Release")
    ELSE ()
      IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
        SET (gussound_project_out_dir "../Debug")
      ELSE ()
        SET (gussound_project_out_dir "../")
      ENDIF ()
    ENDIF ()
ELSE()
    SET (gussound_project_out_dir "bin")
ENDIF()

SET_TARGET_PROPERTIES (${gussound_project_name} 
    PROPERTIES 
        RUNTIME_OUTPUT_DIRECTORY ${gussound_project_out_dir}
)

# 
# # only CPack code below this line
# SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "gussound library")
# SET(CPACK_PACKAGE_VENDOR "Augustin Preda")
# SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
# SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
# SET(CPACK_PACKAGE_VERSION_MAJOR "${SERPENTS_VERSION_MAJOR}")
# SET(CPACK_PACKAGE_VERSION_MINOR "${SERPENTS_VERSION_MINOR_3DIGITS}")
# SET(CPACK_PACKAGE_VERSION_PATCH "${SERPENTS_VERSION_PATCH_2DIGITS}")
# 
# SET(CPACK_SOURCE_GENERATOR TBZ2 ZIP)
# SET(CPACK_GENERATOR NSIS)
# 
# set(CPACK_SOURCE_PACKAGE_FILE_NAME "guslibDLLtest" CACHE INTERNAL "tarball basename")
# 
# set(CPACK_SOURCE_IGNORE_FILES
#  # repository history should not be in source tarballs
# "\\\\.hg.*"
# "\\\\.svn.*"
# 
# # the rest is modelled after .hgignore
# "build/"
# "~$"
# 
# "\\\\.project"
# "\\\\.cproject"
# "\\\\.pydevproject"
# 
# "serpents$"
# "serpents.exe$"
# 
# "\\\\.serpents"
# )

# INCLUDE(CPack)
