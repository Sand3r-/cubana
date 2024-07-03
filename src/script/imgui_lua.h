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
#if defined __cplusplus
}
#endif

EXTERN int luaopen_imgui_lib(lua_State* L);