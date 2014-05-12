#ifndef _APP_SERP_STATE_H
#define _APP_SERP_STATE_H

//   This file is part of Gusty's Serpents, licensed under the terms of the MIT License
//   (further described below).
//
//   Copyright (C) 2009-2012  Augustin Preda (thegusty999@gmail.com)
//
//   Permission is hereby granted, free of charge, to any person obtaining a copy
//   of this software and associated documentation files (the "Software"), to deal
//   in the Software without restriction, including without limitation the rights
//   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//   copies of the Software, and to permit persons to whom the Software is
//   furnished to do so, subject to the following conditions:
//   
//   The above copyright notice and this permission notice shall be included in
//   all copies or substantial portions of the Software.
//   
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//   THE SOFTWARE.
//
//    Keeps track of the application's states
//	 Last change:	$LastChangedDate$
//	 Revision:		$Revision$

// std::string
#include <string>
// std::map
#include <map>
// std::stack
#include <stack>

// open input system keys.
#include <OIS.h>

// using the singleton
#include <guslib/common/singleton.hpp>

// using the Configuration for the optional parameter send op.
#include <guslib/util/Configuration.h>

namespace app
{
	// fwd class defs.
	class SerpEngine;

	/**
		Parameter class/structure that can be passed to a state.
	*/
	class SerpStateParams
	{
	protected:
		guslib::Configuration cfg_;

	public:
		SerpStateParams ();
		SerpStateParams (guslib::Configuration& rhs);
		guslib::Configuration& getConfig () { return cfg_; }
		const guslib::Configuration& getConstConfig () const { return cfg_; }
	};


	/**
		A basic state class. Different application states should specialize extending this state.
	*/
	class SerpState
	{
	protected:
		std::string name_;

		SerpState (const std::string& name, SerpEngine* ptr = 0)
			: name_ (name)
			, enginePtr_ (ptr) 
		{ }

		SerpEngine* enginePtr_;
	public:
		// Handle state initialization.
		virtual void enter (const SerpStateParams& param) = 0;

		// Handle state control release.
		virtual void exit () = 0;
		
		// Suspend the current state. Should most likely be resumed later on.
		virtual void pause () = 0;

		// Resume a suspended state.
		virtual void resume () = 0;

		// Handle input
		virtual void reactToKeyPress (OIS::KeyCode keycode) = 0;
		virtual void reactToKeyRelease (OIS::KeyCode keycode) = 0;

		virtual void reactToMouseMoved (int x, int y) = 0;
		virtual void reactToMousePressed (const OIS::MouseButtonID& btn, int x, int y) = 0;
		virtual void reactToMouseReleased (const OIS::MouseButtonID& btn, int x, int y) = 0;

		//
		virtual bool handleLogicThreadLoop () = 0;

		// Render the 3D scene.
		virtual bool renderScene () = 0;

		// Render the GUI.
		virtual bool renderGUI () = 0;

		virtual void notifyOfShutdown () = 0;

		virtual const std::string& getName () const { return name_; }
	};

	/**
		Listener for the state manager
	*/
	class SerpStateManagerListener
	{
	public:
		virtual void OnStateStackEmpty () = 0;
	};



	typedef std::map <std::string, SerpState*> SerpStateMap;
	typedef std::stack <std::string> SerpStateStack; // this is the state stack


	/**
		Class that handles the game's possible states.
	*/
	class SerpStateManagerUtil
	{
	private:
		//	the internal states vector. States can changed as well as pushed and popped.
		SerpStateMap states_;
		SerpStateStack stateStack_;

		SerpStateManagerListener * listenerPtr_;

		std::string nextSetState_;
		std::string nextPushState_;

		int nextPopState_;
		SerpStateParams paramsForNextState_;

		void internalPushState (const std::string& state, const SerpStateParams& params);
		/**
			Set the state to use. This is an internal function that doesn't use any locking.
			Don't call or make available externally.
		*/
		void internalSetState (const std::string& state, const SerpStateParams& params);
		void internalPopState ();
	public:
		SerpStateManagerUtil ();

		virtual ~SerpStateManagerUtil ();

		virtual bool addState (const SerpState * state);

		virtual void setListener (const SerpStateManagerListener * aListener);

        virtual void setStateNextTick (const std::string& state, const SerpStateParams& params);
		virtual void pushStateNextTick (const std::string& state, const SerpStateParams& params);
		virtual void popStateNextTick ();


		virtual void reactToKeyPress (const OIS::KeyCode& keycode);
		virtual void reactToKeyRelease (const OIS::KeyCode& keycode);

		virtual void reactToMouseMoved (int x, int y);
		virtual void reactToMousePressed (const OIS::MouseButtonID& btn, int x, int y);
		virtual void reactToMouseReleased (const OIS::MouseButtonID& btn, int x, int y);

		virtual bool handleLogicThreadLoop();

		virtual bool renderScene();
		virtual bool renderGUI();

		virtual void setShutdown();
	};

	typedef guslib::Singleton<app::SerpStateManagerUtil> SerpStateManager;

}

#endif