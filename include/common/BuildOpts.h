#pragma once

//    This file is part of Gusty's Serpents
//    Copyright (C) 2009  Augustin Preda (thegusty999@gmail.com)
//
//    Gusty's Serpents is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    This file contains the build options for the current project.
//    To have access to all these macro definitions and flags, it would be a good 
//    ideea to use "force include" and have this file included by force in the project.


/// Flag to control tracing. Use this along with the trace macros (see GTrace.h) to quickly
/// toggle between a development version of a software producs (that should have plenty of
/// debug and trace information to help the development process) and a release version, 
/// that should be optimised generally for speed.
//#ifndef G_DISABLE_TRACING
//#define G_DISABLE_TRACING
//#pragma message ("Tracing is disabled.")
//#endif

#ifndef G_USE_MULTITHREAD
#define G_USE_MULTITHREAD 1

#endif

#ifndef G_USE_BOOST_LIBS
#define G_USE_BOOST_LIBS 1
#endif

/// The macro for thread synchronization will allow code to run in a thread-safe manner.
/// Please note that the boost library is required for this to work.
/// Also note that if using the OGRE rendering engine, OGRE_THREAD_SUPPORT is also a flag
/// that needs to be set (you should find it in OgreConfig.h)

#ifdef G_USE_BOOST_LIBS // -------------------------------

#ifndef G_USE_BOOST_THREADS
#define G_USE_BOOST_THREADS 1
#endif

#ifndef G_USE_BOOST_ANY
#define G_USE_BOOST_ANY 1
#endif

#ifndef G_USE_BOOST_PTR
#define G_USE_BOOST_PTR 1
#endif

#ifndef G_USE_BOOST_PATH
#define G_USE_BOOST_PATH 1
#endif

/// Use boost based random number generation. It seems considerable better than the standard
/// use of rand().

#ifndef G_USE_BOOST_RANDOM
#define G_USE_BOOST_RANDOM 1
#endif

#endif //G_USE_BOOST_LIBS  // -------------------------------


// These two macros can be disabled to remove the sound from a project completely.
// This of course assumes that no direct calls to the classes are done, but that the
// play macros are used instead, since it would also remove the class definitions.

#ifndef GUS_ENABLE_SOUND
#define GUS_ENABLE_SOUND
#endif

#ifndef GAUDIERE_LUA_FUNCS
#define GAUDIERE_LUA_FUNCS
#endif

#ifndef GAME_LUA_FUNCS
#define GAME_LUA_FUNCS
#endif

