#include "scripting.h"
#include "error.h"
#include "event.h"
#include "log/log.h"
#include <lua.h>
#include <lauxlib.h>

static lua_State* L;

static void DEBUG_PrintStack(lua_State *L)
{
    L_DEBUG("Lua stack begin:");
    L_DEBUG("%-5s%-10s%s", "id", "type", "value");
    int top = lua_gettop(L);
    for (int i = 1; i <= top; i++) {
        switch (lua_type(L, i)) {
        case LUA_TNUMBER:
            L_DEBUG("%-5d%-10s%g", i, luaL_typename(L,i), lua_tonumber(L,i));
            break;
        case LUA_TSTRING:
            L_DEBUG("%-5d%-10s%s", i, luaL_typename(L,i), lua_tostring(L,i));
            break;
        case LUA_TBOOLEAN:
            L_DEBUG("%-5d%-10s%s", i, luaL_typename(L,i), (lua_toboolean(L, i) ? "true" : "false"));
            break;
        case LUA_TNIL:
            L_DEBUG("%-5d%-10s%s", i, luaL_typename(L,i), "nil");
            break;
        default:
            L_DEBUG("%-5d%-10s%p", i, luaL_typename(L,i), lua_topointer(L,i));
            break;
        }
    }
    L_DEBUG("Lua stack end.");
}

static b32 HandleError(int error_code)
{
    if (error_code)
    {
        const char* error = lua_tostring(L, -1);
        L_ERROR("Lua error: %s", error);
    }
    return error_code;
}

static void RegisterEvents(void)
{
    for (int event_id = 0; event_id < EVENT_TYPE_MAX; event_id++)
    {
        lua_getglobal(L, "_register_event");
        lua_pushinteger(L, event_id);
        lua_pushstring(L, EventTypeToStr(event_id));
        lua_call(L, 2, 0);
    }
}

static b32 EnsureEventFunctionExist(const char* name)
{
    int does_function_exist = lua_getglobal(L, name) == LUA_TFUNCTION;
    lua_pop(L, 1);
    if (!does_function_exist)
    {
        L_ERROR("%s isn't a function or wasn't registered", name);
        return false;
    }
    return true;
}

static b32 EnsureEventFunctionsExist(void)
{
    if (EnsureEventFunctionExist("_register_event") &&
        EnsureEventFunctionExist("_handle_event") &&
        EnsureEventFunctionExist("register"))
        return CU_SUCCESS;
    return CU_FAILURE;
}

static b32 InitEventHandler(void)
{
    HandleError(luaL_dofile(L, "scripts/core/events.lua"));
    if (EnsureEventFunctionsExist() != CU_SUCCESS) return CU_FAILURE;
    RegisterEvents();
    return CU_SUCCESS;
}

b32 ScriptEngineInit(void)
{
    L = luaL_newstate();
    luaL_openlibs(L);
    InitEventHandler();
    return CU_SUCCESS;
}

void ExecuteScript(const char* string)
{
    HandleError(luaL_dostring(L, string));
}

void ExecuteScriptFile(const char* path)
{
    HandleError(luaL_dofile(L, path));
}

void ScriptEngineProcessEvent(Event event)
{
    lua_getglobal(L, "_handle_event");
    lua_pushinteger(L, event.type);
    lua_call(L, 1, 0);
}

b32 ScriptEngineShutdown(void)
{
    if (L) lua_close(L);
    return true;
}
