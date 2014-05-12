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

#include "GResourceManager.h"
#include <guslib/trace/trace.h>

using namespace Serpents;
namespace GBase
{
	GResourceManagerUtil::GResourceManagerUtil()
		: raySceneQuery( NULL )
	{
	}


	GResourceManagerUtil::~GResourceManagerUtil()
	{
	}


	void GResourceManagerUtil::cancelLoadRequest( GOgreRenderable * res )
	{
		GTRACE(6, "cancelLoadRequest getting mutex (loadingMutex)");
		guslib::GScopedLock myLock(loadingMutex);
		doLoadCancelRequest( res );
	}

	void GResourceManagerUtil::cancelAllLoadRequests()
	{
		GTRACE(6, "cancelAllLoadRequests getting mutex (loadingMutex)");
		guslib::GScopedLock myLock(loadingMutex);
		resToLoad.clear();
	}


	void GResourceManagerUtil::doLoadCancelRequest( GOgreRenderable * res )
	{
		GTRACE(5, "Cancelling load request...");
		if( !res )
		{
			GTRACE(2, "WARNING: tried to cancel request for NULL ptr!");
			return;
		}
		for( std::vector<GOgreRenderable *>::iterator it = resToLoad.begin();
			it != resToLoad.end(); ++it )
		{
			if( (*it) == res )
			{
				resToLoad.erase( it );
				GTRACE(4, "Cancelled load request.");
				return;
			}
		}
		GTRACE(4, "No match found to cancel load request.");
	}

	void GResourceManagerUtil::doLoadRequest( GOgreRenderable * res )
	{
		if( !res )
			return;
		GTRACE(5, "Requesting resource load... "<<res->getMeshName()<<", "<<res->getMaterialName());

		resToLoad.push_back( res );
		GTRACE(4, "Requested resource load...");
	}

	void GResourceManagerUtil::doUnloadRequest( GOgreRenderable * res )
	{
		if( !res )
			return;
		GTRACE(5, "Requesting resource unload... "<<res->getMeshName()<<", "<<res->getMaterialName());

		resToUnload.push_back( res );

		GTRACE(4, "Requested resource unload...");
	}



	void GResourceManagerUtil::processLoadRequests()
	{
		GTRACE(6, "processLoadRequests getting mutex (loadingMutex)");
		guslib::GScopedLock myLock(loadingMutex);
		GTRACE(5, "Processing load requests...");
		for( std::vector<GOgreRenderable*>::iterator it = resToLoad.begin(); 
			it != resToLoad.end(); ++it )
		{
			if( (*it)->getMeshName() != ""  )
			{
				GTRACE(4, "Calling load for "<<(*it)->getMeshName());
				(*it)->loadObject( (*it)->getMeshName(), (*it)->getMaterialName() );
				(*it)->setRaySceneQuery( raySceneQuery );

				GTRACE(4, "Called load for "<<(*it)->getMeshName());
			}
			else
			{
				GTRACE(2, "WARNING! Received load request for mesh=["<<(*it)->getMeshName()<<"]; ignoring");
			}
		}
		resToLoad.clear();
		GTRACE(5, "Processed load requests...");
	}


	void GResourceManagerUtil::processUnloadRequests()
	{
		GTRACE(6, "processUnloadRequests getting mutex (loadingMutex)");
		guslib::GScopedLock myLock(loadingMutex);
		GTRACE(5, "Processing unload requests...");
		for( std::vector<GOgreRenderable*>::iterator it = resToUnload.begin(); 
			it != resToUnload.end(); ++it )
		{
			(*it)->unload();

			delete *it;
		}
		resToUnload.clear();
		GTRACE(5, "Processed unload requests...");
	}



	void GResourceManagerUtil::processRequests()
	{
		processUnloadRequests();
		processLoadRequests();
	}


	void GResourceManagerUtil::requestResourceLoad( GOgreRenderable * res )
	{
		GTRACE(6, "requestResourceLoad getting mutex (loadingMutex)");
		guslib::GScopedLock myLock(loadingMutex);
		doLoadRequest( res );
	}

	void GResourceManagerUtil::requestResourceUnload( GOgreRenderable * res )
	{
		GTRACE(6, "requestResourceUnload getting mutex (loadingMutex)");
		guslib::GScopedLock myLock(loadingMutex);
		doUnloadRequest( res );
	}


	void GResourceManagerUtil::setRaySceneQuery( Ogre::RaySceneQuery * query )
	{
		raySceneQuery = query;
	}
}