#pragma once
#include "world.h"

typedef struct lua_State lua_State;

void LuaWorldSetCurrentWorld(World* world);
void RegisterWorldLuaFunctions(lua_State* L);