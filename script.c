#include "base.h"
#include "panic.h"
#include "script.h"
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <lua5.3/lauxlib.h>

static lua_State *s_State = NULL;
#define BASEDEFS_FILENAME "./res/lua/basedefs.lua"

/*
 * LuaTrace
 *	Equivalent to Trace() but for calling from Lua code.
 *	A bit silly to implement this by hand, really, but I'm using it as a
 *	learning opportunity, so I can get a feel for the interoperability
 *	between Lua and C.
 *
 *	Scoop up the args given and print them into a text buffer. Tempting
 *	to heap alloc it, but don't really want a malloc every time a script
 *	calls this, which will be a lot. So we have to take into account the
 *	size of the buffer everywhere, which is annoying, but it's for a
 *	good reason.
 *
 *	TODO: When I'm satisfied I've learned enough, this could just rack up
 *	its args and call Lua's built-in print function with a prepended str?
 *	Or even better, just implement it in Lua.
 */
// #define MAX_STR 1024
// static int LuaTrace(lua_State *L)
// {
// 	int argc = lua_gettop(L);
// 	char str[MAX_STR] = {0};
//
// 	for (int i = 1; i <= argc; i++) {
// 		Trace(Fmt("Handling %s", lua_typename(L, i)));
// 		switch (lua_type(L, i)) {
// 		case LUA_TSTRING: {
// 			size_t sz = strlen(lua_tostring(L, i));
// 			if (strlen(str) + sz >= MAX_STR) {
// 				Panic("Argument would overflow buffer");
// 			}
// 			strncat(str, lua_tostring(L, i), sz);
// 		} break;
// 		case LUA_TNUMBER: {
// 			/* lua will convert to strings for us, yay */
// 			const char *ns = lua_tostring(L, i);
// 			size_t sz = strlen(ns);
//
// 			if (strlen(str) + sz >= MAX_STR) {
// 				Panic("Argument would overflow buffer");
// 			}
// 			strncat(str, ns, sz);
// 			strncat(str, " ", 1);
// 		} break;
// 		default:
// 			Panic("u wan sum fuk?");
// 		}
// 	}
//
// 	Trace(Fmt("%s", str));
//
// 	return 0;
// }
// #undef MAX_STR

/*
 * Script_Init
 *	Initialise Lua and register some of our own core functionality.
 */
ecode_t Script_Init()
{
	if (s_State) {
		Panic("Script module already initialised");
	}

	s_State = luaL_newstate();
	if (!s_State) {
		Trace("Failed to create Lua state");
		return EFAIL;
	}

	luaL_openlibs(s_State);
	// lua_register(s_State, "Trace2", LuaTrace);

	/* Load and run bootstrap script.
	 * We defer to lua_error() when something goes wrong here,
	 * as it appears to be the only way to get at any kind of error code
	 * or message when something goes wrong.
	 */
	if (luaL_loadfile(s_State, BASEDEFS_FILENAME) != 0) {
		lua_error(s_State);
	}

	if (lua_pcall(s_State, 0, LUA_MULTRET, 0) != 0) {
		lua_error(s_State);
	}

	return EOK;
}

ecode_t Script_Shutdown()
{
	if (!s_State) {
		Panic("Script module not initialised");
	}

	lua_close(s_State);
	s_State = NULL;

	return EOK;
}

/*
 * Script_ExecString
 */
ecode_t Script_ExecString(const char *str)
{
	assert(str != NULL);

	if (!s_State) {
		Panic("Script module not initialised");
	}

	if (luaL_loadstring(s_State, str) != 0) {
		lua_error(s_State);
	}

	if (lua_pcall(s_State, 0, LUA_MULTRET, 0) != 0) {
		lua_error(s_State);
	}

	return EOK;
}

/*
 * Script_ExecFile
 */
ecode_t Script_ExecFile(FileHandle handle)
{
	const char *contents = (const char *) Files_GetData(handle);
	const char *path = Files_GetPath(handle);
	size_t sz = Files_GetSize(handle);

	/* We pass the path here so if something goes wrong, lua_error() will
	 * be able to tell us which file it was. */
	if (luaL_loadbuffer(s_State, contents, sz, path) != 0) {
		lua_error(s_State);
	}

	if (lua_pcall(s_State, 0, LUA_MULTRET, 0) != 0) {
		lua_error(s_State);
	}

	return EOK;
}
