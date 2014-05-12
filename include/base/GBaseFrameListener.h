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

#include <Ogre.h>
#include <OIS/OIS.h>

#include "BuildOpts.h"

#include "GBaseEngine.h"

namespace GBase
{
	// Forward class decls.
	class GBaseMainApp;
	class GBaseEngine;

	/// The base class for frame listeners in the base framework
	class GBaseFrameListener : public Ogre::FrameListener
							 , Ogre::WindowEventListener
	{
	public:
		///	Constructor.
		GBaseFrameListener( GBaseEngine *engine
						, Ogre::RenderWindow *win
						, bool bufferedKeys = false
						, bool bufferedMouse = false
						);

		///	Destructor.
		virtual ~GBaseFrameListener(void);

		///	Resize the window. Adjust mouse clipping area.
		virtual void windowResized(Ogre::RenderWindow *rw);

		///	Close the window. Unattach OIS before window shutdown (very important under Linux).
		virtual void windowClosed(Ogre::RenderWindow *rw);

		virtual void windowFocusChange(Ogre::RenderWindow* rw);

		///	The beginning of a frame. This is where most of the drawing is done, or should be done.
		virtual bool frameStarted(const Ogre::FrameEvent &evt);

		///	The ending part of a frame rendering. This is where the overlay items should be shown.
		virtual bool frameEnded(const Ogre::FrameEvent &evt);


		OIS::Keyboard *getOISKeyboard(){ return keyboard; }
		Ogre::RenderWindow *getRenderWindow(){ return renderWindow; }

		guslib::GMutex & getInputObjectsAccessMutex(){ return inputObjectsAccessMutex;}

		virtual void releaseEngine();
	protected:

		guslib::GMutex inputObjectsAccessMutex; // mouse and kb object access.
		guslib::GMutex windowOperationsMutex; // window close, render frame, etc.
		Ogre::RenderWindow *renderWindow;
		Ogre::SceneManager *sceneMgr;

		//	OIS Input devices

		OIS::InputManager* inputManager;
		OIS::Mouse*    mouse;
		OIS::Keyboard* keyboard;

		GBaseEngine * engine;

		Ogre::TextureFilterOptions mFiltering;
		int mAniso;

		bool activeWindow_;

	};

} // namespace GBase