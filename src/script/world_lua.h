#pragma once
#include "world.h"
#include "memory/arena.h"

typedef struct lua_State lua_State;

void LuaWorldSetCurrentWorld(World* world);
void LuaWorldSetCurrentArena(Arena* arena);
void RegisterWorldLuaFunctions(lua_State* L);