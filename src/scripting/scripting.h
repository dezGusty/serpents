#pragma once

//
//	Includes
//

// Allow the use of std::string
#include <string>

#ifdef SERPENTS_LUA
// Add the link libraries
#pragma comment(lib,"lua5.1.lib")
// The header
#include "lua.hpp"
#endif

namespace Serpents
{
	class Scripting
	{
	private:
#ifdef SERPENTS_LUA
		static lua_State* m_lua;
#endif

	public:

		//
		// Register the scripting.
		//
		static void initialize ();

		//
		// Cleanup
		//
		static void terminate ();

		//
		// Run a script
		//
		static void runScript (const std::string& scriptName);

		//
		// Setters for variables for scripting
		//
		static void setVar (std::string varName, int value);
		
		//
		// Setters for variables for scripting
		//
		static void setVar (std::string varName, float value);
		
		//
		// Setters for variables for scripting
		//
		static void setVar (std::string varName, std::string value);

#ifdef SERPENTS_LUA
		static lua_State* getLuaState () { return m_lua; }
#endif

	};

#ifdef SERPENTS_LUA
	//
	// Get some parameters of an operation (via Lua).
	//
	static int l_StringIdentifier_generateTimeBasedID (lua_State* L);

	//
	// Initialize some factories.
	//
	static int l_InitializeFactories (lua_State* L);
	
	//
	// Write a message to the log.
	//
	static int l_writeLogMessage (lua_State* L);
	
	//
	// Sleep the current thread.
	//
	static int l_sleepCurrentThreadInMillis (lua_State* L);


#endif

}