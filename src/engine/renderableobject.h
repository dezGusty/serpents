#pragma once

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
//    A renderable object's relevant information.

//
// Includes
//

//
// C++ system headers
//

// none

//
// Other libraries' headers
//
#include "OgrePrerequisites.h"
#include "OgreVector3.h"

//
// This project's headers
//

#include "engine/renderableobjectinterface.h"

namespace Serpents
{
  class RenderableObject
    :public RenderableObjectInterface
  {
  protected:
    Ogre::Vector3 dynamicRotationVector;
    Ogre::Vector3 dynamicScalingVector;
    Ogre::Vector3 rotationVector;
    Ogre::Vector3 scalingVector;
    Ogre::Vector3 masterTranslationVector;
    Ogre::Vector3 translationVector;

    Ogre::String sceneNodeName_;

    static long internalCounter;
    long creationIndex;

    /**
      Constructor.
    */
    explicit RenderableObject(Ogre::SceneManager* aSceneManager);

    /**
      Destructor.
    */
    virtual ~RenderableObject();

  public:
    virtual void setRotationVector(Ogre::Vector3 vector);
    virtual Ogre::Vector3 getRotationVector();
    virtual void setScalingVector(Ogre::Vector3 vector);
    virtual Ogre::Vector3 getScalingVector();
    virtual void setTranslationVector(Ogre::Vector3 vector);
    virtual Ogre::Vector3 getTranslationVector();

    virtual Ogre::String getSceneNodeName();

    virtual void setDynamicRotationVector(Ogre::Vector3 vector);
    virtual Ogre::Vector3 getDynamicRotationVector();
    virtual void setDynamicScalingVector(Ogre::Vector3 vector);
    virtual Ogre::Vector3 getDynamicScalingVector();
  };
}
