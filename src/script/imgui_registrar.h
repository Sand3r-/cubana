#pragma once

#if defined __cplusplus
    #define EXTERN extern "C"
#else
    #define EXTERN extern
#endif

#if defined __cplusplus
extern "C" {
#endif
  #include "lua.h"
  #include "lualib.h"
  #include "lauxlib.h"
#if defined __cplusplus
}
#endif

EXTERN lua_State* g_ImguiBindingsState;
EXTERN void LoadImguiBindings();