#include "GRenderable.h"

namespace Serpents
{
	// static
	long GMovableRenderable::internalCounter = 0;

	GMovableRenderable::GMovableRenderable( Ogre::SceneManager * aSceneManager )
		: animRotationVector( Ogre::Vector3::ZERO )
		, rotationVector( Ogre::Vector3::ZERO )
		, scalingVector( Ogre::Vector3::UNIT_SCALE )
		, animScalingVector( Ogre::Vector3::UNIT_SCALE )
		, masterTranslationVector( Ogre::Vector3::ZERO )
		, translationVector( Ogre::Vector3::ZERO )
		, sceneManager_ (aSceneManager)
		, sceneNodeName_( "" )
	{
		// make sure that the internal counter which will be used for the names will be able
		// to create a different name for each different renderable object.
		creationIndex = ++ internalCounter;
	}

}
