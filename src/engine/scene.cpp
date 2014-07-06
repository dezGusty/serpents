//   This file is part of Gusty's Serpents, licensed under the terms of the MIT License
//   (further described below).
//
//   Copyright (C) 2009-2014  Augustin Preda (thegusty999@gmail.com)
//
//   Permission is hereby granted, free of charge, to any person obtaining a copy
//   of this software and associated documentation files (the "Software"), to deal
//   in the Software without restriction, including without limitation the rights
//   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//   copies of the Software, and to permit persons to whom the Software is
//   furnished to do so, subject to the following conditions:
//
//   The above copyright notice and this permission notice shall be included in
//   all copies or substantial portions of the Software.
//
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//   THE SOFTWARE.
//
//    Last change:  $LastChangedDate$
//    Revision:    $Revision$

//
// Includes
//

//
// Own header.
//
#include "engine/scene.h"

//
// C++ system headers
//

// none

//
// Other libraries' headers.
//

#include "guslib/trace/trace.h"

// Ogre includes
#include "OgreBillboardSet.h"
#include "OgreEntity.h"
#include "OgreManualObject.h"
#include "OgreMeshManager.h"
#include "OgreSceneManager.h"

//
// This project's headers.
//

#include "app/serpentsengine.h"

namespace Serpents
{
  //
  //  ------------------------------------------- Scene Internals -----------------------------------------------------
  //

  /**
    Opaque pointer: private implementation of scene.
  */
  class Scene::Impl
  {
  public:
    app::SerpEngine* enginePtr_;

    Scene::Impl(app::SerpEngine* ptrToEngine)
      : enginePtr_(ptrToEngine)
    {
    }
  };

  //
  //  ---------------------------------------------- Scene ---------------------------------------------------
  //

  /**
    Constructor.
  */
  Scene::Scene(app::SerpEngine* ptrToEngine)
    : impl_(new Scene::Impl(ptrToEngine))
  {
  }

  /**
    Destructor.
  */
  Scene::~Scene()
  {
    delete impl_;
  }

  /**
    Allow the specification of skybox details.
    This allows the user to turn the skybox on or off, as well as setting the material to use
    (for the 6 facets of the skybox), distance
  */
  void Scene::setSkyBoxData(
      bool enabled,
      const char* materialName,
      double distanceToSkyBox,
      bool drawFirst,
      const char* resourceGroupName)
  {
    impl_->enginePtr_->getSceneManagerPtr()->setSkyBox(
      enabled,
      materialName,
      distanceToSkyBox,
      drawFirst,
      Ogre::Quaternion::IDENTITY,
      resourceGroupName);
  }

  /**
    Create scene objects from a given configuration.
    @param objectConfiguration A simple configuration object.
  */
  void Scene::createObjectFromConfig(guslib::PropertyGroup& groupEntry, const std::string& resourceGroupName)
  {
    int nType = groupEntry["type"].getAsInt();
    double posX = groupEntry["position.x"].getAsDouble();
    double posY = groupEntry["position.y"].getAsDouble();
    double posZ = groupEntry["position.z"].getAsDouble();

    double sizeWidth = groupEntry["size.x"].getAsDouble();
    double sizeHeight = groupEntry["size.z"].getAsDouble();
    int segsWidth = groupEntry["seg.x"].getAsIntOrDefaultVal(1);
    int segsHeight = groupEntry["seg.z"].getAsIntOrDefaultVal(1);

    std::string materialName = groupEntry["material"].getAsString();
    int renderGroup = groupEntry["renderGroup"].getAsIntOrDefaultVal(0);

    bool castsShadows = groupEntry["castShadows"].getAsBoolOrDefault(true);
    std::string itemName(groupEntry.getName());
    GTRACE(4, "Item named " << itemName << " will " <<(castsShadows?"":"NOT ") << "cast shadows.");

    double scaleX = groupEntry["scale.x"].getAsDoubleOrDefaultVal(1.0);
    double scaleY = groupEntry["scale.y"].getAsDoubleOrDefaultVal(1.0);
    double scaleZ = groupEntry["scale.z"].getAsDoubleOrDefaultVal(1.0);


    switch (nType)
    {
      case 1:
        // mesh
        {
          std::string meshName = groupEntry["mesh"].getAsString();

          if (meshName != "")
          {
            // create the entity, node and attach them to the scene.
            Ogre::Entity* eTemp;
            Ogre::MeshPtr ptrMesh;
            ptrMesh = Ogre::MeshManager::getSingletonPtr()->load(meshName, resourceGroupName);

            bool buildTangents;
            buildTangents = groupEntry["buildTangentVectors"].getAsBoolOrDefault(false);
            if (buildTangents)
            {
              unsigned short src, dest;
              if (false == ptrMesh-> suggestTangentVectorBuildParams(Ogre::VES_TANGENT, src, dest))
              {
                ptrMesh->buildTangentVectors(Ogre::VES_TANGENT, src, dest);
              }
            }

            eTemp = this->impl_->enginePtr_->getSceneManagerPtr()->createEntity(itemName, meshName);
            std::string tempNodeName = itemName;
            tempNodeName.append("node");
            Ogre::SceneNode *snTemp =
                this->impl_->enginePtr_->getSceneManagerPtr()->getRootSceneNode()->createChildSceneNode(tempNodeName);

            snTemp->attachObject(eTemp);

            snTemp->setPosition(posX, posY, posZ);
            snTemp->scale(scaleX, scaleY, scaleZ);

            if (renderGroup!= 0)
            {
              eTemp->setRenderQueueGroup(renderGroup);
            }
            if (materialName != "")
            {
              eTemp->setMaterialName(materialName);
            }
            eTemp->setCastShadows(castsShadows);
            GTRACE(4, "Created entity for mesh object and applied shadow cast " << castsShadows);
          }
        }
        break;
      case 0:
      default:
        {
          // plane type
          std::string planeName(itemName);
          planeName.append("plane");

          int planeNormalIdx = groupEntry["normal"].getAsIntOrDefaultVal(0);
          Ogre::Vector3 planeNormalVector = Ogre::Vector3::UNIT_X;
          switch (planeNormalIdx)
          {
          case -3:
            planeNormalVector = Ogre::Vector3::NEGATIVE_UNIT_Z;
            break;
          case -2:
            planeNormalVector = Ogre::Vector3::NEGATIVE_UNIT_Y;
            break;
          case -1:
            planeNormalVector = Ogre::Vector3::NEGATIVE_UNIT_X;
            break;
          case 1:
            planeNormalVector = Ogre::Vector3::UNIT_X;
            break;
          case 2:
            planeNormalVector = Ogre::Vector3::UNIT_Y;
            break;
          case 3:
          default:
            planeNormalVector = Ogre::Vector3::UNIT_Z;
            break;
          }

          int planeUpIdx = groupEntry["up"].getAsIntOrDefaultVal(0);
          Ogre::Vector3 planeUpVector = Ogre::Vector3::UNIT_X;
          switch (planeUpIdx)
          {
          case -3:
            planeUpVector = Ogre::Vector3::NEGATIVE_UNIT_Z;
            break;
          case -2:
            planeUpVector = Ogre::Vector3::NEGATIVE_UNIT_Y;
            break;
          case -1:
            planeUpVector = Ogre::Vector3::NEGATIVE_UNIT_X;
            break;
          case 1:
            planeUpVector = Ogre::Vector3::UNIT_X;
            break;
          case 2:
            planeUpVector = Ogre::Vector3::UNIT_Y;
            break;
          case 3:
          default:
            planeUpVector = Ogre::Vector3::UNIT_Z;
            break;
          }

          GTRACE(4, "Planar object " << itemName << " will use normal vector : " << planeNormalVector);
          GTRACE(4, "Planar object " << itemName << " will use up vector : " << planeUpVector);

          Ogre::MovablePlane* myPlane = new Ogre::MovablePlane(planeName);
          myPlane->normal = planeNormalVector;
          myPlane->d = posY;
#if 0
          // TODO(Augustin Preda, 2014.07.04): this plane may be used in light calculation.
          // Test whether a different scene integration may be needed.
          if (mPlane == 0)
          {
            mPlane = myPlane;
          }
#endif
          std::string planeMeshName(itemName);
          planeMeshName.append("mesh");
          Ogre::MeshManager::getSingleton().createPlane(
              planeMeshName,
              resourceGroupName, *myPlane,
              sizeWidth, sizeHeight,
              segsWidth, segsHeight,
              true, 1, 1, 1, planeUpVector);

          GTRACE(4, "Created plane mesh " << planeMeshName << " size " << sizeWidth << "x" << sizeHeight
            << "; segments " << segsWidth << "x" << segsHeight);
          // create the entity, node and attach them to the scene.
          Ogre::Entity * eTemp;
          eTemp = this->impl_->enginePtr_->getSceneManagerPtr()->createEntity(itemName, planeMeshName);
          std::string tempNodeName = itemName;
          tempNodeName.append("node");
          Ogre::SceneNode *snTemp =
              this->impl_->enginePtr_->getSceneManagerPtr()->getRootSceneNode()->createChildSceneNode(tempNodeName);
          snTemp->attachObject(eTemp);

          snTemp->translate(posX, 0, posZ);
          if (renderGroup!= 0)
          {
            eTemp->setRenderQueueGroup(renderGroup);
          }
          if (materialName != "")
          {
#if 0
            Ogre::MaterialPtr matPtr = Ogre::MaterialManager::getSingletonPtr()->getByName(materialName);
            matPtr->setReceiveShadows(!castsShadows);
            eTemp->setMaterial(matPtr);
#else
            eTemp->setMaterialName(materialName);
#endif
          }

          eTemp->setCastShadows(castsShadows);
          GTRACE(4, "Created entity for plane and applied shadow cast " << castsShadows);
        }
        break;
    }
  }

  /**
    Create a light configuration from a base configuration entry.
    @param groupEntry The configuration for the light, based on a property group.
    @param createVisibleLightInformation Specify to create additional items to help the user see the light source.
    @param resourceGroupName The resource group to use for creating additional items.
  */
  void Scene::createLightFromConfig(
      guslib::PropertyGroup& groupEntry,
      bool createVisibleLightInformation,
      const std::string& resourceGroupName)
  {
    std::string lightName(groupEntry.getName());
    double x, y, z;

    std::string sLightName = groupEntry["name"].getAsStringOrDefaultVal("");
    if (sLightName.length() > 0)
    {
      Ogre::Light * lightObject;
      GTRACE(4, "Creating light for scene: " << sLightName);
      lightObject = this->impl_->enginePtr_->getSceneManagerPtr()->createLight(sLightName);

      this->applyLightSettingsFromCfg(lightObject, groupEntry);
      
      double initPosX = groupEntry["position.x"].getAsDouble();
      double initPosY = groupEntry["position.y"].getAsDouble();
      double initPosZ = groupEntry["position.z"].getAsDouble();

      x = groupEntry["lookAt.x"].getAsDouble();
      y = groupEntry["lookAt.y"].getAsDouble();
      z = groupEntry["lookAt.z"].getAsDouble();

      int lightType = groupEntry["type"].getAsInt();

      Ogre::ManualObject* myManualObject = NULL;
      if (createVisibleLightInformation)
      {
        if (lightType != 0)
        {
          std::string lineName = lightName;
          lineName.append(".line");
          std::string lineMaterial = lineName;
          lineMaterial.append(".material");
          myManualObject =  this->impl_->enginePtr_->getSceneManagerPtr()->createManualObject(lineName);
          Ogre::MaterialPtr myManualObjectMaterial =
              Ogre::MaterialManager::getSingleton().create(lineMaterial, resourceGroupName);
          myManualObjectMaterial->setReceiveShadows(false);
          myManualObjectMaterial->getTechnique(0)->setLightingEnabled(true);
          myManualObjectMaterial->getTechnique(0)->getPass(0)->setDiffuse(0, 0, 1, 0);
          myManualObjectMaterial->getTechnique(0)->getPass(0)->setAmbient(0, 0, 1);
          myManualObjectMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(0, 0, 1);
          myManualObject->begin(lineMaterial, Ogre::RenderOperation::OT_LINE_LIST);
          myManualObject->position(initPosX, initPosY, initPosZ);
          myManualObject->position(x, y, z);
          GTRACE(4, "Added light debug line from(" << initPosX << ", " << initPosY << ", " << initPosZ << ") to ("
            << x << ", " << y << ", " << z << ")");
          // etc
          myManualObject->end();
        }      
      
        Ogre::BillboardSet* bbs = 
            this->impl_->enginePtr_->getSceneManagerPtr()->createBillboardSet(lightName + std::string("bbs"), 1);
        Ogre::ColourValue myCol(0.95f, 0.95f, 1.0f);
        bbs->createBillboard(Ogre::Vector3::ZERO, myCol);
        bbs->setMaterialName("Workbench/Flare");
        bbs->setCastShadows(false);
      
        std::string nodeName = lightName;
        nodeName.append("node");
        Ogre::SceneNode * myNode =
            this->impl_->enginePtr_->getSceneManagerPtr()->getRootSceneNode()->createChildSceneNode(nodeName);

        // Light attaching to node DISABLED.
        //myNode->attachObject(lightObject);
        if (lightType != 0 && myManualObject != NULL)
        {
          myNode->attachObject(myManualObject);
        }

        std::string anonName = nodeName;
        anonName.append(".anonymous");
        Ogre::SceneNode * anonNode =
            this->impl_->enginePtr_->getSceneManagerPtr()->getRootSceneNode()->createChildSceneNode(anonName);

        anonNode->attachObject(bbs);
        anonNode->scale(0.2f, 0.2f, 0.2f);

        anonNode->translate(initPosX, initPosY, initPosZ);
        //myNode->addChild(anonNode);

        GTRACE(4, "Moved node " << nodeName);
      }
    }
  }

  
  /**
    Apply the light settings to a light entry.
    @param lightObject The light object to which settings are applied.
    @param lightCfgEntry The configuration entry.
  */
  void Scene::applyLightSettingsFromCfg(Ogre::Light* lightObject, guslib::PropertyGroup& lightCfgEntry)
  {
    double x, y, z;
    std::string itemName(lightCfgEntry.getName());
    std::string sLightName = lightCfgEntry["name"].getAsStringOrDefaultVal("");

    if (NULL == lightObject)
    {
      return;
    }

    int lightType = lightCfgEntry["type"].getAsInt();
    x = lightCfgEntry["lookAt.x"].getAsDouble();
    y = lightCfgEntry["lookAt.y"].getAsDouble();
    z = lightCfgEntry["lookAt.z"].getAsDouble();


    double spotX = lightCfgEntry["range.x"].getAsDoubleOrDefaultVal(0.0f);
    double spotY = lightCfgEntry["range.y"].getAsDoubleOrDefaultVal(0.0f);

    double initPosX = lightCfgEntry["position.x"].getAsDouble();
    double initPosY = lightCfgEntry["position.y"].getAsDouble();
    double initPosZ = lightCfgEntry["position.z"].getAsDouble();

    Ogre::Vector3 posVector(initPosX, initPosY, initPosZ);
    Ogre::Vector3 dirVector(x, y, z);

    lightObject->setPosition(initPosX, initPosY, initPosZ);

    switch (lightType)
    {
    case 0:
      lightObject->setType(Ogre::Light::LT_POINT);
      break;
    case 1:
      lightObject->setType(Ogre::Light::LT_DIRECTIONAL);
      {
        Ogre::Vector3 diffVector = dirVector - posVector;
        diffVector.normalise();
        lightObject->setDirection(diffVector);
        GTRACE(4, "Setting direction for light [" << itemName << "]: " << dirVector);
      }
      break;
    case 2:
      lightObject->setType(Ogre::Light::LT_SPOTLIGHT);
      {
        Ogre::Vector3 diffVector = dirVector - posVector;
        diffVector.normalise();
        lightObject->setDirection(diffVector);
        GTRACE(4, "Setting direction for light [" << itemName << "]: " << dirVector);
      }

      lightObject->setSpotlightRange(Ogre::Degree(spotX), Ogre::Degree(spotY));
      break;
    }

    int nCastShadows = lightCfgEntry["castShadows"].getAsInt();
    lightObject->setCastShadows(nCastShadows != 0);

    x = lightCfgEntry["diffuseColor.x"].getAsDouble();
    y = lightCfgEntry["diffuseColor.y"].getAsDouble();
    z = lightCfgEntry["diffuseColor.z"].getAsDouble();
    lightObject->setDiffuseColour(x, y, z);

    x = lightCfgEntry["specularColor.x"].getAsDouble();
    y = lightCfgEntry["specularColor.y"].getAsDouble();
    z = lightCfgEntry["specularColor.z"].getAsDouble();
    lightObject->setSpecularColour(x, y, z);
  }

}
