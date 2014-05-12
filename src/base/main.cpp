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

//
// Includes
//

//
// C system headers
//

// TODO(Augustin Preda, 2014.04.25): Make platform independent!
// Include the main Windows headers.
#include <windows.h>

//
// C++ system headers
//

// Allow the use of std::exception
#include <exception>

//
// This project's headers
//

// The core of the application loop is handled in this class
#include "app/ogreapp.h"

// Add a preprocessor symbol. Not sure about the specifics of how it affects the
// build, but the project doesn't build on Windows without it.
#define WIN32_LEAN_AND_MEAN


/**
  The entry point for the application.
*/
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
{
  // Create the main class of the application and try to run it.
  app::OgreApp mainApp;

  try
  {
    mainApp.run();
  }
  catch (const std::exception& e)
  {
    MessageBoxA(NULL, e.what(), "A std exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
  }
  catch (...)
  {
    MessageBoxA(NULL, "unkown exception", "A std exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
  }

  return 0;
}
