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

#include "GBaseMenu.h"
#include "GUtils.h"

//#pragma comment(lib, "tinyxml.lib")

#define XML_ITEM_KEY "Item"
#define XML_TYPE_KEY "type"
#define XML_NAME_KEY "name"
#define XML_CAPT_KEY "caption"
#define XML_XINF_KEY "xtraInfo"
#define XML_XPNL_KEY "panel"
#define XML_XUMT_KEY "unselmat"
#define XML_XSMT_KEY "selmat"
#define XML_XCAP_KEY "caption"

#define XML_MAIN_MENU_KEY "MainMenu"
#define XML_V_ACTION "action"
#define XML_V_SUBMENU "submenu"
#define XML_V_OPTION "option"
#define XML_V_OPTVAL "optval"
#define XML_V_BACK "back"

using namespace guslib;

namespace GBase
{

	bool GBaseMenu::processNode(TiXmlNode * node, GMenuItem * currentParent)
	{
		int nmax = G_MAX_MENU_SUBITEMS_COUNT; // don't allow more than this many subitems.
		if( node == NULL )
			return false;

		GTRACE(5, "GBaseMenu::processNode, nmax="<<nmax<<", parent="<<currentParent->getName());
		// tiny xml reading data.
		TiXmlElement * menuElement = node->ToElement();
		TiXmlNode *auxnode;
		TiXmlElement * auxelem = menuElement;
		TiXmlElement * xinfoElem = NULL;
		TiXmlNode *xinfoNode = NULL;

		// data for the current element.
		std::string sType;
		std::string sName;
		std::string sCaption;
		GMenuItem * itemToCreate = NULL;

		// the main item has only sub-items, so process the additional properties only for other items.
		if( std::string(node->Value()) != std::string(XML_MAIN_MENU_KEY) )
		{
			// get the name of the current key
			auxnode = menuElement->FirstChildElement(XML_NAME_KEY);
			if( auxnode == NULL )
				return false;
			auxelem = auxnode->ToElement();
			sName = auxelem->GetText();

			// get the caption of the current key
			auxnode = menuElement->FirstChildElement(XML_CAPT_KEY);
			if( auxnode == NULL )
				return false;
			auxelem = auxnode->ToElement();
			sCaption = auxelem->GetText();

			// read the type
			auxnode = menuElement->FirstChildElement(XML_TYPE_KEY);
			if( auxnode == NULL )
				return false;
			auxelem = auxnode->ToElement();
			sType = auxelem->GetText();

			// create the item according to the type

			if( sType == XML_V_ACTION )
			{
				itemToCreate = new GMenuAction( this, sName, sCaption );
			}else if(sType == XML_V_BACK )
			{
				itemToCreate = new GMenuBack( this, sName, sCaption );
			}else if (sType == XML_V_SUBMENU)
			{
				itemToCreate = new GMenuSubMenu( this, sName, sCaption );
			}else if( sType == XML_V_OPTION )
			{
				itemToCreate = new GMenuOption( this, sName, sCaption );
			}else if( sType == XML_V_OPTVAL )
			{
				GTRACE(5, "option value added:"<<sName<<", "<<sCaption);
				itemToCreate = new GMenuItem( this, sName, sCaption );
			}
			if( itemToCreate != NULL && (sType != std::string(XML_V_OPTVAL) )) // if it was created
			{
				// get the additional properties for non-option-values
				std::string sAux1, sAux2, sAux3, sAux4;

				auxnode = menuElement->FirstChildElement(XML_XINF_KEY);
				if( auxnode == NULL )
					return false;

				xinfoNode = auxnode->FirstChildElement(XML_XPNL_KEY);
				if( xinfoNode == NULL )
					return false;
				xinfoElem = xinfoNode->ToElement();
				sAux1 = xinfoElem->GetText();

				xinfoNode = auxnode->FirstChildElement(XML_XUMT_KEY);
				if( xinfoNode == NULL )
					return false;
				xinfoElem = xinfoNode->ToElement();
				sAux2 = xinfoElem->GetText();

				xinfoNode = auxnode->FirstChildElement(XML_XSMT_KEY);
				if( xinfoNode == NULL )
					return false;
				xinfoElem = xinfoNode->ToElement();
				sAux3 = xinfoElem->GetText();

				xinfoNode = auxnode->FirstChildElement(XML_XCAP_KEY);
				if( xinfoNode == NULL )
					return false;
				xinfoElem = xinfoNode->ToElement();
				sAux4 = xinfoElem->GetText();

				GTRACE(7, "infos are:"<<sAux1<<", "<<sAux2<<", "<<sAux3<<", "<<sAux4);
				itemToCreate->setAuxData( new GAuxQuadStringInfo(sAux1, sAux2, sAux3, sAux4) );
			}
			if( itemToCreate != NULL ) // if it was created
			{
				GTRACE(5, "add child to parent; parent="<<currentParent->getName()<<" , child="<<itemToCreate->getName());
				currentParent->addChild( itemToCreate );
			}
		}

		// get the first sub-item.
		auxnode = menuElement->FirstChildElement(XML_ITEM_KEY);
		// run thru all sub-items.
		while( auxnode != NULL && nmax>0 )
		{
			// add the child nodes.
			if( itemToCreate == NULL )
				processNode(auxnode, currentParent);
			else
				processNode(auxnode, itemToCreate);
			auxnode = auxnode->NextSiblingElement();
			nmax--;
		}

		return true;
	}

	bool GBaseMenu::loadFromXML( const std::string & fileName, const std::string & defaultContent )
	{
		// The loading can be done either from a file or from a regular string.
		// A file should be used while in development, but for the public release it would
		// be better to place the contents here, so that the users wouldn't be able to
		// change it by accident, since missing elements may crash the application.

		if( fileName != "" )
		{
			GTRACE(3, "Loading menu from file ="<<fileName);
			TiXmlDocument doc( fileName.c_str() );
			doc.LoadFile();
			TiXmlNode * node = doc.FirstChild(XML_MAIN_MENU_KEY);
			if( node != NULL )
			{
				return processNode(node, this->getRoot());
			}
			else
			{
				GTRACE(1, "Error loading menu from file:" );
				return false;
			}
		}
		else
		{
			TiXmlDocument doc( "" );
			doc.Parse( defaultContent.c_str() );
			TiXmlNode * node = doc.FirstChild(XML_MAIN_MENU_KEY);
			if( node != NULL )
			{
				return processNode(node, this->getRoot());
			}
		}

		return true;
	}
}