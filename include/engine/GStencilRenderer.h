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
//    Listener for stencil operations.
//    This is copied from the Ogre tutorial on stencil glow.

#include <OgreRenderQueueListener.h> 
#include <OgreRenderQueue.h>
#include <Ogre.h>
#include <vector>

using namespace Ogre;

// render queues
#define FIRST_STENCIL_OP_RENDER_QUEUE		RENDER_QUEUE_MAIN + 1
#define RENDER_QUEUE_OUTLINE_GLOW_OBJECTS	RENDER_QUEUE_MAIN + 1
#define RENDER_QUEUE_OUTLINE_GLOW_GLOWS		RENDER_QUEUE_MAIN + 2
#define LAST_STENCIL_OP_RENDER_QUEUE		RENDER_QUEUE_MAIN + 2

// stencil values
#define STENCIL_VALUE_FOR_OUTLINE_GLOW 1
#define STENCIL_FULL_MASK 0xFFFFFFFF

namespace Serpents
{
	struct RenderQueue
	{
		Ogre::uint8 queueID_;
		Ogre::CompareFunction func_;
		Ogre::StencilOperation stencilFailOp_;
		Ogre::StencilOperation depthFailOp_;
		Ogre::StencilOperation passOp_;
		int value_;

		RenderQueue()
		{
			queueID_ = RENDER_QUEUE_MAIN;
			func_ = Ogre::CMPF_ALWAYS_PASS;
			stencilFailOp_ = Ogre::SOP_KEEP;
			depthFailOp_ = Ogre::SOP_KEEP;
			passOp_ = Ogre::SOP_KEEP;
			value_ = 1;
		}
	};
	typedef std::vector<RenderQueue> RenderQueueList;

	// a Render queue listener to change the stencil mode
	class StencilOpQueueListener : public Ogre::RenderQueueListener 
	{ 
	private:
		bool stopped;
		bool started;
		Ogre::uint8 firstStencilOpRenderQueue;
		Ogre::uint8 lastStencilOpRenderQueue;
		RenderQueueList renderQueueList;
	public:
		StencilOpQueueListener();

		virtual void addRenderQueue(Ogre::uint8 queueID, Ogre::CompareFunction func, Ogre::StencilOperation stencilFailOp
			, Ogre::StencilOperation depthFailOp, Ogre::StencilOperation passOp, int value );

		virtual void renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& skipThisInvocation) ;
		
		virtual void renderQueueEnded(Ogre::uint8 queueGroupId, const Ogre::String& invocation, bool& repeatThisInvocation) ;

		virtual void setFirstStencilOpRenderQueue(Ogre::uint8 value){ firstStencilOpRenderQueue = value; }

		virtual void setLastStencilOpRenderQueue(Ogre::uint8 value){ lastStencilOpRenderQueue = value; }

	}; 

}//namespace