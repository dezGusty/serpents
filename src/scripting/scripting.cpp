//
// Includes
//

// Own header
#include "scripting/scripting.h"

// Allow delays
#include <guslib/thirdparty/tinythread.h>

// Allow tracing
#include <guslib/trace/trace.h>

// Additional includes based on the functionality being exposed by the scripting.
#ifdef SERPENTS_LUA
// TODO: add other header files that contain functionality available via scripting.

#endif

namespace Serpents
{
	// Inititialzie the static members.
#ifdef SERPENTS_LUA
	lua_State* Scripting::m_lua = 0;
#endif

	//
	// Register the scripting.
	//
	void Scripting::initialize ()
	{
#ifdef SERPENTS_LUA
		// create the lua state
		m_lua = luaL_newstate ();
		luaL_openlibs (m_lua);

		// register the functions.
		lua_register (m_lua, "writeLogMessage", Serpents::l_writeLogMessage);
		lua_register (m_lua, "sleepCurrentThreadInMillis", Serpents::l_sleepCurrentThreadInMillis);
		

#endif
	}



	//
	// Register the scripting.
	//
	void Scripting::terminate ()
	{
#ifdef SERPENTS_LUA
		// create the lua state
		if (0 != m_lua)
		{
			lua_close (m_lua);
			m_lua = 0;
		}
#endif
	}

	//
	// Setters for variables for scripting
	//
	void Scripting::setVar (std::string varName, int value)
	{
#ifdef SERPENTS_LUA
		lua_pushinteger (m_lua, value);
		lua_setglobal (m_lua, varName.c_str());
#endif
	}
	

	//
	// Setters for variables for scripting
	//
	void Scripting::setVar (std::string varName, float value)
	{
#ifdef SERPENTS_LUA
		lua_pushnumber (m_lua, value);
		lua_setglobal (m_lua, varName.c_str());
#endif
	}

	
	//
	// Setters for variables for scripting
	//
	void Scripting::setVar (std::string varName, std::string value)
	{
#ifdef SERPENTS_LUA
		lua_pushstring (m_lua, value.c_str());
		lua_setglobal (m_lua, varName.c_str());
#endif
	}

	//
	// Run a script
	//
	void Scripting::runScript (const std::string& scriptName)
	{
#ifdef SERPENTS_LUA
		int errorNum = luaL_dofile (m_lua, scriptName.c_str ());
		if (errorNum)
		{

		}
#endif
	}



#ifdef SERPENTS_LUA

	//
	// Generate some time based id.
	//
	int l_StringIdentifier_generateTimeBasedID (lua_State* L)
	{
		std::string newTimeBasedId = std::string (""); //::StringIdentifier::generateTimeBasedID ();
		lua_pushstring (L, newTimeBasedId.c_str ());

		// lua-return code must be amount of pushed results
		return 1;
	}



	//
	// Initialize the factory from Lua.
	//
	int l_InitializeFactories (lua_State* L)
	{
		// TODO: add some factories.
		// lua-return code must be amount of pushed results
		return 0;
	}



	//
	// Write something to the log.
	//
	int l_writeLogMessage (lua_State* L)
	{
		std::string logMsg (luaL_checkstring (L, 1));
		// lua-return code must be amount of pushed results
		return 0;
	}


	
	//
	// Make the current thread sleep.
	//
	int l_sleepCurrentThreadInMillis (lua_State* L)
	{
		int sleepValueInMillis (luaL_checkinteger (L, 1));
		tthread::this_thread::sleep_for (tthread::chrono::milliseconds (sleepValueInMillis));

		// lua-return code must be amount of pushed results
		return 0;
	}




#endif

}