#include "scriptreg.h"
#include <lua.h>
#include <lauxlib.h>
#include <cim3d.h>
#include <cimgui.h>
#include <assert.h>
#include "culibc.h"
#include "imgui_registrar.h"
#include "log/log.h"

#define JOIN_STR2(X, Y) X ## Y
#define WRAP_NAME(NAME) JOIN_STR2(L_, NAME)

static v2 lua_tovec2(lua_State* L, int index, void* null)
{
    luaL_checktype(L, index, LUA_TTABLE);
    lua_getfield(L, index, "x");
    lua_getfield(L, index, "y");
    v2 ret = {
        .x = lua_tonumber(L, -2),
        .y = lua_tonumber(L, -1),
    };
    lua_pop(L, 2);
    return ret;
}

static v3 lua_tovec3(lua_State* L, int index, void* null)
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

static m4 lua_tom4(lua_State* L, int index, void* null)
{
    m4 ret;
    luaL_checktype(L, index, LUA_TTABLE);
    for (int i = 0; i < 16; i++)
    {
        lua_rawgeti(L, index, i + 1);
        ret.raw[i] = lua_tonumber(L, -1);
        lua_pop(L, 1);
    }

    return ret;
}

static m4* lua_tom4p(lua_State* L, int index, void* null)
{
    static m4 ret;
    ret = lua_tom4(L, index, null);
    return &ret;
}

static int* lua_tointegerxp(lua_State* L, int index, void* null)
{
    static int integer;
    integer = lua_tointegerx(L, index, null);
    return &integer;
}

static float* lua_tonumberxp(lua_State* L, int index, void* null)
{
    static float number;
    number = lua_tonumberx(L, index, null);
    return &number;
}

static bool* lua_tobooleanp(lua_State* L, int index, void* null)
{
    static bool boolean;
    boolean = lua_toboolean(L, index);
    return &boolean;
}

// Fetch lua function argument given stack L, and index IDX to that variable.
// Save result to X
#define FETCH_ARG(L, X, IDX) X = _Generic((X), \
    f32:         lua_tonumberx, \
    f32*:        lua_tonumberxp, \
    f64:         lua_tonumberx, \
    u8:          lua_tointegerx, \
    u8*:         lua_tointegerxp, \
    u16:         lua_tointegerx, \
    u16*:        lua_tointegerxp, \
    u32:         lua_tointegerx, \
    u32*:        lua_tointegerxp, \
    u64:         lua_tointegerx, \
    u64*:        lua_tointegerxp, \
    i8:          lua_tointegerx, \
    i8*:         lua_tointegerxp, \
    i16:         lua_tointegerx, \
    i16*:         lua_tointegerxp, \
    i32:         lua_tointegerx, \
    i32*:        lua_tointegerxp, \
    i64:         lua_tointegerx, \
    i64*:        lua_tointegerxp, \
    bool*:       lua_tobooleanp, \
    bool:        lua_tointegerx, \
    const char*: lua_tolstring, \
    v2:          lua_tovec2, \
    v3:          lua_tovec3, \
    m4:          lua_tom4, \
    m4*:         lua_tom4p \
    )(L, IDX, NULL)

static void lua_pushvec2(lua_State* L, v2 v)
{
    lua_createtable(L, 0, 2);
    const int t = lua_gettop(L);
    lua_pushnumber(L, v.x);
    lua_setfield(L, t, "x");
    lua_pushnumber(L, v.y);
    lua_setfield(L, t, "y");
}

static void lua_pushvec3(lua_State* L, v3 v)
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

static void lua_pushm4(lua_State* L, m4 m)
{
    lua_createtable(L, 0, 16);
    const int t = lua_gettop(L);
    for (int i = 0; i < 16; i++)
    {
        lua_pushnumber(L, m.raw[i]);
        lua_rawseti(L, t, i + 1);
    }
}

static void lua_pushm4p(lua_State* L, m4* m)
{
    lua_createtable(L, 0, 16);
    const int t = lua_gettop(L);
    for (int i = 0; i < 16; i++)
    {
        lua_pushnumber(L, m->raw[i]);
        lua_rawseti(L, t, i + 1);
    }
}

static void lua_pushbooleanp(lua_State* L, bool* b)
{
    lua_pushboolean(L, *b);
}

static void lua_pushintegerp(lua_State* L, int* v)
{
    lua_pushinteger(L, *v);
}

static void lua_pushnumberp(lua_State* L, float* v)
{
    lua_pushnumber(L, *v);
}

// Define X as a return value, given stack L
#define RET_VAL(L, X) _Generic((X), \
    f32:         lua_pushnumber, \
    f32*:        lua_pushnumberp, \
    f64:         lua_pushnumber, \
    u8:          lua_pushinteger, \
    u8*:         lua_pushintegerp, \
    u16:         lua_pushinteger, \
    u16*:        lua_pushintegerp, \
    u32:         lua_pushinteger, \
    u32*:        lua_pushintegerp, \
    u64:         lua_pushinteger, \
    u64*:        lua_pushintegerp, \
    i8:          lua_pushinteger, \
    i8*:         lua_pushintegerp, \
    i16:         lua_pushinteger, \
    i16*:        lua_pushintegerp, \
    i32:         lua_pushinteger, \
    i32*:        lua_pushintegerp, \
    i64:         lua_pushinteger, \
    i64*:        lua_pushintegerp, \
    bool:        lua_pushboolean, \
    bool*:       lua_pushbooleanp, \
    const char*: lua_pushstring, \
    v2:          lua_pushvec2, \
    v3:          lua_pushvec3, \
    m4:          lua_pushm4, \
    m4*:         lua_pushm4p \
    )(L, X)

// Define a lua function taking 0 arguments and returning 0 values.
// FUNC is the name of the C function to wrap
#define WRAP_VOID_0_ARG_FUNCTION(FUNC) \
    static int WRAP_NAME(FUNC)(lua_State *L) \
    { \
        FUNC(); \
        return 0; \
    }

// Define a lua function taking 1 argument and returning 0 values.
// FUNC is the name of the C function to wrap
// arg0_t is the type of the argument 0
#define WRAP_VOID_1_ARG_FUNCTION(FUNC, arg0_t) \
    static int WRAP_NAME(FUNC)(lua_State* L) \
    { \
        assert(lua_gettop(L) == 1); \
        arg0_t arg0; \
        FETCH_ARG(L, arg0, 1); \
        FUNC(arg0); \
        return 0; \
    }

// Define a lua function taking 2 arguments and returning 0 values.
// FUNC is the name of the C function to wrap
// arg0_t is the type of the argument 0
// arg1_t is the type of the argument 1
#define WRAP_VOID_2_ARG_FUNCTION(FUNC, arg0_t, arg1_t) \
    static int WRAP_NAME(FUNC)(lua_State* L) \
    { \
        assert(lua_gettop(L) == 2); \
        arg0_t arg0; \
        FETCH_ARG(L, arg0, 1); \
        arg1_t arg1; \
        FETCH_ARG(L, arg1, 2); \
        FUNC(arg0, arg1); \
        return 0; \
    }

// Define a lua function taking 3 arguments and returning 0 values.
// FUNC is the name of the C function to wrap
// arg0_t is the type of the argument 0
// arg1_t is the type of the argument 1
// arg2_t is the type of the argument 2
#define WRAP_VOID_3_ARG_FUNCTION(FUNC, arg0_t, arg1_t, arg2_t) \
    static int WRAP_NAME(FUNC)(lua_State* L) \
    { \
        assert(lua_gettop(L) == 3); \
        arg0_t arg0; \
        FETCH_ARG(L, arg0, 1); \
        arg1_t arg1; \
        FETCH_ARG(L, arg1, 2); \
        arg2_t arg2; \
        FETCH_ARG(L, arg2, 3); \
        FUNC(arg0, arg1, arg2); \
        return 0; \
    }

// Define a lua function taking 4 arguments and returning 0 values.
// FUNC is the name of the C function to wrap
// arg0_t is the type of the argument 0
// arg1_t is the type of the argument 1
// arg2_t is the type of the argument 2
// arg3_t is the type of the argument 3
#define WRAP_VOID_4_ARG_FUNCTION(FUNC, arg0_t, arg1_t, arg2_t, arg3_t) \
    static int WRAP_NAME(FUNC)(lua_State* L) \
    { \
        assert(lua_gettop(L) == 4); \
        arg0_t arg0; \
        FETCH_ARG(L, arg0, 1); \
        arg1_t arg1; \
        FETCH_ARG(L, arg1, 2); \
        arg2_t arg2; \
        FETCH_ARG(L, arg2, 3); \
        arg3_t arg3; \
        FETCH_ARG(L, arg3, 4); \
        FUNC(arg0, arg1, arg2, arg3); \
        return 0; \
    }

// Define a lua function taking 1 argument and returning 1 value.
// FUNC is the name of the C function to wrap
// ret0_t is the type of the return value 0
// arg0_t is the type of the argument 0
#define WRAP_1_RET_1_ARG_FUNCTION(FUNC, ret0_t, arg0_t) \
    static int WRAP_NAME(FUNC)(lua_State* L) \
    { \
        assert(lua_gettop(L) == 1); \
        arg0_t arg0; \
        FETCH_ARG(L, arg0, 1); \
        ret0_t ret_value = FUNC(arg0); \
        RET_VAL(L, ret_value); \
        return 1; \
    }

// Define a lua function taking 2 arguments and returning 2 values.
// FUNC is the name of the C function to wrap
// ret0_t is the type of the return value 0
// ret_arg_id is the id of the argument that should be trated as out variable
// arg0_t is the type of the argument 0
// arg1_t is the type of the argument 1
#define WRAP_2_RET_2_ARG_FUNCTION(FUNC, ret0_t, ret_arg_id, arg0_t, arg1_t) \
    static int WRAP_NAME(FUNC)(lua_State* L) \
    { \
        assert(lua_gettop(L) == 2); \
        arg0_t arg0; \
        FETCH_ARG(L, arg0, 1); \
        arg1_t arg1; \
        FETCH_ARG(L, arg1, 2); \
        ret0_t ret_value = FUNC(arg0, arg1); \
        RET_VAL(L, ret_value); \
        RET_VAL(L, JOIN_STR2(arg, ret_arg_id)); \
        return 2; \
    }

// Define a lua function taking 3 arguments and returning 2 values.
// FUNC is the name of the C function to wrap
// ret0_t is the type of the return value 0
// ret_arg_id is the id of the argument that should be trated as out variable
// arg0_t is the type of the argument 0
// arg1_t is the type of the argument 1
// arg2_t is the type of the argument 2
#define WRAP_2_RET_3_ARG_FUNCTION(FUNC, ret0_t, ret_arg_id, arg0_t, arg1_t, arg2_t) \
    static int WRAP_NAME(FUNC)(lua_State* L) \
    { \
        assert(lua_gettop(L) == 3); \
        arg0_t arg0; \
        FETCH_ARG(L, arg0, 1); \
        arg1_t arg1; \
        FETCH_ARG(L, arg1, 2); \
        arg2_t arg2; \
        FETCH_ARG(L, arg2, 3); \
        ret0_t ret_value = FUNC(arg0, arg1, arg2); \
        RET_VAL(L, ret_value); \
        RET_VAL(L, JOIN_STR2(arg, ret_arg_id)); \
        return 2; \
    }

// Define a lua function taking 2 variable and 3 constant arguments and returning 2 values.
// FUNC is the name of the C function to wrap
// ret0_t is the type of the return value 0
// ret_arg_id is the id of the argument that should be trated as out variable
// arg0_t is the type of the argument 0
// arg1_t is the type of the argument 1
// const1 is a constant variable always used as an argument 2
// const2 is a constant variable always used as an argument 3
// const3 is a constant variable always used as an argument 4
#define WRAP_2_RET_2_ARG_3_CONST_FUNCTION(FUNC, ret0_t, ret_arg_id, arg0_t, arg1_t, const1, const2, const3) \
    static int WRAP_NAME(FUNC)(lua_State* L) \
    { \
        assert(lua_gettop(L) == 2); \
        arg0_t arg0; \
        FETCH_ARG(L, arg0, 1); \
        arg1_t arg1; \
        FETCH_ARG(L, arg1, 2); \
        ret0_t ret_value = FUNC(arg0, arg1, const1, const2, const3); \
        RET_VAL(L, ret_value); \
        RET_VAL(L, JOIN_STR2(arg, ret_arg_id)); \
        return 2; \
    }

// Define a lua function taking 2 variable and 4 constant arguments and returning 2 values.
// FUNC is the name of the C function to wrap
// ret0_t is the type of the return value 0
// ret_arg_id is the id of the argument that should be trated as out variable
// arg0_t is the type of the argument 0
// arg1_t is the type of the argument 1
// const1 is a constant variable always used as an argument 2
// const2 is a constant variable always used as an argument 3
// const3 is a constant variable always used as an argument 4
// const4 is a constant variable always used as an argument 5
#define WRAP_2_RET_2_ARG_4_CONST_FUNCTION(FUNC, ret0_t, ret_arg_id, arg0_t, arg1_t, const1, const2, const3, const4) \
    static int WRAP_NAME(FUNC)(lua_State* L) \
    { \
        assert(lua_gettop(L) == 2); \
        arg0_t arg0; \
        FETCH_ARG(L, arg0, 1); \
        arg1_t arg1; \
        FETCH_ARG(L, arg1, 2); \
        ret0_t ret_value = FUNC(arg0, arg1, const1, const2, const3, const4); \
        RET_VAL(L, ret_value); \
        RET_VAL(L, JOIN_STR2(arg, ret_arg_id)); \
        return 2; \
    }


WRAP_VOID_0_ARG_FUNCTION(im3dPopMatrix)
WRAP_VOID_0_ARG_FUNCTION(im3dPushDrawState)
WRAP_VOID_0_ARG_FUNCTION(im3dPopDrawState)
WRAP_VOID_0_ARG_FUNCTION(im3dDrawXyzAxes)

WRAP_VOID_1_ARG_FUNCTION(im3dPushMatrix, m4)
WRAP_VOID_1_ARG_FUNCTION(im3dSetSize, f32)
WRAP_VOID_1_ARG_FUNCTION(im3dSetColor, u32)

WRAP_VOID_3_ARG_FUNCTION(im3dDrawPoint, v3, float, u32)
WRAP_VOID_4_ARG_FUNCTION(im3dDrawLine, v3, v3, float, u32)

WRAP_VOID_3_ARG_FUNCTION(im3dDrawQuad, v3, v3, v2)
WRAP_VOID_4_ARG_FUNCTION(im3dDrawQuadFromPoints, v3, v3, v3, v3)

WRAP_VOID_3_ARG_FUNCTION(im3dDrawQuadFilled, v3, v3, v2)
WRAP_VOID_4_ARG_FUNCTION(im3dDrawQuadFilledFromPoints, v3, v3, v3, v3)

WRAP_VOID_4_ARG_FUNCTION(im3dDrawCircle, v3, v3, float, int)
WRAP_VOID_4_ARG_FUNCTION(im3dDrawCircleFilled, v3, v3, float, int)

WRAP_VOID_3_ARG_FUNCTION(im3dDrawSphere, v3, float, int)
WRAP_VOID_3_ARG_FUNCTION(im3dDrawSphereFilled, v3, float, int)
WRAP_VOID_2_ARG_FUNCTION(im3dDrawAlignedBox, v3, v3);
WRAP_VOID_2_ARG_FUNCTION(im3dDrawAlignedBoxFilled, v3, v3)

WRAP_VOID_4_ARG_FUNCTION(im3dDrawCylinder, v3, v3, float, int)
WRAP_VOID_4_ARG_FUNCTION(im3dDrawCapsule, v3, v3, float, int)
WRAP_VOID_4_ARG_FUNCTION(im3dDrawPrism, v3, v3, float, int)
WRAP_VOID_4_ARG_FUNCTION(im3dDrawArrow, v3, v3, float, float)
WRAP_2_RET_2_ARG_FUNCTION(im3dGizmo, bool, 1, const char*, m4*)

WRAP_2_RET_3_ARG_FUNCTION(igBegin, bool, 1, const char*, bool*, ImGuiWindowFlags)
WRAP_VOID_0_ARG_FUNCTION(igEnd)
WRAP_VOID_1_ARG_FUNCTION(igText, const char*)
WRAP_2_RET_2_ARG_FUNCTION(igCheckbox, bool, 1, const char*, bool*)
WRAP_1_RET_1_ARG_FUNCTION(igSmallButton, bool, const char*)

WRAP_2_RET_2_ARG_3_CONST_FUNCTION(igInputInt, bool, 1, const char*, i32*, 0, 0, 0)
WRAP_2_RET_2_ARG_4_CONST_FUNCTION(igInputFloat, bool, 1, const char*, f32*, 0.f, 0.f, "%f", 0)

// Since this is a complicated case, igInputText is wrapped manually
// igInputText takes label and a buffer and returns whether the value has
// changed and potentially modified by the user text buffer.
static int WRAP_NAME(igInputText)(lua_State* L)
{
    #define INPUT_TEXT_BUF_SIZE 512
    assert(lua_gettop(L) == 2);
    const char* label;
    FETCH_ARG(L, label, 1);
    const char* value;
    FETCH_ARG(L, value, 2);
    char buffer[INPUT_TEXT_BUF_SIZE] = {0};
    cu_strlcpy(buffer, value, INPUT_TEXT_BUF_SIZE);

    bool ret_value = igInputText(label, buffer, INPUT_TEXT_BUF_SIZE, 0, NULL, NULL);
    RET_VAL(L, ret_value);
    lua_pushlstring(L, buffer, INPUT_TEXT_BUF_SIZE);
    return 2;
}

WRAP_VOID_4_ARG_FUNCTION(log_log, int, const char*, int, const char*)

#define REGISTER(L, NAME) lua_register(L, #NAME, WRAP_NAME(NAME))

void RegisterLuaFunctions(lua_State* L)
{
    REGISTER(L, im3dPushDrawState);
    REGISTER(L, im3dPushMatrix);
    REGISTER(L, im3dPopMatrix);
    REGISTER(L, im3dPopDrawState);
    REGISTER(L, im3dDrawXyzAxes);
    REGISTER(L, im3dSetSize);
    REGISTER(L, im3dSetColor);
    REGISTER(L, im3dDrawPoint);
    REGISTER(L, im3dDrawLine);
    REGISTER(L, im3dDrawQuad);
    REGISTER(L, im3dDrawQuadFromPoints);
    REGISTER(L, im3dDrawQuadFilled);
    REGISTER(L, im3dDrawQuadFilledFromPoints);
    REGISTER(L, im3dDrawCircle);
    REGISTER(L, im3dDrawCircleFilled);
    REGISTER(L, im3dDrawSphere);
    REGISTER(L, im3dDrawSphereFilled);
    REGISTER(L, im3dDrawAlignedBox);
    REGISTER(L, im3dDrawAlignedBoxFilled);
    REGISTER(L, im3dDrawCylinder);
    REGISTER(L, im3dDrawCapsule);
    REGISTER(L, im3dDrawPrism);
    REGISTER(L, im3dDrawArrow);
    REGISTER(L, im3dGizmo);

    REGISTER(L, log_log);

    LoadImguiBindings(L);
}

#undef REGISTER