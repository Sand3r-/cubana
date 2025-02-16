#include "world_lua.h"
#include <lua.h>
#include <lauxlib.h>

static World* g_World = NULL;

void LuaWorldSetCurrentWorld(World* world)
{
    g_World = world;
}

static v3 lua_to_vec3(lua_State* L, int index)
{
    luaL_checktype(L, index, LUA_TTABLE);
    lua_getfield(L, index, "x");
    lua_getfield(L, index, "y");
    lua_getfield(L, index, "z");
    v3 ret = {
        .x = lua_tonumber(L, -3),
        .y = lua_tonumber(L, -2),
        .z = lua_tonumber(L, -1)
    };
    lua_pop(L, 3);
    return ret;
}

static void lua_push_vec3(lua_State* L, v3 v)
{
    lua_createtable(L, 0, 3);
    const int t = lua_gettop(L);
    lua_pushnumber(L, v.x);
    lua_setfield(L, t, "x");
    lua_pushnumber(L, v.y);
    lua_setfield(L, t, "y");
    lua_pushnumber(L, v.z);
    lua_setfield(L, t, "z");
}

static u32 GetEntityId(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_getfield(L, 1, "id");
    u32 entity_id = luaL_checkinteger(L, -1);
    lua_pop(L, 1);
    return entity_id;
}

static int Lua_KillEntity(lua_State* L)
{
    u32 entity_id = GetEntityId(L);
    WorldKillEntity(g_World, entity_id);
    return 0;
}
static int Lua_EntityMove(lua_State* L)
{
    u32 entity_id = GetEntityId(L);
    v3 velocity = lua_to_vec3(L, 2);
    WorldEntityMove(g_World, entity_id, velocity);
    return 0;
}
static int Lua_EntitySetPosition(lua_State* L)
{
    u32 entity_id = GetEntityId(L);
    v3 position = lua_to_vec3(L, 2);
    WorldEntitySetPosition(g_World, entity_id, position);
    return 0;
}
static int Lua_EntitySetDimensions(lua_State* L)
{
    u32 entity_id = GetEntityId(L);
    v3 dimensions = lua_to_vec3(L, 2);
    WorldEntitySetDimensions(g_World, entity_id, dimensions);
    return 0;
}
static int Lua_EntitySetColour(lua_State* L)
{
    u32 entity_id = GetEntityId(L);
    v3 colour = lua_to_vec3(L, 2);
    WorldEntitySetColour(g_World, entity_id, colour);
    return 0;
}
static int Lua_EntitySetFlags(lua_State* L)
{
    u32 entity_id = GetEntityId(L);
    u32 flags = luaL_checkinteger(L, 2);
    WorldEntitySetFlags(g_World, entity_id, flags);
    return 0;
}
static int Lua_EntityGetPosition(lua_State* L)
{
    u32 entity_id = GetEntityId(L);
    v3 position = WorldEntityGetPosition(g_World, entity_id);
    lua_push_vec3(L, position);
    return 1;
}
static int Lua_EntityGetDimensions(lua_State* L)
{
    u32 entity_id = GetEntityId(L);
    v3 dimensions = WorldEntityGetDimensions(g_World, entity_id);
    lua_push_vec3(L, dimensions);
    return 1;
}
static int Lua_EntityGetColour(lua_State* L)
{
    u32 entity_id = GetEntityId(L);
    v3 colour = WorldEntityGetColour(g_World, entity_id);
    lua_push_vec3(L, colour);
    return 1;
}
static int Lua_EntityGetFlags(lua_State* L)
{
    u32 entity_id = GetEntityId(L);
    u32 flags = WorldEntityGetFlags(g_World, entity_id);
    lua_pushinteger(L, flags);
    return 1;
}

static int Lua_EntitySpawn(lua_State* L)
{
    u32 flags = luaL_checkinteger(L, 1);
    v3 pos = lua_to_vec3(L, 2);
    v3 dim = lua_to_vec3(L, 3);
    v3 colour = lua_to_vec3(L, 4);

    u32 entity_id = WorldSpawnEntity(g_World, flags, pos, dim, colour);

    lua_newtable(L); // Create a new table
    lua_pushinteger(L, entity_id);
    lua_setfield(L, -2, "id"); // entity.id = entity_id

    // Attach methods
    lua_pushcfunction(L, Lua_EntityMove);
    lua_setfield(L, -2, "Move");

    lua_pushcfunction(L, Lua_EntitySetPosition);
    lua_setfield(L, -2, "SetPosition");

    lua_pushcfunction(L, Lua_EntitySetDimensions);
    lua_setfield(L, -2, "SetDimensions");

    lua_pushcfunction(L, Lua_EntitySetColour);
    lua_setfield(L, -2, "SetColour");

    lua_pushcfunction(L, Lua_EntitySetFlags);
    lua_setfield(L, -2, "SetFlags");

    lua_pushcfunction(L, Lua_EntityGetPosition);
    lua_setfield(L, -2, "GetPosition");

    lua_pushcfunction(L, Lua_EntityGetDimensions);
    lua_setfield(L, -2, "GetDimensions");

    lua_pushcfunction(L, Lua_EntityGetColour);
    lua_setfield(L, -2, "GetColour");

    lua_pushcfunction(L, Lua_EntityGetFlags);
    lua_setfield(L, -2, "GetFlags");

    return 1; // Table is now on the stack
}

void RegisterEntityFlags(lua_State* L)
{
    lua_pushinteger(L, ENTITY_NONE);
    lua_setglobal(L, "ENTITY_NONE");

    lua_pushinteger(L, ENTITY_CAMERA_BIT);
    lua_setglobal(L, "ENTITY_CAMERA_BIT");

    lua_pushinteger(L, ENTITY_STATIC_BIT);
    lua_setglobal(L, "ENTITY_STATIC_BIT");

    lua_pushinteger(L, ENTITY_TRIGGER_BIT);
    lua_setglobal(L, "ENTITY_TRIGGER_BIT");

    lua_pushinteger(L, ENTITY_PLAYER_BIT);
    lua_setglobal(L, "ENTITY_PLAYER_BIT");

    lua_pushinteger(L, ENTITY_ENEMY_BIT);
    lua_setglobal(L, "ENTITY_ENEMY_BIT");

    lua_pushinteger(L, ENTITY_COLLIDES_BIT);
    lua_setglobal(L, "ENTITY_COLLIDES_BIT");

    lua_pushinteger(L, ENTITY_VISIBLE_BIT);
    lua_setglobal(L, "ENTITY_VISIBLE_BIT");

    lua_pushinteger(L, ENTITY_GRAVITY_BIT);
    lua_setglobal(L, "ENTITY_GRAVITY_BIT");
}


void RegisterWorldLuaFunctions(lua_State* L)
{
    RegisterEntityFlags(L);
    lua_register(L, "SpawnEntity", Lua_EntitySpawn);
}
