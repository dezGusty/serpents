# This file is part of the guslib library, licensed under the terms of the MIT License.
# 
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

# 
# Utility macro used to create a new variable (that can have the value 0 or 1). The initial variable
# is expected to be a user specified option (with the values ON/OFF).
# 
MACRO (CREATE_BINARY_VAR_FROM_OPTION _option_name)
  IF ("${_option_name}" STREQUAL "ON")
    SET (${_option_name}_NUM 1)
  ELSE ()
    SET (${_option_name}_NUM 0)
  ENDIF ()

  MESSAGE (STATUS "Set ${_option_name}_NUM to ${${_option_name}_NUM}") 
ENDMACRO (CREATE_BINARY_VAR_FROM_OPTION)


#
# Utility function used to prepare linking to the boost libraries.
# This will add the boost include dir to the inclusion list and will store the necessary boost libraries.
#
# Parameters:
#   _project_name - The project name.
#   _use_boost_threads - need to use the thread lib.
#   _use_boost_smart_ptrs - boost shared ptrs are in use.
#   _use_boost_random - need to use the boost random lib.
#
MACRO (GUSLIB_ADD_BOOST_DEPS _output_list_of_libs _use_boost_threads _use_boost_smart_ptrs _use_boost_random )
  MESSAGE (STATUS "Trying to add boost dependencies")
  SET (LINK_OPTIONS_BOOST_REQUIRED OFF)
  
  IF (${_use_boost_threads})
    SET (LINK_OPTIONS_BOOST_REQUIRED ON)
    MESSAGE (STATUS "-> thread")
  ENDIF ()
  
  IF (${_use_boost_smart_ptrs})
    SET (LINK_OPTIONS_BOOST_REQUIRED ON)
    MESSAGE (STATUS "-> smart ptrs")
  ENDIF ()
  
  IF (${_use_boost_random})
    SET (LINK_OPTIONS_BOOST_REQUIRED ON)
    MESSAGE (STATUS "-> random")
  ENDIF ()
  
  SET (list_of_boost_libs_to_link "")
  
  IF (LINK_OPTIONS_BOOST_REQUIRED)
    IF(WIN32)
      SET(Boost_USE_STATIC_LIBS ON)
      MESSAGE (STATUS "${CMAKE_CURRENT_LIST_FILE}:${CMAKE_CURRENT_LIST_LINE} Boost_USE_STATIC_LIBS was set to ${Boost_USE_STATIC_LIBS}.")
  
    ENDIF(WIN32)
    SET (Boost_NO_BOOST_CMAKE ON)
  #   SET (Boost_DEBUG 1) #uncomment this if you have problems finding boost!
    SET (Boost_USE_MULTITHREADED      ON)
    SET (Boost_USE_STATIC_RUNTIME    OFF)
  
  # Libraries to look for: system date_time chrono thread
    IF (${_use_boost_threads})
      FIND_PACKAGE(Boost COMPONENTS thread system date_time chrono REQUIRED)
      SET (list_of_boost_libs_to_link ${list_of_boost_libs_to_link} ${Boost_LIBRARIES})
    ENDIF ()
    IF (${_use_boost_smart_ptrs})
      FIND_PACKAGE(Boost COMPONENTS system REQUIRED)
      SET (list_of_boost_libs_to_link ${list_of_boost_libs_to_link} ${Boost_LIBRARIES})
    ENDIF ()
    IF (${_use_boost_random})
      FIND_PACKAGE(Boost COMPONENTS random REQUIRED)
      SET (list_of_boost_libs_to_link ${list_of_boost_libs_to_link} ${Boost_LIBRARIES})
    ENDIF ()
    
    IF (Boost_FOUND)
      INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
      MESSAGE (STATUS "Boost was found")
      SET (${_output_list_of_libs} ${list_of_boost_libs_to_link})
      MESSAGE (STATUS "Updated (output) list_of_boost_libs_to_link to: ${list_of_boost_libs_to_link}")
    ELSE ()
      MESSAGE (STATUS "BOOST was NOT found, but is required.")
    ENDIF ()
  ENDIF () 
  
   
ENDMACRO (GUSLIB_ADD_BOOST_DEPS)
