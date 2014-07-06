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

//
// Includes
//

//
// Own header.
//
#include "engine/renderableobject.h"

//
// C++ system headers
//

// none

//
// Other libraries' headers.
//

// none

//
// This project's headers.
//

// none

namespace Serpents
{
  /**
    Constructor.
  */
  RenderableObject::RenderableObject(Ogre::SceneManager * aSceneManager)
  {
  }


  /**
    Destructor.
  */
  RenderableObject::~RenderableObject()
  {
  }

  void RenderableObject::setRotationVector(Ogre::Vector3 vector)
  {
    this->rotationVector = vector;
  }

  Ogre::Vector3 RenderableObject::getRotationVector()
  {
    return rotationVector;
  }

  void RenderableObject::setScalingVector(Ogre::Vector3 vector)
  {
    this->scalingVector = vector;
  }

  Ogre::Vector3 RenderableObject::getScalingVector()
  {
    return scalingVector;
  }

  void RenderableObject::setTranslationVector(Ogre::Vector3 vector)
  {
    this->translationVector = vector;
  }

  Ogre::Vector3 RenderableObject::getTranslationVector()
  {
    return translationVector;
  }

  Ogre::String RenderableObject::getSceneNodeName()
  {
    return sceneNodeName_;
  }

  void RenderableObject::setDynamicRotationVector(Ogre::Vector3 vector)
  {
    this->dynamicRotationVector = vector;
  }

  Ogre::Vector3 RenderableObject::getDynamicRotationVector()
  {
    return this->dynamicRotationVector;
  }

  void RenderableObject::setDynamicScalingVector(Ogre::Vector3 vector)
  {
    this->dynamicScalingVector = vector;
  }

  Ogre::Vector3 RenderableObject::getDynamicScalingVector()
  {
    return this->dynamicScalingVector;
  }
}
