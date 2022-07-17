#include "scripting.h"
#include "event.h"
#include "log/log.h"
#include <lua.h>
#include <lauxlib.h>

static lua_State* L;

static b32 HandleError(int error_code)
{
    if (error_code)
    {
        const char* error = lua_tostring(L, -1);
        L_ERROR("Lua error: %s", error);
    }
    return error_code;
}

static b32 InitEventHandler(void)
{
    // load script that implements the event handling logic
}

b32 ScriptEngineInit(void)
{
    L = luaL_newstate();
    InitEventHandler();
    return true;
}

void ExecuteString(const char* string)
{
    HandleError(luaL_dostring(L, string));
}

void ExecuteFile(const char* path)
{
    HandleError(luaL_dofile(L, path));
}

void ScriptEngineProcessEvent(Event event)
{
    // Process event
}

b32 ScriptEngineShutdown(void)
{
    if (L) lua_close(L);
    return true;
}
