# - Locate Audiere
# This module defines
#  Audiere_LIBRARY
#  Audiere_FOUND, if false, do not try to link against Audiere
#  Audiere_INCLUDE_DIR, where to find the headers
#
# Created by Augustin Preda. Adapted from the [FindALUT.cmake] module (by Eric Wing).
#

INCLUDE(FindPackageHandleStandardArgs)
INCLUDE(HandleLibraryTypes)

SET (Audiere_SEARCH_PATH ${PROJECT_SOURCE_DIR}/dependencies/audiere 
        ${PROJECT_SOURCE_DIR}/../dependencies/audiere
        $ENV{AUDIEREDIR})
        
FIND_PATH(Audiere_INCLUDE_DIR audiere.h
  PATHS  ${Audiere_SEARCH_PATH}
  ~/Library/Frameworks/Audiere.framework
  /Library/Frameworks/Audiere.framework
  /System/Library/Frameworks/Audiere.framework
  PATH_SUFFIXES include include/audiere include/Audiere audiere
)

# I'm not sure if I should do a special casing for Apple. It is
# unlikely that other Unix systems will find the framework path.
# But if they do ([Next|Open|GNU]Step?),
# do they want the -framework option also?
IF(${Audiere_INCLUDE_DIR} MATCHES ".framework")

  STRING(REGEX REPLACE "(.*)/.*\\.framework/.*" "\\1" AUDIERE_FRAMEWORK_PATH_TMP ${Audiere_INCLUDE_DIR})
  IF("${ALUT_FRAMEWORK_PATH_TMP}" STREQUAL "/Library/Frameworks"
      OR "${ALUT_FRAMEWORK_PATH_TMP}" STREQUAL "/System/Library/Frameworks"
      )
    # String is in default search path, don't need to use -F
    SET (Audiere_LIBRARY_OPTIMIZED "-framework Audiere" CACHE STRING "Audiere framework for OSX")
  ELSE()
    # String is not /Library/Frameworks, need to use -F
    SET(Audiere_LIBRARY_OPTIMIZED "-F${Audiere_FRAMEWORK_PATH_TMP} -framework Audiere" CACHE STRING "Audiere framework for OSX")
  ENDIF()
  # Clear the temp variable so nobody can see it
  SET(Audiere_FRAMEWORK_PATH_TMP "" CACHE INTERNAL "")

ELSE()
  FIND_LIBRARY(Audiere_LIBRARY_OPTIMIZED
    NAMES audiere
    PATHS ${Audiere_SEARCH_PATH}
    PATH_SUFFIXES lib libs
  )
  FIND_LIBRARY(Audiere_LIBRARY_DEBUG
    NAMES audiered audiere
    PATHS ${Audiere_SEARCH_PATH}
    PATH_SUFFIXES lib libs
  )
ENDIF()

IF (NOT AUDIERE_STATIC)
  FIND_FILE (AUDIERE_BINARY_REL 
      NAMES "audiere.dll" 
      HINTS ${Audiere_SEARCH_PATH}
      PATH_SUFFIXES "" Release relwithdebinfo minsizerel
  )
        
  FIND_FILE (AUDIERE_BINARY_DBG 
      NAMES "audiere.dll" "audiered.dll" 
      HINTS ${Audiere_SEARCH_PATH}
      PATH_SUFFIXES "" debug
  )
ENDIF ()

# Handle the REQUIRED argument and set Audiere_FOUND
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Audiere DEFAULT_MSG
    Audiere_LIBRARY_OPTIMIZED
    Audiere_INCLUDE_DIR
)

# Collect optimized and debug libraries
HANDLE_LIBRARY_TYPES(Audiere)

MARK_AS_ADVANCED(
    Audiere_INCLUDE_DIR
    Audiere_LIBRARY_OPTIMIZED
    Audiere_LIBRARY_DEBUG
)
