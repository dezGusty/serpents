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

#include <guslib/system/thread.h>
#include "GOgreRenderable.h"
#include <guslib/common/singleton.hpp>

namespace GBase
{
	class GResourceManagerUtil
	{
	protected:

		std::vector<Serpents::GOgreRenderable *> resToLoad;
		std::vector<Serpents::GOgreRenderable *> resToUnload;

		guslib::GMutex loadingMutex;

		Ogre::RaySceneQuery * raySceneQuery;

		virtual void doLoadCancelRequest( Serpents::GOgreRenderable * res );

		virtual void doLoadRequest( Serpents::GOgreRenderable * res );

		virtual void doUnloadRequest( Serpents::GOgreRenderable * res );

	public:
		GResourceManagerUtil();
		virtual ~GResourceManagerUtil();

		/// Public interface for resource load cancel command.
		void cancelLoadRequest( Serpents::GOgreRenderable * res );

		void cancelAllLoadRequests();

		/// Do the processing.
		virtual void processRequests();
		virtual void processLoadRequests();
		virtual void processUnloadRequests();

		/// Request the loading of a resource.
		void requestResourceLoad( Serpents::GOgreRenderable * res );

		/// Request the unloading of a resource.
		void requestResourceUnload( Serpents::GOgreRenderable * res );


		virtual void setRaySceneQuery( Ogre::RaySceneQuery * query );
	};

	typedef guslib::Singleton<GResourceManagerUtil> GResourceManager;
}