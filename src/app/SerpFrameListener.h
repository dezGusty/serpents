#pragma once

// Forwards the inclusion of ogre.h
#include <config/serpentsogre.h>
#include <OIS.h>

namespace app
{
	//
	//    Forward class declarations.
	//
	class SerpEngine;


	/// The base class for frame listeners in the base framework
	class SerpFrameListener 
		: public Ogre::FrameListener
		, public Ogre::WindowEventListener
	{
	protected:
		//guslib::GMutex inputObjectsAccessMutex; // mouse and kb object access.
		//guslib::GMutex windowOperationsMutex; // window close, render frame, etc.
		Ogre::RenderWindow* renderWindowPtr_;
		
		//	OIS Input devices

		OIS::InputManager* inputManagerPtr_;
		OIS::Mouse*    mousePtr_;
		OIS::Keyboard* keyboardPtr_;

		SerpEngine * enginePtr_;

		bool activeWindow_;

	public:
		///	Constructor.
		SerpFrameListener(
        SerpEngine *engine,
        Ogre::RenderWindow *win,
        bool bufferedKeys = false,
        bool bufferedMouse = false);

		///	Destructor.
		virtual ~SerpFrameListener(void);


		/**
			Resize the window. Adjust mouse clipping area.
		*/
		virtual void windowResized(Ogre::RenderWindow *rw);


		/**
			Close the window. Unattach OIS before window shutdown (very important under Linux).
		*/
		virtual void windowClosed(Ogre::RenderWindow *rw);


		/**
			React to the window losing the focus.
			(You could trigger non-rendering functions here, such as pausing or resuming the music. Keep in mind synchronization issues!)
		*/
		virtual void windowFocusChange(Ogre::RenderWindow* rw);


		/**
			A "frame started rendering" callback. This is where most of the drawing is done, or should be done.
			Typically, called from SerpEngine -> renderCompleteScene
		*/
		virtual bool frameStarted(const Ogre::FrameEvent &evt);


		/**
			The ending part of a frame rendering. This is where the overlay items should be shown.
		*/
		virtual bool frameEnded(const Ogre::FrameEvent &evt);

		/// Getter for the OIS Keyboard.
		OIS::Keyboard* getOISKeyboard()
    {
      return keyboardPtr_;
    }

		/// Getter for the OIS Mouse.
		OIS::Mouse* getOISMouse()
    {
      return mousePtr_;
    }

		/// Getter for the render window.
		Ogre::RenderWindow* getRenderWindow()
    {
      return renderWindowPtr_;
    }

		//guslib::GMutex& getInputObjectsAccessMutex(){ return inputObjectsAccessMutex;}

		virtual void releaseEngine();
	};
}
