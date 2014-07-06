# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 2.6)
PROJECT(serpents)

MESSAGE(STATUS "")
MESSAGE(STATUS "### Welcome to Gusty's Serpents build system ###")
MESSAGE(STATUS "************************************************")
MESSAGE(STATUS "   (based on the Summoning Wars build system)")
MESSAGE(STATUS "")

ADD_DEFINITIONS(-DUNICODE -D_UNICODE -DGUSLIB_IMP -DGUSSOUND_IMP)

OPTION(SERPENTS_NO_TINYXML "Mark this if you do not want to use the bundled, in-source TinyXML Lib" OFF)
# OPTION(SERPENTS_POST_BUILD_COPY "After a build, copy the executable to the project folder (Win32)" ON)
OPTION(SERPENTS_STANDALONE_MODE "(Highly recommended for deployment on Windows and MacOSX) In case this is enabled, all paths (share, doc, bin) are set to be relative" OFF)
OPTION(SERPENTS_BUILD_GUSLIB "Specify whether to build the guslib library" ON)
OPTION(SERPENTS_BUILD_GUSSOUND "Specify whether to build the gussound library" ON)
OPTION(SERPENTS_ENABLE_SCRIPTING "Specify whether to enable scripting or not" ON)
OPTION(SERPENTS_SUPPRESS_OGRE_C4251 "Specify whether to suppress the C4251 warnings related to Ogre" ON)

OPTION (OGG_STATIC "Use static OGG link" ON)
OPTION (ALUT_STATIC "Use static ALUT link" OFF)
OPTION (GUSLIB_STATIC "Use static guslib link" OFF)
OPTION (OIS_STATIC "Use static OIS link" OFF)

# Add definitions for scripting.
IF (SERPENTS_ENABLE_SCRIPTING)
  ADD_DEFINITIONS (-DSERPENTS_LUA)
ENDIF ()

#
# --------------------------------- Set-up the path to the CMake Modules ----------------------------------------------- 
#
SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/;${CMAKE_MODULE_PATH}")

IF(WIN32)
  SET(CMAKE_MODULE_PATH "$ENV{OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
ENDIF(WIN32)

IF(UNIX)
  SET(CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
ENDIF(UNIX)

#
# ----------------------------------- Include other cmake scripts for macros and functions -----------------------------
#

# Include a small macro file.
INCLUDE (GusLibUtils)

#
# Include subdirectories.
#
IF (SERPENTS_BUILD_GUSLIB)
  ADD_SUBDIRECTORY (guslib)
ENDIF ()

IF (SERPENTS_BUILD_GUSSOUND)
  ADD_SUBDIRECTORY (gussound)
ENDIF ()

SET (list_of_boost_libs_to_link "")
# Add the boost dependencies (this adds the both the include directories and the library linkage).
SET (SERPENTS_LINK_BOOST_SMART_POINTERS OFF)
SET (SERPENTS_LINK_BOOST_RANDOM OFF)
SET (SERPENTS_LINK_BOOST_THREADS ON)

GUSLIB_ADD_BOOST_DEPS(list_of_boost_libs_to_link SERPENTS_LINK_BOOST_THREADS SERPENTS_LINK_BOOST_SMART_POINTERS SERPENTS_LINK_BOOST_RANDOM)
MESSAGE (STATUS "Should link to: ${list_of_boost_libs_to_link}")

# 
# Configure the version file. Set the variables then call the parsing function.
# 
SET(SERPENTS_VERSION_MAJOR "0")
SET(SERPENTS_VERSION_MINOR_1DIGIT "3")
SET(SERPENTS_VERSION_MINOR_2DIGITS "03")
SET(SERPENTS_VERSION_MINOR_3DIGITS "003")
SET(SERPENTS_VERSION_PATCH_1DIGIT "0")
SET(SERPENTS_VERSION_PATCH_2DIGITS "0")

# Create a variable named SERPENTS_SUPPRESS_OGRE_C4251_NUM with the value "0" or "1".
CREATE_BINARY_VAR_FROM_OPTION (SERPENTS_SUPPRESS_OGRE_C4251)

CONFIGURE_FILE("src/config/serpents_version.h.in" "src/config/serpents_version.h")

#
# Also set a common environment variable for a nicer display here.
#
SET(SERPENTS_VERSION "${SERPENTS_VERSION_MAJOR}.${SERPENTS_VERSION_MINOR_3DIGITS}.${SERPENTS_VERSION_PATCH_2DIGITS}")
MESSAGE(STATUS "Version: ${SERPENTS_VERSION}")

#
# Set the default location of the output executable file.
#
IF (WIN32)
    IF (CMAKE_BUILD_TYPE STREQUAL "Release")
      SET (SERPENTS_EXECUTABLE_DEFAULT_DIR "Release")
    ELSE ()
      IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
        SET (SERPENTS_EXECUTABLE_DEFAULT_DIR "Debug")
      ELSE ()
        SET (SERPENTS_EXECUTABLE_DEFAULT_DIR "game")
      ENDIF ()
    ENDIF ()
ELSE()
    SET (SERPENTS_EXECUTABLE_DEFAULT_DIR "bin")
ENDIF()

SET(SERPENTS_EXECUTABLE_DIR "${SERPENTS_EXECUTABLE_DEFAULT_DIR}" CACHE STRING "Path that represents the directory with serpents executable, CMAKE_INSTALL_PREFIX gets prepended! (Only applies when SERPENTS_STANDALONE_MODE is OFF)")


IF (NOT CMAKE_BUILD_TYPE)
  # CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
  # differentiation between debug and release builds.
  SET( CMAKE_BUILD_TYPE "Release" )
ENDIF()

MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "")



SET(CMAKE_DEBUG_POSTFIX "_d")

IF(NOT CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/dist")
ENDIF()

MESSAGE(STATUS "Searching for OGRE")

FIND_PACKAGE(OGRE REQUIRED)

MESSAGE(STATUS "*** Obtained OGRE_VERSION_NAME as \"${OGRE_VERSION_NAME}\"")
IF("${OGRE_VERSION_NAME}" STREQUAL "Ghadamon")
  set(SERPENTS_OGRE_SINGLETON "msSingleton") # not really needed; todo: remove
ELSEIF("${OGRE_VERSION_NAME}" STREQUAL "Byatis")
  set(SERPENTS_OGRE_SINGLETON "msSingleton") # not really needed; todo: remove
ELSE()
  MESSAGE(SEND_ERROR "You need Ogre 1.8 Byatis or 1.9 Ghadamon to build this. You have: ${OGRE_VERSION_NAME}")
ENDIF()

# add our build include directory
include_directories(
    "${CMAKE_CURRENT_BINARY_DIR}/src"
    "${CMAKE_CURRENT_BINARY_DIR}/src/gui"
)

IF (WIN32)
#  FIND_PACKAGE(BoostOGRE REQUIRED)
ENDIF (WIN32)
FIND_PACKAGE(OIS REQUIRED)
FIND_PACKAGE(Lua51 REQUIRED)
FIND_PACKAGE(CEGUI REQUIRED)
FIND_PACKAGE(CEGUIOGRE REQUIRED)
FIND_PACKAGE(OpenAL REQUIRED)
FIND_PACKAGE(ALUT REQUIRED)
FIND_PACKAGE(Ogg REQUIRED)
FIND_PACKAGE(Vorbis REQUIRED)
IF(SERPENTS_NO_TINYXML)
  FIND_PACKAGE(TinyXML REQUIRED)
ENDIF()

IF (UNIX AND NOT APPLE)
  FIND_PACKAGE(X11 REQUIRED)
ENDIF()

# Update the batch file used to copy DLLs
IF (WIN32)
  CONFIGURE_FILE ("env_setup/copyReleaseDLLs.bat.in" "env_setup/copyReleaseDLLs.bat")
  MESSAGE(STATUS "****** You can run [env_setup/copyReleaseDLLs.bat] to copy the Release DLLs ******")
  CONFIGURE_FILE ("env_setup/copyDebugDLLs.bat.in" "env_setup/copyDebugDLLs.bat")
  MESSAGE(STATUS "****** You can run [env_setup/copyDebugDLLs.bat] to copy the Debug DLLs ******")
ENDIF (WIN32)

#
# --------------------------------------------- Set the output file location -------------------------------------------
#

# Set the destination project target
if (WIN32)
    IF (CMAKE_BUILD_TYPE STREQUAL "Release")
      set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release")
    ELSE ()
      IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
        SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Debug")
      ELSE ()
        SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
      ENDIF ()
    ENDIF ()
else()
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
endif()


INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/guslib/guslib)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/gussound/include)
INCLUDE_DIRECTORIES(${OGRE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${OGRE_RTShaderSystem_INCLUDE_DIR})
IF (WIN32)
#   Message(STATUS "Adding boost include: ${BoostOGRE_INCLUDE_DIR}")
#   INCLUDE_DIRECTORIES(${BoostOGRE_INCLUDE_DIR})
ENDIF (WIN32)
INCLUDE_DIRECTORIES(${OIS_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${LUA_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CEGUI_INCLUDE_DIR})
# INCLUDE_DIRECTORIES(${CEGUIOGRE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${OPENAL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${ALUT_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${OGG_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${VORBIS_INCLUDE_DIR})

IF(WIN32)
#   INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/dirent)
#   INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/boost)
  ADD_DEFINITIONS( "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4800 /wd4244 /wd4305 /nologo" )
  MESSAGE("WARNING: Compiler warning 4800 disabled ('type' : forcing value to bool 'true' or 'false' (performance warning)")
  MESSAGE("WARNING: Compiler warning 4244 disabled ('conversion' conversion from 'type1' to 'type2', possible loss of data)")
  MESSAGE("WARNING: Compiler warning 4305 disabled ('identifier' : truncation from 'type1' to 'type2')")
ELSE()
  INCLUDE_DIRECTORIES(${PTHREADS_INCLUDE_DIR})
ENDIF()

IF(UNIX AND NOT APPLE)
  INCLUDE_DIRECTORIES(${X11_X11_INCLUDE_PATH})
  INCLUDE_DIRECTORIES(${X11_Xrandr_INCLUDE_PATH})
ENDIF()

CONFIGURE_FILE("src/config/serpents_config.h.in" "src/config/serpents_config.h")

FILE(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)


# 
# -------------------------------------------------- File section ------------------------------------------------------
#

SET (SOURCE_FILES
  ./include/common/BuildOpts.h
  ./include/common/RestartOpts.h
  ./include/model/BodyPartBase.h
  ./include/model/Bonus.h
  ./include/model/GRenderable.h
  ./include/model/Map.h
  ./include/model/MapObject.h
  ./include/model/PathStrip.h
  ./include/model/SerpentBase.h
  ./include/serp/SerpConstants.h
  ./src/base/main.cpp
  ./src/model/BodyPartBase.cpp
  ./src/model/Bonus.cpp
  ./src/model/Map.cpp
  ./src/model/MapObject.cpp
  ./src/model/PathStrip.cpp
  ./src/model/SerpentBase.cpp
  ./src/utils/CEGUIUtils.cpp
  ./src/utils/CEGUIUtils.h
  ./src/utils/GGeneralUtils.cpp
)

# Specify the group for base app files
SET (SOURCE_FILES_APP
  ./src/app/baseappinterface.cpp
  ./src/app/baseappinterface.h
  ./src/app/GameConsole.cpp
  ./src/app/GameConsole.h
  ./src/app/ogreapp.cpp
  ./src/app/ogreapp.h
  ./src/app/serpentsengine.cpp
  ./src/app/serpentsengine.h
  ./src/app/serpentsframelistener.cpp
  ./src/app/serpentsframelistener.h
  ./src/app/serpentsgui.cpp
  ./src/app/serpentsgui.h
  ./src/app/serpentsstate.cpp
  ./src/app/serpentsstate.h
) 


# Specify the group for the configuration files.
SET (SOURCE_FILES_CONFIG
  ./src/config/serpents_config.h
  ./src/config/serpents_version.h
  ./src/config/serpentsogre.h
)

# Specify the group for state implementations.
SET (SOURCE_FILES_IMPL
  ./src/impl/menustate.cpp
  ./src/impl/menustate.h
  ./src/impl/SerpMaterialListener.cpp
  ./src/impl/SerpMaterialListener.h
  ./src/impl/workbenchstate.cpp
  ./src/impl/workbenchstate.h
)

# Specify the group for engine
SET (SOURCE_FILES_ENGINE
  ./src/engine/renderableobject.cpp
  ./src/engine/renderableobject.h
  ./src/engine/renderableobjectinterface.h
  ./src/engine/scene.cpp
  ./src/engine/scene.h
  ./src/engine/serpentscamera.cpp
  ./src/engine/serpentscamera.h
  ./src/engine/serpentsgamehelper.cpp
  ./src/engine/serpentsgamehelper.h
  ./src/engine/shadinghelper.cpp
  ./src/engine/shadinghelper.h
)


# Specify the 3rd party files added to the build.
SET (SOURCE_FILES_THIRD_PARTY
  ./guslib/guslib/guslib/thirdparty/fast_mutex.h
  ./guslib/guslib/guslib/thirdparty/tinythread.cpp
  ./guslib/guslib/guslib/thirdparty/tinythread.h
)

# Specify the 3rd party files added to the build.
SET (SOURCE_FILES_SCRIPTING
  ./src/scripting/scripting.cpp
  ./src/scripting/scripting.h
)

IF (WIN32)
  SET (RESOURCE_FILES
    ./src/res/serpents.rc
    ./src/res/resource.h
  )
  # Create the source group for it (so that it does not appear directly under source files in VS).
  SOURCE_GROUP ("res" FILES ${RESOURCE_FILES})
ENDIF ()

# Create the source groups, using the already defined variables.
SOURCE_GROUP ("src\\3rdparty" FILES ${SOURCE_FILES_THIRD_PARTY})
SOURCE_GROUP ("src\\config" FILES ${SOURCE_FILES_CONFIG})
SOURCE_GROUP ("src\\engine" FILES ${SOURCE_FILES_ENGINE})
SOURCE_GROUP ("src\\scripting" FILES ${SOURCE_FILES_SCRIPTING})

SOURCE_GROUP("include" FILES
  ./include/common/BuildOpts.h
  ./include/common/RestartOpts.h
  ./include/model/BodyPartBase.h
  ./include/model/Bonus.h
  ./include/model/GRenderable.h
  ./include/model/Map.h
  ./include/model/MapObject.h
  ./include/model/PathStrip.h
  ./include/model/SerpentBase.h
  ./include/serp/SerpConstants.h
)

SOURCE_GROUP("src\\app" FILES ${SOURCE_FILES_APP})

SOURCE_GROUP("src\\base" FILES
  ./src/base/main.cpp
)

SOURCE_GROUP ("src\\impl" FILES ${SOURCE_FILES_IMPL})

SOURCE_GROUP("src\\model" FILES
  ./src/model/BodyPartBase.cpp
  ./src/model/Bonus.cpp
  ./src/model/Map.cpp
  ./src/model/MapObject.cpp
  ./src/model/PathStrip.cpp
  ./src/model/SerpentBase.cpp
)

SOURCE_GROUP("src\\utils" FILES
  ./src/utils/CEGUIUtils.cpp
  ./src/utils/CEGUIUtils.h
  ./src/utils/GGeneralUtils.cpp
)


# If building for windows based systems, the resource file (containing 
# the manifest, icon and other resources) should also be included
IF (WIN32)
  SET(SOURCE_FILES 
    ${SOURCE_FILES}
    ${SOURCE_FILES_APP}
    ${SOURCE_FILES_CONFIG}
    ${SOURCE_FILES_ENGINE}
    ${SOURCE_FILES_IMPL}
    ${SOURCE_FILES_SCRIPTING}
    ${SOURCE_FILES_THIRD_PARTY}
    ${RESOURCE_FILES}
    )
ENDIF (WIN32)


If (WIN32)
  ADD_EXECUTABLE(serpents WIN32 ${SOURCE_FILES})
ELSE ()
  ADD_EXECUTABLE(serpents ${SOURCE_FILES})
ENDIF ()

SET_TARGET_PROPERTIES(serpents PROPERTIES OUTPUT_NAME  "serpents")


#${CEGUILUA_LIBRARY}


SET(LINK_LIBS
  ${OGRE_LIBRARIES}
  ${OGRE_Terrain_LIBRARY}
  ${OGRE_RTShaderSystem_LIBRARY}
  ${OIS_LIBRARIES} 
  ${CEGUI_LIBRARY} 
  ${CEGUIOGRE_LIBRARY}
  ${LUA_LIBRARIES} 
  ${OPENAL_LIBRARY} 
  ${ALUT_LIBRARY} 
  ${OGG_LIBRARY}
  ${VORBIS_LIBRARIES}
  guslib
  gussound 
  )

SET (LINK_LIBS
  ${LINK_LIBS}
  ${list_of_boost_libs_to_link}
)

IF(WIN32)
  SET(LINK_LIBS
    ws2_32.lib
    winmm.lib
    ${LINK_LIBS}
#     ${BoostOGRE_LIBRARIES}
    )
ELSEIF(UNIX AND NOT APPLE)
  SET(LINK_LIBS
    ${X11_X11_LIB}
    ${X11_Xrandr_LIB}
    ${LINK_LIBS}
    )
ENDIF()

TARGET_LINK_LIBRARIES(serpents ${LINK_LIBS})

# IF(SERPENTS_POST_BUILD_COPY)
#   # Due to the fact that CMake can't yet target properly subconfigurations in multi-configuration environments
#   # (such as Visual Studio), make a special case using Visual Studio macros.
#   IF (MSVC)
#     SET (CUSTOM_POST_BUILD_COPY_COMMAND "copy $(OutDir)$(TargetFileName) $(ProjectDir)Game/$(TargetFileName)")
#     ADD_CUSTOM_COMMAND(TARGET serpents
#       POST_BUILD
#       COMMAND ${CUSTOM_POST_BUILD_COPY_COMMAND}
#     )
#     MESSAGE (STATUS "Added post-build event (MSVC) ${CUSTOM_POST_BUILD_COPY_COMMAND}")
#   ELSE ()
#     MESSAGE (STATUS "Not (MSVC)")
# #     ADD_CUSTOM_COMMAND(TARGET serpents
# #       POST_BUILD
# #       COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/serpents${CMAKE_EXECUTABLE_SUFFIX} ${CMAKE_SOURCE_DIR}
# #     )
#   ENDIF (MSVC)
# ENDIF()


# install the application
install(TARGETS serpents
  RUNTIME DESTINATION "${SERPENTS_EXECUTABLE_DIR}")
 
# # freedesktop.org icons and desktop files
# # TODO: How do we do this on Windows? Installing start menu items.
# if (UNIX)
#   install(FILES "packaging/serpents.desktop"
#     DESTINATION "share/applications")
#     
#   install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/share/icon/serpentsIcon_16x16.png"
#     DESTINATION "share/icons/hicolor/16x16/"
#     RENAME "serpents.png")
#   install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/share/icon/serpentsIcon_128x128.png"
#     DESTINATION "share/icons/hicolor/128x128/"
#     RENAME "serpents.png")
# endif()
# 
# install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/share/data"
#   DESTINATION "${SERPENTS_SHARE_DIR}"
#   PATTERN ".hg" EXCLUDE)
# install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/share/resources"
#   DESTINATION "${SERPENTS_SHARE_DIR}"
#   PATTERN ".hg" EXCLUDE
#   PATTERN "icon" EXCLUDE)
# install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/share/translation"
#   DESTINATION "${SERPENTS_SHARE_DIR}"
#   FILES_MATCHING PATTERN "*.mo"
#   PATTERN ".hg" EXCLUDE
#   PATTERN "xmlgettext" EXCLUDE) # both pattern excludes are there to prevent the directories from being created
#   

# 
# 
# # only CPack code below this line
# SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Gusty's Serpents")
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
# set(CPACK_SOURCE_PACKAGE_FILE_NAME "serpents-${SERPENTS_SHORT_VERSION}" CACHE INTERNAL "tarball basename")
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
# 
# INCLUDE(CPack)
