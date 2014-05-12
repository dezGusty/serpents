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

//    File to define base constants.
//    These constants should not be tied to any particular game. They are meant
//    to be general and be reused wherever possible


//    Rendering engine names.

const std::string G_RENDER_ENGINE_D3D_LONG_NAME = "Direct3D9 Rendering Subsystem";
const std::string G_RENDER_ENGINE_OGL_LONG_NAME = "OpenGL Rendering Subsystem";
const std::string G_RENDER_ENGINE_D3D_SHORT_NAME = "Direct3D";
const std::string G_RENDER_ENGINE_OGL_SHORT_NAME = "OpenGL";
const std::string G_PLUGIN_D3D = "RenderSystem_Direct3D9";
const std::string G_PLUGIN_OGL = "RenderSystem_GL";
