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
//    Implementation of the graphical border or the game surface.

#define GUS_USE_WORLD_GEOMETRY

#include <Ogre.h>


static bool objectsWereCreated = false;


void OSE_createTheBorder( Ogre::SceneManager * sceneManager, double width, double height, double border,
						 Ogre::Vector3 translationVector, std::string materialName,
						 std::string resourceGroupName )
{
#define OSE_createBorderF1(pnode) enttmp->setMaterialName( materialName.c_str() );\
	pnode->attachObject( enttmp );\
	pnode->translate( translationVector )

	Ogre::SceneNode * planeNodeL1 = sceneManager->getRootSceneNode()->createChildSceneNode("planeNodeL1");
	Ogre::SceneNode * planeNodeL2 = sceneManager->getRootSceneNode()->createChildSceneNode("planeNodeL2");
	Ogre::SceneNode * planeNodeL3 = sceneManager->getRootSceneNode()->createChildSceneNode("planeNodeL3");
	Ogre::SceneNode * planeNodeR1 = sceneManager->getRootSceneNode()->createChildSceneNode("planeNodeR1");
	Ogre::SceneNode * planeNodeR2 = sceneManager->getRootSceneNode()->createChildSceneNode("planeNodeR2");
	Ogre::SceneNode * planeNodeR3 = sceneManager->getRootSceneNode()->createChildSceneNode("planeNodeR3");
	Ogre::SceneNode * planeNodeT1 = sceneManager->getRootSceneNode()->createChildSceneNode("planeNodeT1");
	Ogre::SceneNode * planeNodeT2 = sceneManager->getRootSceneNode()->createChildSceneNode("planeNodeT2");
	Ogre::SceneNode * planeNodeT3 = sceneManager->getRootSceneNode()->createChildSceneNode("planeNodeT3");
	Ogre::SceneNode * planeNodeB1 = sceneManager->getRootSceneNode()->createChildSceneNode("planeNodeB1");
	Ogre::SceneNode * planeNodeB2 = sceneManager->getRootSceneNode()->createChildSceneNode("planeNodeB2");
	Ogre::SceneNode * planeNodeB3 = sceneManager->getRootSceneNode()->createChildSceneNode("planeNodeB3");

	//Plane planeL1( Vector3::UNIT_Y, 0 );//almost parallel to user
	Ogre::Plane planeL1( Ogre::Vector3::UNIT_X, 0 );//left inner side
	Ogre::Plane planeL2( Ogre::Vector3::NEGATIVE_UNIT_X, 0 );//left outer side
	Ogre::Plane planeL3( Ogre::Vector3::UNIT_Y, 0 );//	left upper side

	Ogre::Plane planeR1( Ogre::Vector3::NEGATIVE_UNIT_X, 0 );//right inner side
	Ogre::Plane planeR2( Ogre::Vector3::UNIT_X, 0 );//right outer side
	Ogre::Plane planeR3( Ogre::Vector3::UNIT_Y, 0 );//	right upper side

	Ogre::Plane planeT1( Ogre::Vector3::UNIT_Z, 0 );//top inner side
	Ogre::Plane planeT2( Ogre::Vector3::NEGATIVE_UNIT_Z, 0 );//top outer side
	Ogre::Plane planeT3( Ogre::Vector3::UNIT_Y, 0 );//top upper side

	Ogre::Plane planeB1( Ogre::Vector3::NEGATIVE_UNIT_Z, 0 );//bottom inner side
	Ogre::Plane planeB2( Ogre::Vector3::UNIT_Z, 0 );//bottom outer side
	Ogre::Plane planeB3( Ogre::Vector3::UNIT_Y, 0 );//bottom upper side

	double planeSizeX = border;
	double planeSizeY = height + border * 2;
	double planeSizeXH = width;
	double planeSizeXH2 = width + border * 2;
	double planeSizeYH = border;

	if( ! objectsWereCreated )
	{
		Ogre::MeshManager::getSingleton().createPlane("planeNodeL1",
			   resourceGroupName, planeL1,
			   planeSizeX, planeSizeY, 1, 1, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
		Ogre::MeshManager::getSingleton().createPlane("planeNodeL2",
			   resourceGroupName, planeL2,
			   planeSizeX, planeSizeY, 1, 1, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
		Ogre::MeshManager::getSingleton().createPlane("planeNodeL3",
			   resourceGroupName, planeL3,
			   planeSizeX, planeSizeY, 1, 1, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
		Ogre::MeshManager::getSingleton().createPlane("planeNodeR1",
			   resourceGroupName, planeR1,
			   planeSizeX, planeSizeY, 1, 1, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
		Ogre::MeshManager::getSingleton().createPlane("planeNodeR2",
			   resourceGroupName, planeR2,
			   planeSizeX, planeSizeY, 1, 1, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
		Ogre::MeshManager::getSingleton().createPlane("planeNodeR3",
			   resourceGroupName, planeR3,
			   planeSizeX, planeSizeY, 1, 1, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
		Ogre::MeshManager::getSingleton().createPlane("planeNodeT1",
			   resourceGroupName, planeT1,
			   planeSizeXH, planeSizeYH, 1, 1, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);
		Ogre::MeshManager::getSingleton().createPlane("planeNodeT2",
			   resourceGroupName, planeT2,
			   planeSizeXH, planeSizeYH, 1, 1, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);
		Ogre::MeshManager::getSingleton().createPlane("planeNodeT3",
			   resourceGroupName, planeT3,
			   planeSizeXH2, planeSizeYH, 1, 1, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
		Ogre::MeshManager::getSingleton().createPlane("planeNodeB1",
			   resourceGroupName, planeB1,
			   planeSizeXH, planeSizeYH, 1, 1, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);
		Ogre::MeshManager::getSingleton().createPlane("planeNodeB2",
			   resourceGroupName, planeB2,
			   planeSizeXH2, planeSizeYH, 1, 1, true, 1, 5, 5, Ogre::Vector3::UNIT_Y);
		Ogre::MeshManager::getSingleton().createPlane("planeNodeB3",
			   resourceGroupName, planeB3,
			   planeSizeXH, planeSizeYH, 1, 1, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

		objectsWereCreated = true;
	}

	Ogre::Entity * enttmp;

	enttmp = sceneManager->createEntity( "planeEntL1", "planeNodeL1" );
	OSE_createBorderF1(planeNodeL1);
	planeNodeL1->translate( Ogre::Vector3( - width/2, 0, 0 ));

	enttmp = sceneManager->createEntity( "planeEntL2", "planeNodeL2" );
	OSE_createBorderF1(planeNodeL2);
	planeNodeL2->translate( Ogre::Vector3( - width/2 - border, 0, 0 ));

	enttmp = sceneManager->createEntity( "planeEntL3", "planeNodeL3" );
	OSE_createBorderF1(planeNodeL3);
	planeNodeL3->translate( Ogre::Vector3( - width/2 - border/2, border/2, 0 ));

	enttmp = sceneManager->createEntity( "planeEntR1", "planeNodeR1" );
	OSE_createBorderF1(planeNodeR1);
	planeNodeR1->translate( Ogre::Vector3( width/2, 0, 0 ));

	enttmp = sceneManager->createEntity( "planeEntR2", "planeNodeR2" );
	OSE_createBorderF1(planeNodeR2);
	planeNodeR2->translate( Ogre::Vector3( width/2 + border, 0, 0 ));

	enttmp = sceneManager->createEntity( "planeEntR3", "planeNodeR3" );
	OSE_createBorderF1(planeNodeR3);
	planeNodeR3->translate( Ogre::Vector3( width/2 + border/2, border/2, 0 ));

	enttmp = sceneManager->createEntity( "planeEntT1", "planeNodeT1" );
	OSE_createBorderF1(planeNodeT1);
	planeNodeT1->translate( Ogre::Vector3( 0, 0, - height/2 ));

	enttmp = sceneManager->createEntity( "planeEntT2", "planeNodeT2" );
	OSE_createBorderF1(planeNodeT2);
	planeNodeT2->translate( Ogre::Vector3( 0, 0, - height/2 - border ));

	enttmp = sceneManager->createEntity( "planeEntT3", "planeNodeT3" );
	OSE_createBorderF1(planeNodeT3);
	planeNodeT3->translate( Ogre::Vector3( 0, border/2, - height/2 - border/2 ));

	enttmp = sceneManager->createEntity( "planeEntB1", "planeNodeB1" );
	OSE_createBorderF1(planeNodeB1);
	planeNodeB1->translate( Ogre::Vector3( 0, 0, height/2 ));

	enttmp = sceneManager->createEntity( "planeEntB2", "planeNodeB2" );
	OSE_createBorderF1(planeNodeB2);
	planeNodeB2->translate( Ogre::Vector3( 0, 0, height/2 + border ));

	enttmp = sceneManager->createEntity( "planeEntB3", "planeNodeB3" );
	OSE_createBorderF1(planeNodeB3);
	planeNodeB3->translate( Ogre::Vector3( 0, border/2, height/2 + border/2 ));

	planeNodeL1->setVisible( true );
	planeNodeL2->setVisible( true );
	planeNodeL3->setVisible( true );
}


void _removeSubBorder( Ogre::SceneManager * sceneManager, Ogre::String nodeName, Ogre::String entName )
{
	sceneManager->getRootSceneNode()->getChild( nodeName )->removeAllChildren();
	sceneManager->getRootSceneNode()->removeChild( nodeName );
	sceneManager->destroySceneNode( nodeName );
	sceneManager->destroyManualObject( nodeName );
	sceneManager->destroyEntity( entName );
}

void OSE_destroyTheBorder( Ogre::SceneManager * sceneManager )
{
	_removeSubBorder( sceneManager, "planeNodeL1", "planeEntL1");
	_removeSubBorder( sceneManager, "planeNodeL2", "planeEntL2");
	_removeSubBorder( sceneManager, "planeNodeL3", "planeEntL3");
	_removeSubBorder( sceneManager, "planeNodeR1", "planeEntR1");
	_removeSubBorder( sceneManager, "planeNodeR2", "planeEntR2");
	_removeSubBorder( sceneManager, "planeNodeR3", "planeEntR3");
	_removeSubBorder( sceneManager, "planeNodeT1", "planeEntT1");
	_removeSubBorder( sceneManager, "planeNodeT2", "planeEntT2");
	_removeSubBorder( sceneManager, "planeNodeT3", "planeEntT3");
	_removeSubBorder( sceneManager, "planeNodeB1", "planeEntB1");
	_removeSubBorder( sceneManager, "planeNodeB2", "planeEntB2");
	_removeSubBorder( sceneManager, "planeNodeB3", "planeEntB3");
	objectsWereCreated = false;
}
