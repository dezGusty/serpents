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
//    stencil effects renderer.

#include "GStencilRenderer.h"

namespace Serpents
{
	StencilOpQueueListener::StencilOpQueueListener()
		:stopped(false)
		,started(false)
	{
	}
	void StencilOpQueueListener::addRenderQueue(Ogre::uint8 queueID, Ogre::CompareFunction func, Ogre::StencilOperation stencilFailOp
			, Ogre::StencilOperation depthFailOp, Ogre::StencilOperation passOp, int value )
	{
		RenderQueue rq;
		rq.queueID_ = queueID;
		rq.func_ = func;
		rq.stencilFailOp_ = stencilFailOp;
		rq.depthFailOp_ = depthFailOp;
		rq.passOp_ = passOp;
		rq.value_ = value;

		renderQueueList.push_back(rq);
	}

	void StencilOpQueueListener::renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& skipThisInvocation)
	{
		if( queueGroupId >= firstStencilOpRenderQueue
			&& queueGroupId <= lastStencilOpRenderQueue )
		{
			Ogre::RenderSystem * rendersys = Ogre::Root::getSingleton().getRenderSystem();
			rendersys->setStencilCheckEnabled(true);
			rendersys->clearFrameBuffer(Ogre::FBT_STENCIL);
			rendersys->setStencilBufferParams();
			started = true;
			stopped = false;
		}

		for( RenderQueueList::iterator it = renderQueueList.begin(); it != renderQueueList.end(); ++it )
		{
			if( it->queueID_ == queueGroupId )
			{
				Ogre::RenderSystem * rendersys = Ogre::Root::getSingleton().getRenderSystem();
				rendersys->setStencilCheckEnabled(true);
				rendersys->setStencilBufferParams(it->func_,
					it->value_, STENCIL_FULL_MASK,
					it->stencilFailOp_, it->depthFailOp_, it->passOp_, false);
			}
		}

		skipThisInvocation = false;
	}


	void StencilOpQueueListener::renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation)
	{
		if ( queueGroupId >= LAST_STENCIL_OP_RENDER_QUEUE )
		{
			if( started )
			{
				Ogre::RenderSystem * rendersys = Ogre::Root::getSingleton().getRenderSystem();
				rendersys->setStencilCheckEnabled(false);
				rendersys->setStencilBufferParams();
				stopped = true;
				started = false;
			}
		}
	}

}
