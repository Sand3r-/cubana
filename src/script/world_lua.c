#include "world_lua.h"
#include <lua.h>
#include <lauxlib.h>
#include "math/qat.h"
#include "log/log.h"

static World* g_World = NULL;
static Arena* g_Arena = NULL;

void LuaWorldSetCurrentWorld(World* world)
{
    g_World = world;
}

void LuaWorldSetCurrentArena(Arena* arena)
{
    g_Arena = arena;
}

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

static q4 lua_to_q4(lua_State* L, int index)
{
    luaL_checktype(L, index, LUA_TTABLE);
    lua_getfield(L, index - 0, "x");
    lua_getfield(L, index - 1, "y");
    lua_getfield(L, index - 2, "z");
    lua_getfield(L, index - 3, "w");
    q4 ret = {
        .x = lua_tonumber(L, -4),
        .y = lua_tonumber(L, -3),
        .z = lua_tonumber(L, -2),
        .w = lua_tonumber(L, -1)
    };
    lua_pop(L, 4);
    return ret;
}

static v3 lua_to_v3(lua_State* L, int index)
{
    luaL_checktype(L, index, LUA_TTABLE);
    lua_getfield(L, index - 0, "x");
    lua_getfield(L, index - 1, "y");
    lua_getfield(L, index - 2, "z");
    v3 ret = {
        .x = lua_tonumber(L, -3),
        .y = lua_tonumber(L, -2),
        .z = lua_tonumber(L, -1)
    };
    lua_pop(L, 3);
    return ret;
}

static void lua_push_quat(lua_State* L, q4 q)
{
    lua_createtable(L, 0, 4);
    const int t = lua_gettop(L);
    lua_pushnumber(L, q.x);
    lua_setfield(L, t, "x");
    lua_pushnumber(L, q.y);
    lua_setfield(L, t, "y");
    lua_pushnumber(L, q.z);
    lua_setfield(L, t, "z");
    lua_pushnumber(L, q.w);
    lua_setfield(L, t, "w");
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

static const char* lua_getfield_str(lua_State* L, int index, const char* key, const char* def) {
    lua_getfield(L, index, key);
    const char* ret = lua_isnil(L, -1) ? def : luaL_checkstring(L, -1);
    lua_pop(L, 1);
    return ret;
}

static i64 lua_getfield_int(lua_State* L, int index, const char* key, i64 def) {
    lua_getfield(L, index, key);
    i64 ret = lua_isnil(L, -1) ? def : luaL_checkinteger(L, -1);
    lua_pop(L, 1);
    return ret;
}

static v3 lua_getfield_v3(lua_State* L, int index, const char* key, v3 def) {
    lua_getfield(L, index, key);
    v3 ret = lua_isnil(L, -1) ? def : lua_to_v3(L, -1);
    lua_pop(L, 1);
    return ret;
}

static q4 lua_getfield_q4(lua_State* L, int index, const char* key, q4 def) {
    lua_getfield(L, index, key);
    q4 ret = lua_isnil(L, -1) ? def : lua_to_q4(L, -1);
    lua_pop(L, 1);
    return ret;
}

static Entity lua_to_entity(lua_State* L, int index)
{
    luaL_checktype(L, index, LUA_TTABLE);
    Entity ret = {
        .name = lua_getfield_str(L, -1, "name", "Unnamed"),
        .flags = lua_getfield_int(L, -1, "flags", ENTITY_NONE),
        .position = lua_getfield_v3(L, -1, "position", v3(0.0f)),
        .rotation = lua_getfield_q4(L, -1, "rotation", Q4Identity()),
        .dimensions = lua_getfield_v3(L, -1, "dimensions", v3(0.0f)),
        .colour = lua_getfield_v3(L, -1, "colour", v3(0.5f)),
        .velocity = lua_getfield_v3(L, -1, "velocity", v3(0.0f))
    };
    return ret;
}

static u32 GetEntityId(lua_State* L, int index)
{
    luaL_checktype(L, index, LUA_TTABLE);
    lua_getfield(L, index, "id");
    u32 entity_id = luaL_checkinteger(L, -1);
    lua_pop(L, 1); // Pop id from the stack
    return entity_id;
}

static int Lua_KillEntity(lua_State* L)
{
    u32 entity_id = GetEntityId(L, -1);
    WorldKillEntity(g_World, entity_id);
    return 0;
}

static int Lua_EntityAddChild(lua_State* L)
{
    u32 parent_id = GetEntityId(L, -2);
    u32 child_id = GetEntityId(L, -1);
    WorldEntityAddChild(g_Arena, g_World, parent_id, child_id);
    return 0;
}

static int Lua_EntityMove(lua_State* L)
{
    u32 entity_id = GetEntityId(L, -2);
    v3 velocity = lua_to_v3(L, -1);
    WorldEntityMove(g_World, entity_id, velocity);
    return 0;
}

static int Lua_EntitySetName(lua_State* L)
{
    u32 entity_id = GetEntityId(L, -2);
    const char* name = luaL_checkstring(L, -1);
    WorldEntitySetName(g_World, entity_id, name);
    return 0;
}

static int Lua_EntitySetPosition(lua_State* L)
{
    u32 entity_id = GetEntityId(L, -2);
    v3 position = lua_to_v3(L, -1);
    WorldEntitySetPosition(g_World, entity_id, position);
    return 0;
}

static int Lua_EntitySetRotation(lua_State* L)
{
    u32 entity_id = GetEntityId(L, -2);
    q4 rotation = lua_to_q4(L, -1);
    WorldEntitySetRotation(g_World, entity_id, rotation);
    return 0;
}

static int Lua_EntitySetDimensions(lua_State* L)
{
    u32 entity_id = GetEntityId(L, -2);
    v3 dimensions = lua_to_v3(L, -1);
    WorldEntitySetDimensions(g_World, entity_id, dimensions);
    return 0;
}
static int Lua_EntitySetColour(lua_State* L)
{
    u32 entity_id = GetEntityId(L, -2);
    v3 colour = lua_to_v3(L, -1);
    WorldEntitySetColour(g_World, entity_id, colour);
    return 0;
}
static int Lua_EntitySetFlags(lua_State* L)
{
    u32 entity_id = GetEntityId(L, -2);
    u32 flags = luaL_checkinteger(L, -1);
    WorldEntitySetFlags(g_World, entity_id, flags);
    return 0;
}

static int Lua_EntityGetName(lua_State* L)
{
    u32 entity_id = GetEntityId(L, -1);
    const char* name = WorldEntityGetName(g_World, entity_id);
    lua_pushstring(L, name);
    return 1;
}

static int Lua_EntityGetPosition(lua_State* L)
{
    u32 entity_id = GetEntityId(L, -1);
    v3 position = WorldEntityGetPosition(g_World, entity_id);
    lua_push_vec3(L, position);
    return 1;
}


static int Lua_EntityGetRotation(lua_State* L)
{
    u32 entity_id = GetEntityId(L, -1);
    q4 rotation = WorldEntityGetRotation(g_World, entity_id);
    lua_push_quat(L, rotation);
    return 1;
}

static int Lua_EntityGetDimensions(lua_State* L)
{
    u32 entity_id = GetEntityId(L, -1);
    v3 dimensions = WorldEntityGetDimensions(g_World, entity_id);
    lua_push_vec3(L, dimensions);
    return 1;
}
static int Lua_EntityGetColour(lua_State* L)
{
    u32 entity_id = GetEntityId(L, -1);
    v3 colour = WorldEntityGetColour(g_World, entity_id);
    lua_push_vec3(L, colour);
    return 1;
}
static int Lua_EntityGetFlags(lua_State* L)
{
    u32 entity_id = GetEntityId(L, -1);
    u32 flags = WorldEntityGetFlags(g_World, entity_id);
    lua_pushinteger(L, flags);
    return 1;
}

static int Lua_EntitySpawn(lua_State* L)
{
    Entity e = lua_to_entity(L, -1);
    u32 entity_id = WorldSpawnEntity(g_World, e);

    lua_newtable(L); // Create a new table
    lua_pushinteger(L, entity_id);
    lua_setfield(L, -2, "id"); // entity.id = entity_id

    // Attach methods
    lua_pushcfunction(L, Lua_EntityMove);
    lua_setfield(L, -2, "Move");

    lua_pushcfunction(L, Lua_EntityAddChild);
    lua_setfield(L, -2, "AddChild");

    lua_pushcfunction(L, Lua_EntitySetName);
    lua_setfield(L, -2, "SetName");

    lua_pushcfunction(L, Lua_EntitySetPosition);
    lua_setfield(L, -2, "SetPosition");

    lua_pushcfunction(L, Lua_EntitySetRotation);
    lua_setfield(L, -2, "SetRotation");

    lua_pushcfunction(L, Lua_EntitySetDimensions);
    lua_setfield(L, -2, "SetDimensions");

    lua_pushcfunction(L, Lua_EntitySetColour);
    lua_setfield(L, -2, "SetColour");

    lua_pushcfunction(L, Lua_EntitySetFlags);
    lua_setfield(L, -2, "SetFlags");

    lua_pushcfunction(L, Lua_EntityGetName);
    lua_setfield(L, -2, "GetName");

    lua_pushcfunction(L, Lua_EntityGetPosition);
    lua_setfield(L, -2, "GetPosition");

    lua_pushcfunction(L, Lua_EntityGetRotation);
    lua_setfield(L, -2, "GetRotation");

    lua_pushcfunction(L, Lua_EntityGetDimensions);
    lua_setfield(L, -2, "GetDimensions");

    lua_pushcfunction(L, Lua_EntityGetColour);
    lua_setfield(L, -2, "GetColour");

    lua_pushcfunction(L, Lua_EntityGetFlags);
    lua_setfield(L, -2, "GetFlags");

    return 1; // Table is now on the stack
}

static int Lua_WorldSetCamera(lua_State* L)
{
    u32 entity_id = GetEntityId(L, -1);
    WorldSetCamera(g_World, entity_id);
    return 0;
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
    lua_register(L, "SetWorldCamera", Lua_WorldSetCamera);
}
