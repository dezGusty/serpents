

#include "VisualSerpentsEngine.h"


namespace Serpents
{
	VisualSerpentsEngine::VisualSerpentsEngine(Ogre::Root * root, GusFrameListener * listener )
		: GusEngine( root, listener )
	{
	}

	VisualSerpentsEngine::~VisualSerpentsEngine()
	{
	}


	bool VisualSerpentsEngine::mainLoopBeforeRender()
	{
		if( !GusEngine::mainLoopBeforeRender() )
		{
			return false;
		}

		

		GTIMEUNITS timeUnits = timer.renew();
		return true;
	}

}