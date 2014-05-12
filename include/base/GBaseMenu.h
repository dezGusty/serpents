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
//    Custom menu creation. (Uses tinyXML).
//    Make sure you include the tinyXML (https://sourceforge.net/projects/tinyxml/)
//    (or http://www.grinninglizard.com/tinyxml/) library, adding the header to the
//    VC++ include directories and the lib to the VC++ lib directories.

#include <guslib/util/consolemenu.h>
#include <guslib/thirdparty/tinyxml.h>

// The maximum number of children (as in direct descendants of a node, NOT recursion levels)
#define G_MAX_MENU_SUBITEMS_COUNT 100

namespace GBase
{
	/// Extend the functionality of the standard menu handler by adding support for loading
	/// the data from an XML (via tinyXML)
	class GBaseMenu
		: public guslib::GMenu
	{
	public:
		GBaseMenu(){}
		virtual ~GBaseMenu(){}

		/// Allows the loading from an XML. This can be either an external file or an internal
		/// string. If the filename is an empty string, the content will be loaded from the
		/// defaultContent string.
		/// A file should be used while in development, but for the public release it would
		/// be better to place the contents here, so that the users wouldn't be able to
		/// change it by accident, since missing elements may crash the application.
		virtual bool loadFromXML( const std::string& fileName="", const std::string& defaultContent="" );

	protected:
		virtual bool processNode(TiXmlNode * node, guslib::GMenuItem * currentParent);
	};

	// This will be used in general to store the following information:
	// button container (1), button material off (2), button material on (3), text container (4)
	class GAuxQuadStringInfo : public guslib::GMenuAuxData
	{
	protected:
		std::string info;
		std::string info2;
		std::string info3;
		std::string info4;
	public:
		GAuxQuadStringInfo(std::string & s1, std::string & s2, std::string & s3, std::string & s4)
		{info=s1; info2=s2; info3=s3; info4=s4;}
		~GAuxQuadStringInfo(){}
		std::string getInfo(){ return info; }
		std::string getInfo2(){ return info2; }
		std::string getInfo3(){ return info3; }
		std::string getInfo4(){ return info4; }
	};
}