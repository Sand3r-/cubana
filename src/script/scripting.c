#include "scripting.h"
#include "culibc.h"
#include "error.h"
#include "event.h"
#include "log/log.h"
#include <lua.h>
#include <lauxlib.h>
#include "scriptreg.h"
#include "ui/scripteditor.h"
#include "world_lua.h"

#include "math/vec.h"
#include "math/mat.h"
#include <cim3d.h>

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
        ReportLuaError(error);
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

static void RegisterFunctions(lua_State* L)
{
    RegisterLuaFunctions(L);
    RegisterWorldLuaFunctions(L);
}

static void RegisterModulePaths(lua_State* L)
{
    lua_getglobal( L, "package" );
    lua_getfield( L, -1, "path" );
    const char* package_path = lua_tostring( L, -1 );
    char* cwd = cu_getcwd();
    char buffer[2048];
    // Here for development purposes I also append the path to source_dir/scripts
    cu_snprintf(
        buffer, 2048,
        "%s;%s../../scripts/core/cpml/?.lua;%sscripts/core/cpml/?.lua;"
        "%s../../scripts/core/?.lua;%sscripts/core/?.lua",
        package_path, cwd, cwd, cwd, cwd);
    cu_freecwdptr(cwd);
    lua_pop( L, 1 );
    lua_pushstring( L, buffer );
    lua_setfield( L, -2, "path" );
    lua_pop( L, 1 );
}

b32 ScriptEngineInit(void)
{
    L = luaL_newstate();
    luaL_openlibs(L);
    RegisterModulePaths(L);
    RegisterFunctions(L);
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

    lua_createtable(L, 0, 2);
    const int t = lua_gettop(L);
    lua_pushinteger(L, event.type);
    lua_setfield(L, t, "type");

    switch (event.type)
    {
        case EVENT_GAME_BEGIN:
        case EVENT_GAME_END:
            break;
        case EVENT_TICK:
            lua_pushnumber(L, event.event_tick.delta);
            lua_setfield(L, t, "dt");
            break;
    }

    lua_call(L, 1, 0);
}

b32 ScriptEngineShutdown(void)
{
    if (L) lua_close(L);
    return true;
}
