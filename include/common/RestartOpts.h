#ifndef _RESTART_OPTS_H
#define _RESTART_OPTS_H

//    This file is part of Gusty's Serpents
//    Copyright (C) 2009-2011  Augustin Preda (thegusty999@gmail.com)
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
//    This file contains the macro definitions for restart parameters for the current project.
//
//	  Last change:	$LastChangedDate$
//	  Revision:		$Revision$

//
//    This is the command line argument used in the restart command.
//    The application will launch a new instance of the same executable, but adding this parameter to the argument list.
//
#ifndef APP_RESTART_PARAMETER_NAME
#define APP_RESTART_PARAMETER_NAME "--restart"
#endif

//
//     A named mutex used to block the startup sequence of the second instance.
//
#ifndef APP_MUTEX_PARAMETER_NAME
#define APP_MUTEX_PARAMETER_NAME "--gusAppMutex"
#endif


#endif // _RESTART_OPTS_H