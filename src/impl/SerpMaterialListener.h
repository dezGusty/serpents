#pragma once

// Forwards the inclusion of ogre.h
#include <config/serpentsogre.h>

#include <OgreMaterialManager.h>

class MaterialListener :
    public Ogre::MaterialManager::Listener
{
    protected:
        Ogre::MaterialPtr mBlackMat;
    public:
        MaterialListener();

        Ogre::Technique *handleSchemeNotFound(unsigned short, const Ogre::String& schemeName,
                Ogre::Material*, unsigned short, const Ogre::Renderable*);
};
