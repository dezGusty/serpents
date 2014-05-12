// Own header
#include <app/SerpFrameListener.h>

// Engine class has a fwd declaration in the header, so it must be included here.
#include <app/SerpEngine.h>

// Tracing
#include <guslib/trace/Trace.h>

namespace app
{
	SerpFrameListener::SerpFrameListener (SerpEngine *engine
											, Ogre::RenderWindow *win
											, bool bufferedKeys
											, bool bufferedMouse
											)
										: activeWindow_(true)
										, enginePtr_ (engine)
										, renderWindowPtr_ (win)
										, inputManagerPtr_ (0)
										, mousePtr_ (0)
										, keyboardPtr_ (0)
	{
		GTRACE (3, "SerpFrameListener CTor");

		using namespace OIS;

		ParamList params;
		size_t windowHnd = 0;
		std::ostringstream windowHndStr;

		win->getCustomAttribute ("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		params.insert (std::make_pair (std::string("WINDOW"), windowHndStr.str ()));

		// Insert additional properties to have the mouse as NON-exclusive (meaning one can move the mouse freely outside the window).
		params.insert (std::make_pair (std::string ("w32_mouse"), std::string ("DISCL_FOREGROUND")));
		params.insert (std::make_pair (std::string ("w32_mouse"), std::string ("DISCL_NONEXCLUSIVE")));
		params.insert (std::make_pair (std::string ("w32_keyboard"), std::string ("DISCL_FOREGROUND")));
		params.insert (std::make_pair (std::string ("w32_keyboard"), std::string ("DISCL_NONEXCLUSIVE"))); 

		inputManagerPtr_ = InputManager::createInputSystem (params);

		// Create all devices
		keyboardPtr_ = static_cast<Keyboard*> (inputManagerPtr_->createInputObject (OISKeyboard, bufferedKeys));
		mousePtr_ = static_cast<Mouse*> (inputManagerPtr_->createInputObject (OISMouse, bufferedMouse));

		//	Set initial mouse clipping size
		windowResized (renderWindowPtr_);
		keyboardPtr_->setBuffered (true);
		keyboardPtr_->setTextTranslation (OIS::Keyboard::Ascii);
		//GTRACE (4, "setting callback to engine for mouse and key events...");
		keyboardPtr_->setEventCallback (enginePtr_);

		mousePtr_->setBuffered (true);
		mousePtr_->setEventCallback (enginePtr_);
		//keyboardPtr_->setEventCallback (this);

		// Register as a Window listener
		Ogre::WindowEventUtilities::addWindowEventListener (renderWindowPtr_, this);
	}


	SerpFrameListener::~SerpFrameListener ()
	{
		// call the engine destruction if it is not yet destroyed.

		GTRACE (3, "SerpFrameListener destructor called...");
		if (enginePtr_) enginePtr_->releaseFrameListener ();
		if (enginePtr_) enginePtr_->shutDown ();
		if (enginePtr_) releaseEngine ();

		//Remove the current object as a Window listener
		if (renderWindowPtr_)
		{
			Ogre::WindowEventUtilities::removeWindowEventListener (renderWindowPtr_, this);
			windowClosed (renderWindowPtr_);
		}
		GTRACE(3, "SerpFrameListener destructor exiting...");
	}


	void SerpFrameListener::releaseEngine ()
	{ 
		GTRACE(4, "GBaseFrameListener received command to release engine. Setting ptr to engine to NULL. (Engine not deleted).");
		enginePtr_ = NULL;
	}


	void SerpFrameListener::windowResized (Ogre::RenderWindow* rw)
	{
		if (rw == renderWindowPtr_)
		{
			if (enginePtr_ == NULL)
			{
				//guslib::GScopedLock myLock (windowOperationsMutex);
				unsigned int width, height, depth;
				int left, top;
				rw->getMetrics(width, height, depth, left, top);
				const OIS::MouseState &ms = mousePtr_->getMouseState ();
				ms.width = width;
				ms.height = height;
			}
			else
			{
				//guslib::GScopedLock myLock(enginePtr_->getLoadAndRenderMutex());
				unsigned int width, height, depth;
				int left, top;
				rw->getMetrics (width, height, depth, left, top);
				const OIS::MouseState &ms = mousePtr_->getMouseState ();
				ms.width = width;
				ms.height = height;
			}
		}
	}


	void SerpFrameListener::windowClosed (Ogre::RenderWindow* rw)
	{
		GTRACE (3, "GBaseFrameListener windowClosed called...");
		//Only close for renderwindow
		if (rw && rw == renderWindowPtr_)
		{
			//guslib::GScopedLock myLock(windowOperationsMutex);
			if (enginePtr_ != NULL)
			{
				enginePtr_->shutDown ();
				GTRACE(3, "GBaseFrameListener sent shutdown cmd to engine...");
			}

			GUS_THREAD_SLEEP (100); // TODO: get from config at start-up.

			// The input devices should probably wait for a loop of the input reading thread to finish.

			//guslib::GScopedLock secondLock(inputObjectsAccessMutex);
			GTRACE (4, "destroying input manager and objects");
			if (inputManagerPtr_)
			{
				inputManagerPtr_->destroyInputObject (mousePtr_);
				inputManagerPtr_->destroyInputObject (keyboardPtr_);

				OIS::InputManager::destroyInputSystem (inputManagerPtr_);
				inputManagerPtr_ = NULL;
				keyboardPtr_ = NULL;
				mousePtr_ = NULL;
			}

			GTRACE (4, "finished destroying input manager and objects");
			renderWindowPtr_ = NULL;
		}
	}


	void SerpFrameListener::windowFocusChange (Ogre::RenderWindow* rw)
	{
		activeWindow_ = !activeWindow_;
		if (rw == renderWindowPtr_)
		{
			//guslib::GScopedLock myLock(windowOperationsMutex);
			enginePtr_->setWindowIsActive (activeWindow_);
		}
	}

	/**
		A "frame started rendering" callback. This is where most of the drawing is done, or should be done.
		Typically, called from SerpEngine -> renderCompleteScene
	*/
	bool SerpFrameListener::frameStarted (const Ogre::FrameEvent &evt)
	{
		GTRACE (5, "SerpFrameListener::frameStarted");
		if (! renderWindowPtr_)
			return false;

		if (renderWindowPtr_->isClosed ())	
			return false;

		//guslib::GScopedLock myLock(windowOperationsMutex);
		//if (!renderWindow)
		//	return false;

		bool result (true);

		if (enginePtr_)
		{
			// Call the scene rendering here.
			result = enginePtr_->renderScene (evt.timeSinceLastFrame);
		}

		GTRACE(5, "SerpFrameListener::frameStarted exit");
		return result;
	}

	bool SerpFrameListener::frameEnded (const Ogre::FrameEvent &evt)
	{
		GTRACE (5, "SerpFrameListener::frameEnded");
		//guslib::GScopedLock myLock(windowOperationsMutex);
		if (renderWindowPtr_ == NULL)
			return false;

		if( this->renderWindowPtr_->isClosed())	
			return false;

		bool result=true;
		if (enginePtr_ != NULL)
		{
			result = enginePtr_->renderGUI (evt.timeSinceLastFrame);
		}

		GTRACE(5, "SerpFrameListener::frameEnded exit");
		return result;
	}

}