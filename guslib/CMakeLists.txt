#-------------------------------------------------------------------
# This file is part of the CMake build system for the guslib library
#-------------------------------------------------------------------

# Configure Components build
cmake_minimum_required(VERSION 2.6)

# 
# Specify some user configurable options
# 
OPTION(GUSLIB_BUILD_CORE "Build the core lib" ON)
# OPTION(GUSLIB_BUILD_DIRECT_TEST "Build a test, using the source files directly" ON)
OPTION(GUSLIB_BUILD_REGULAR_TEST "Build a test, linking to the guslib library" ON)
OPTION(GUSLIB_BUILD_DLL_TEST "Build a test of a DLL including the guslib source" ON)


project(guslib)

if (GUSLIB_BUILD_CORE)
  add_subdirectory(guslib)
endif ()

# if (GUSLIB_BUILD_DIRECT_TEST)
#   add_subdirectory(directGusLibTest)
# endif ()
# 
if (GUSLIB_BUILD_REGULAR_TEST)
  add_subdirectory(guslibtest)
endif ()
 
if (GUSLIB_BUILD_DLL_TEST)
	add_subdirectory(guslibTestDLL)
endif ()
