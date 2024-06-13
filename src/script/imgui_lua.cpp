
/*
    THIS FILE HAS BEEN GENERATED WITH gen_lua_wrappers.py.
    Do not modify it directly.
*/
extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include "log/log.h"
}
#include <imgui.h>
#include "imgui_lua.h"

#define MAX_STR_BUFFER_SIZE 256

namespace ImGui
{
    
static inline bool GetGlobalbool(lua_State* L, const char* name) {
    lua_getglobal(L, name);
    bool value = (bool)lua_toboolean(L, -1);
    lua_pop(L, 1);
    return value;
}

static inline void SetGlobalbool(lua_State* L, const char* name, bool value) {
    lua_pushboolean(L, value);
    lua_setglobal(L, name);
}


static inline float GetGlobalfloat(lua_State* L, const char* name) {
    lua_getglobal(L, name);
    float value = (float)luaL_checknumber(L, -1);
    lua_pop(L, 1);
    return value;
}

static inline void SetGlobalfloat(lua_State* L, const char* name, float value) {
    lua_pushnumber(L, value);
    lua_setglobal(L, name);
}


static inline int GetGlobalint(lua_State* L, const char* name) {
    lua_getglobal(L, name);
    int value = (int)luaL_checkinteger(L, -1);
    lua_pop(L, 1);
    return value;
}

static inline void SetGlobalint(lua_State* L, const char* name, int value) {
    lua_pushinteger(L, value);
    lua_setglobal(L, name);
}


static inline ImVec2 GetGlobalImVec2(lua_State* L, const char* name) {
    ImVec2 value;
    lua_getglobal(L, name);
    if (lua_istable(L, -1)) {
        lua_pushnumber(L, 1);
        lua_gettable(L, -2);
        value.x = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_pushnumber(L, 2);
        lua_gettable(L, -2);
        value.y = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    return value;
}


static inline void SetGlobalImVec2(lua_State* L, const char* name, ImVec2 value) {
    lua_newtable(L);
    lua_pushnumber(L, 1);
    lua_pushnumber(L, value.x);
    lua_settable(L, -3);
    lua_pushnumber(L, 2);
    lua_pushnumber(L, value.y);
    lua_settable(L, -3);
    lua_setglobal(L, name);
}


static inline ImVec4 GetGlobalImVec4(lua_State* L, const char* name) {
    ImVec4 value;
    lua_getglobal(L, name);
    if (lua_istable(L, -1)) {
        lua_pushnumber(L, 1);
        lua_gettable(L, -2);
        value.x = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_pushnumber(L, 2);
        lua_gettable(L, -2);
        value.y = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_pushnumber(L, 3);
        lua_gettable(L, -2);
        value.z = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_pushnumber(L, 4);
        lua_gettable(L, -2);
        value.w = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    return value;
}


static inline void SetGlobalImVec4(lua_State* L, const char* name, ImVec4 value) {
    lua_newtable(L);
    lua_pushnumber(L, 1);
    lua_pushnumber(L, value.x);
    lua_settable(L, -3);
    lua_pushnumber(L, 2);
    lua_pushnumber(L, value.y);
    lua_settable(L, -3);
    lua_pushnumber(L, 3);
    lua_pushnumber(L, value.z);
    lua_settable(L, -3);
    lua_pushnumber(L, 4);
    lua_pushnumber(L, value.w);
    lua_settable(L, -3);
    lua_setglobal(L, name);
}


static inline void GetGlobalCharArray(lua_State* L, const char* name, char* value, size_t size) {
    lua_getglobal(L, name);
    strncpy(value, luaL_checkstring(L, -1), size);
    lua_pop(L, 1);
}

static inline void SetGlobalCharArray(lua_State* L, const char* name, const char* value) {
    lua_pushstring(L, value);
    lua_setglobal(L, name);
}


static int lua_Begin(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 3 && num_args >= 1 && lua_isstring(L, 1)) {
        const char * name = (const char *)luaL_checkstring(L, 1);
        if (num_args == 1) {
           bool result = Begin(name);
           lua_pushboolean(L, result);
           return 1;
        }
        const char* p_open_str = luaL_checkstring(L, 2);
        bool  p_open = GetGlobalbool(L, p_open_str);
        if (num_args == 2) {
           bool result = Begin(name, &p_open);
           SetGlobalbool(L, p_open_str, p_open);
           lua_pushboolean(L, result);
           return 1;
        }
        ImGuiWindowFlags flags = (ImGuiWindowFlags)luaL_checkinteger(L, 3);
        if (num_args == 3) {
           bool result = Begin(name, &p_open, flags);
           SetGlobalbool(L, p_open_str, p_open);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    L_WARN("No matching overload found for Begin");
    return 0;
}

static int lua_End(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 0 && num_args >= 0) {
        if (num_args == 0) {
           End();
           return 0;
        }
    }
    L_WARN("No matching overload found for End");
    return 0;
}

static int lua_BeginChild(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 4 && num_args >= 1 && lua_isstring(L, 1)) {
        const char * str_id = (const char *)luaL_checkstring(L, 1);
        if (num_args == 1) {
           bool result = BeginChild(str_id);
           lua_pushboolean(L, result);
           return 1;
        }
        const char* size_str = luaL_checkstring(L, 2);
        const ImVec2  size = GetGlobalImVec2(L, size_str);
        if (num_args == 2) {
           bool result = BeginChild(str_id, size);
           lua_pushboolean(L, result);
           return 1;
        }
        bool border = (bool)lua_toboolean(L, 3);
        if (num_args == 3) {
           bool result = BeginChild(str_id, size, border);
           lua_pushboolean(L, result);
           return 1;
        }
        ImGuiWindowFlags flags = (ImGuiWindowFlags)luaL_checkinteger(L, 4);
        if (num_args == 4) {
           bool result = BeginChild(str_id, size, border, flags);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    if (num_args <= 4 && num_args >= 1 && lua_isinteger(L, 1)) {
        ImGuiID id = (ImGuiID)luaL_checkinteger(L, 1);
        if (num_args == 1) {
           bool result = BeginChild(id);
           lua_pushboolean(L, result);
           return 1;
        }
        const char* size_str = luaL_checkstring(L, 2);
        const ImVec2  size = GetGlobalImVec2(L, size_str);
        if (num_args == 2) {
           bool result = BeginChild(id, size);
           lua_pushboolean(L, result);
           return 1;
        }
        bool border = (bool)lua_toboolean(L, 3);
        if (num_args == 3) {
           bool result = BeginChild(id, size, border);
           lua_pushboolean(L, result);
           return 1;
        }
        ImGuiWindowFlags flags = (ImGuiWindowFlags)luaL_checkinteger(L, 4);
        if (num_args == 4) {
           bool result = BeginChild(id, size, border, flags);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    L_WARN("No matching overload found for BeginChild");
    return 0;
}

static int lua_EndChild(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 0 && num_args >= 0) {
        if (num_args == 0) {
           EndChild();
           return 0;
        }
    }
    L_WARN("No matching overload found for EndChild");
    return 0;
}

static int lua_SetNextWindowPos(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 3 && num_args >= 1 && lua_isstring(L, 1)) {
        const char* pos_str = luaL_checkstring(L, 1);
        const ImVec2  pos = GetGlobalImVec2(L, pos_str);
        if (num_args == 1) {
           SetNextWindowPos(pos);
           return 0;
        }
        ImGuiCond cond = (ImGuiCond)luaL_checkinteger(L, 2);
        if (num_args == 2) {
           SetNextWindowPos(pos, cond);
           return 0;
        }
        const char* pivot_str = luaL_checkstring(L, 3);
        const ImVec2  pivot = GetGlobalImVec2(L, pivot_str);
        if (num_args == 3) {
           SetNextWindowPos(pos, cond, pivot);
           return 0;
        }
    }
    L_WARN("No matching overload found for SetNextWindowPos");
    return 0;
}

static int lua_SetNextWindowSize(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 2 && num_args >= 1 && lua_isstring(L, 1)) {
        const char* size_str = luaL_checkstring(L, 1);
        const ImVec2  size = GetGlobalImVec2(L, size_str);
        if (num_args == 1) {
           SetNextWindowSize(size);
           return 0;
        }
        ImGuiCond cond = (ImGuiCond)luaL_checkinteger(L, 2);
        if (num_args == 2) {
           SetNextWindowSize(size, cond);
           return 0;
        }
    }
    L_WARN("No matching overload found for SetNextWindowSize");
    return 0;
}

static int lua_PushStyleColor(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 2 && num_args >= 2 && lua_isinteger(L, 1) && lua_isinteger(L, 2)) {
        ImGuiCol idx = (ImGuiCol)luaL_checkinteger(L, 1);
        ImU32 col = (ImU32)luaL_checkinteger(L, 2);
        if (num_args == 2) {
           PushStyleColor(idx, col);
           return 0;
        }
    }
    if (num_args <= 2 && num_args >= 2 && lua_isinteger(L, 1) && lua_isstring(L, 2)) {
        ImGuiCol idx = (ImGuiCol)luaL_checkinteger(L, 1);
        const char* col_str = luaL_checkstring(L, 2);
        const ImVec4  col = GetGlobalImVec4(L, col_str);
        if (num_args == 2) {
           PushStyleColor(idx, col);
           return 0;
        }
    }
    L_WARN("No matching overload found for PushStyleColor");
    return 0;
}

static int lua_PopStyleColor(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 1 && num_args >= 0) {
        if (num_args == 0) {
           PopStyleColor();
           return 0;
        }
        int count = (int)luaL_checkinteger(L, 1);
        if (num_args == 1) {
           PopStyleColor(count);
           return 0;
        }
    }
    L_WARN("No matching overload found for PopStyleColor");
    return 0;
}

static int lua_PushStyleVar(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 2 && num_args >= 2 && lua_isinteger(L, 1) && lua_isnumber(L, 2)) {
        ImGuiStyleVar idx = (ImGuiStyleVar)luaL_checkinteger(L, 1);
        float val = (float)luaL_checknumber(L, 2);
        if (num_args == 2) {
           PushStyleVar(idx, val);
           return 0;
        }
    }
    if (num_args <= 2 && num_args >= 2 && lua_isinteger(L, 1) && lua_isstring(L, 2)) {
        ImGuiStyleVar idx = (ImGuiStyleVar)luaL_checkinteger(L, 1);
        const char* val_str = luaL_checkstring(L, 2);
        const ImVec2  val = GetGlobalImVec2(L, val_str);
        if (num_args == 2) {
           PushStyleVar(idx, val);
           return 0;
        }
    }
    L_WARN("No matching overload found for PushStyleVar");
    return 0;
}

static int lua_PopStyleVar(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 1 && num_args >= 0) {
        if (num_args == 0) {
           PopStyleVar();
           return 0;
        }
        int count = (int)luaL_checkinteger(L, 1);
        if (num_args == 1) {
           PopStyleVar(count);
           return 0;
        }
    }
    L_WARN("No matching overload found for PopStyleVar");
    return 0;
}

static int lua_Separator(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 0 && num_args >= 0) {
        if (num_args == 0) {
           Separator();
           return 0;
        }
    }
    L_WARN("No matching overload found for Separator");
    return 0;
}

static int lua_SameLine(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 2 && num_args >= 0) {
        if (num_args == 0) {
           SameLine();
           return 0;
        }
        float offset_from_start_x = (float)luaL_checknumber(L, 1);
        if (num_args == 1) {
           SameLine(offset_from_start_x);
           return 0;
        }
        float spacing = (float)luaL_checknumber(L, 2);
        if (num_args == 2) {
           SameLine(offset_from_start_x, spacing);
           return 0;
        }
    }
    L_WARN("No matching overload found for SameLine");
    return 0;
}

static int lua_Spacing(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 0 && num_args >= 0) {
        if (num_args == 0) {
           Spacing();
           return 0;
        }
    }
    L_WARN("No matching overload found for Spacing");
    return 0;
}

static int lua_PushID(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 1 && num_args >= 1 && lua_isstring(L, 1)) {
        const char * str_id = (const char *)luaL_checkstring(L, 1);
        if (num_args == 1) {
           PushID(str_id);
           return 0;
        }
    }
    if (num_args <= 2 && num_args >= 2 && lua_isstring(L, 1) && lua_isstring(L, 2)) {
        const char * str_id_begin = (const char *)luaL_checkstring(L, 1);
        const char * str_id_end = (const char *)luaL_checkstring(L, 2);
        if (num_args == 2) {
           PushID(str_id_begin, str_id_end);
           return 0;
        }
    }
    if (num_args <= 1 && num_args >= 1 && lua_isinteger(L, 1)) {
        int int_id = (int)luaL_checkinteger(L, 1);
        if (num_args == 1) {
           PushID(int_id);
           return 0;
        }
    }
    L_WARN("No matching overload found for PushID");
    return 0;
}

static int lua_PopID(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 0 && num_args >= 0) {
        if (num_args == 0) {
           PopID();
           return 0;
        }
    }
    L_WARN("No matching overload found for PopID");
    return 0;
}

static int lua_TextUnformatted(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 2 && num_args >= 1 && lua_isstring(L, 1)) {
        const char * text = (const char *)luaL_checkstring(L, 1);
        if (num_args == 1) {
           TextUnformatted(text);
           return 0;
        }
        const char * text_end = (const char *)luaL_checkstring(L, 2);
        if (num_args == 2) {
           TextUnformatted(text, text_end);
           return 0;
        }
    }
    L_WARN("No matching overload found for TextUnformatted");
    return 0;
}

static int lua_Button(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 2 && num_args >= 1 && lua_isstring(L, 1)) {
        const char * label = (const char *)luaL_checkstring(L, 1);
        if (num_args == 1) {
           bool result = Button(label);
           lua_pushboolean(L, result);
           return 1;
        }
        const char* size_str = luaL_checkstring(L, 2);
        const ImVec2  size = GetGlobalImVec2(L, size_str);
        if (num_args == 2) {
           bool result = Button(label, size);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    L_WARN("No matching overload found for Button");
    return 0;
}

static int lua_Checkbox(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 2 && num_args >= 2 && lua_isstring(L, 1) && lua_isstring(L, 2)) {
        const char * label = (const char *)luaL_checkstring(L, 1);
        const char* v_str = luaL_checkstring(L, 2);
        bool  v = GetGlobalbool(L, v_str);
        if (num_args == 2) {
           bool result = Checkbox(label, &v);
           SetGlobalbool(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    L_WARN("No matching overload found for Checkbox");
    return 0;
}

static int lua_RadioButton(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 2 && num_args >= 2 && lua_isstring(L, 1) && lua_isboolean(L, 2)) {
        const char * label = (const char *)luaL_checkstring(L, 1);
        bool active = (bool)lua_toboolean(L, 2);
        if (num_args == 2) {
           bool result = RadioButton(label, active);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    if (num_args <= 3 && num_args >= 3 && lua_isstring(L, 1) && lua_isstring(L, 2) && lua_isinteger(L, 3)) {
        const char * label = (const char *)luaL_checkstring(L, 1);
        const char* v_str = luaL_checkstring(L, 2);
        int  v = GetGlobalint(L, v_str);
        int v_button = (int)luaL_checkinteger(L, 3);
        if (num_args == 3) {
           bool result = RadioButton(label, &v, v_button);
           SetGlobalint(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    L_WARN("No matching overload found for RadioButton");
    return 0;
}

static int lua_ProgressBar(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 3 && num_args >= 1 && lua_isnumber(L, 1)) {
        float fraction = (float)luaL_checknumber(L, 1);
        if (num_args == 1) {
           ProgressBar(fraction);
           return 0;
        }
        const char* size_arg_str = luaL_checkstring(L, 2);
        const ImVec2  size_arg = GetGlobalImVec2(L, size_arg_str);
        if (num_args == 2) {
           ProgressBar(fraction, size_arg);
           return 0;
        }
        const char * overlay = (const char *)luaL_checkstring(L, 3);
        if (num_args == 3) {
           ProgressBar(fraction, size_arg, overlay);
           return 0;
        }
    }
    L_WARN("No matching overload found for ProgressBar");
    return 0;
}

static int lua_BeginCombo(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 3 && num_args >= 2 && lua_isstring(L, 1) && lua_isstring(L, 2)) {
        const char * label = (const char *)luaL_checkstring(L, 1);
        const char * preview_value = (const char *)luaL_checkstring(L, 2);
        if (num_args == 2) {
           bool result = BeginCombo(label, preview_value);
           lua_pushboolean(L, result);
           return 1;
        }
        ImGuiComboFlags flags = (ImGuiComboFlags)luaL_checkinteger(L, 3);
        if (num_args == 3) {
           bool result = BeginCombo(label, preview_value, flags);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    L_WARN("No matching overload found for BeginCombo");
    return 0;
}

static int lua_EndCombo(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 0 && num_args >= 0) {
        if (num_args == 0) {
           EndCombo();
           return 0;
        }
    }
    L_WARN("No matching overload found for EndCombo");
    return 0;
}

static int lua_DragFloat(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 7 && num_args >= 2 && lua_isstring(L, 1) && lua_isstring(L, 2)) {
        const char * label = (const char *)luaL_checkstring(L, 1);
        const char* v_str = luaL_checkstring(L, 2);
        float  v = GetGlobalfloat(L, v_str);
        if (num_args == 2) {
           bool result = DragFloat(label, &v);
           SetGlobalfloat(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
        float v_speed = (float)luaL_checknumber(L, 3);
        if (num_args == 3) {
           bool result = DragFloat(label, &v, v_speed);
           SetGlobalfloat(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
        float v_min = (float)luaL_checknumber(L, 4);
        if (num_args == 4) {
           bool result = DragFloat(label, &v, v_speed, v_min);
           SetGlobalfloat(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
        float v_max = (float)luaL_checknumber(L, 5);
        if (num_args == 5) {
           bool result = DragFloat(label, &v, v_speed, v_min, v_max);
           SetGlobalfloat(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
        const char * format = (const char *)luaL_checkstring(L, 6);
        if (num_args == 6) {
           bool result = DragFloat(label, &v, v_speed, v_min, v_max, format);
           SetGlobalfloat(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
        ImGuiSliderFlags flags = (ImGuiSliderFlags)luaL_checkinteger(L, 7);
        if (num_args == 7) {
           bool result = DragFloat(label, &v, v_speed, v_min, v_max, format, flags);
           SetGlobalfloat(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    L_WARN("No matching overload found for DragFloat");
    return 0;
}

static int lua_DragInt(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 7 && num_args >= 2 && lua_isstring(L, 1) && lua_isstring(L, 2)) {
        const char * label = (const char *)luaL_checkstring(L, 1);
        const char* v_str = luaL_checkstring(L, 2);
        int  v = GetGlobalint(L, v_str);
        if (num_args == 2) {
           bool result = DragInt(label, &v);
           SetGlobalint(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
        float v_speed = (float)luaL_checknumber(L, 3);
        if (num_args == 3) {
           bool result = DragInt(label, &v, v_speed);
           SetGlobalint(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
        int v_min = (int)luaL_checkinteger(L, 4);
        if (num_args == 4) {
           bool result = DragInt(label, &v, v_speed, v_min);
           SetGlobalint(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
        int v_max = (int)luaL_checkinteger(L, 5);
        if (num_args == 5) {
           bool result = DragInt(label, &v, v_speed, v_min, v_max);
           SetGlobalint(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
        const char * format = (const char *)luaL_checkstring(L, 6);
        if (num_args == 6) {
           bool result = DragInt(label, &v, v_speed, v_min, v_max, format);
           SetGlobalint(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
        ImGuiSliderFlags flags = (ImGuiSliderFlags)luaL_checkinteger(L, 7);
        if (num_args == 7) {
           bool result = DragInt(label, &v, v_speed, v_min, v_max, format, flags);
           SetGlobalint(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    L_WARN("No matching overload found for DragInt");
    return 0;
}

static int lua_SliderFloat(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 6 && num_args >= 4 && lua_isstring(L, 1) && lua_isstring(L, 2) && lua_isnumber(L, 3) && lua_isnumber(L, 4)) {
        const char * label = (const char *)luaL_checkstring(L, 1);
        const char* v_str = luaL_checkstring(L, 2);
        float  v = GetGlobalfloat(L, v_str);
        float v_min = (float)luaL_checknumber(L, 3);
        float v_max = (float)luaL_checknumber(L, 4);
        if (num_args == 4) {
           bool result = SliderFloat(label, &v, v_min, v_max);
           SetGlobalfloat(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
        const char * format = (const char *)luaL_checkstring(L, 5);
        if (num_args == 5) {
           bool result = SliderFloat(label, &v, v_min, v_max, format);
           SetGlobalfloat(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
        ImGuiSliderFlags flags = (ImGuiSliderFlags)luaL_checkinteger(L, 6);
        if (num_args == 6) {
           bool result = SliderFloat(label, &v, v_min, v_max, format, flags);
           SetGlobalfloat(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    L_WARN("No matching overload found for SliderFloat");
    return 0;
}

static int lua_InputText(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 6 && num_args >= 3 && lua_isstring(L, 1) && lua_isstring(L, 2) && lua_isinteger(L, 3)) {
        const char * label = (const char *)luaL_checkstring(L, 1);
        char  buf[MAX_STR_BUFFER_SIZE] = {0};
        const char* buf_str = luaL_checkstring(L, 2);
        GetGlobalCharArray(L, buf_str, buf, MAX_STR_BUFFER_SIZE);
        size_t buf_size = (size_t)luaL_checkinteger(L, 3);
        if (num_args == 3) {
           bool result = InputText(label, buf, buf_size);
           SetGlobalCharArray(L, buf_str, buf);
           lua_pushboolean(L, result);
           return 1;
        }
        ImGuiInputTextFlags flags = (ImGuiInputTextFlags)luaL_checkinteger(L, 4);
        if (num_args == 4) {
           bool result = InputText(label, buf, buf_size, flags);
           SetGlobalCharArray(L, buf_str, buf);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    L_WARN("No matching overload found for InputText");
    return 0;
}

static int lua_InputFloat(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 6 && num_args >= 2 && lua_isstring(L, 1) && lua_isstring(L, 2)) {
        const char * label = (const char *)luaL_checkstring(L, 1);
        const char* v_str = luaL_checkstring(L, 2);
        float  v = GetGlobalfloat(L, v_str);
        if (num_args == 2) {
           bool result = InputFloat(label, &v);
           SetGlobalfloat(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
        float step = (float)luaL_checknumber(L, 3);
        if (num_args == 3) {
           bool result = InputFloat(label, &v, step);
           SetGlobalfloat(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
        float step_fast = (float)luaL_checknumber(L, 4);
        if (num_args == 4) {
           bool result = InputFloat(label, &v, step, step_fast);
           SetGlobalfloat(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
        const char * format = (const char *)luaL_checkstring(L, 5);
        if (num_args == 5) {
           bool result = InputFloat(label, &v, step, step_fast, format);
           SetGlobalfloat(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
        ImGuiInputTextFlags flags = (ImGuiInputTextFlags)luaL_checkinteger(L, 6);
        if (num_args == 6) {
           bool result = InputFloat(label, &v, step, step_fast, format, flags);
           SetGlobalfloat(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    L_WARN("No matching overload found for InputFloat");
    return 0;
}

static int lua_InputInt(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 5 && num_args >= 2 && lua_isstring(L, 1) && lua_isstring(L, 2)) {
        const char * label = (const char *)luaL_checkstring(L, 1);
        const char* v_str = luaL_checkstring(L, 2);
        int  v = GetGlobalint(L, v_str);
        if (num_args == 2) {
           bool result = InputInt(label, &v);
           SetGlobalint(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
        int step = (int)luaL_checkinteger(L, 3);
        if (num_args == 3) {
           bool result = InputInt(label, &v, step);
           SetGlobalint(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
        int step_fast = (int)luaL_checkinteger(L, 4);
        if (num_args == 4) {
           bool result = InputInt(label, &v, step, step_fast);
           SetGlobalint(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
        ImGuiInputTextFlags flags = (ImGuiInputTextFlags)luaL_checkinteger(L, 5);
        if (num_args == 5) {
           bool result = InputInt(label, &v, step, step_fast, flags);
           SetGlobalint(L, v_str, v);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    L_WARN("No matching overload found for InputInt");
    return 0;
}

static int lua_ColorEdit3(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 3 && num_args >= 2 && lua_isstring(L, 1) && lua_istable(L, 2)) {
        const char * label = (const char *)luaL_checkstring(L, 1);
        float col[3];
        lua_pushnumber(L, 1);
        lua_gettable(L, 2);
        col[0] = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_pushnumber(L, 2);
        lua_gettable(L, 2);
        col[1] = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_pushnumber(L, 3);
        lua_gettable(L, 2);
        col[2] = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        if (num_args == 2) {
           bool result = ColorEdit3(label, col);
           lua_pushboolean(L, result);
           return 1;
        }
        ImGuiColorEditFlags flags = (ImGuiColorEditFlags)luaL_checkinteger(L, 3);
        if (num_args == 3) {
           bool result = ColorEdit3(label, col, flags);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    L_WARN("No matching overload found for ColorEdit3");
    return 0;
}

static int lua_ColorEdit4(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 3 && num_args >= 2 && lua_isstring(L, 1) && lua_istable(L, 2)) {
        const char * label = (const char *)luaL_checkstring(L, 1);
        float col[4];
        lua_pushnumber(L, 1);
        lua_gettable(L, 2);
        col[0] = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_pushnumber(L, 2);
        lua_gettable(L, 2);
        col[1] = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_pushnumber(L, 3);
        lua_gettable(L, 2);
        col[2] = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        lua_pushnumber(L, 4);
        lua_gettable(L, 2);
        col[3] = (float)luaL_checknumber(L, -1);
        lua_pop(L, 1);
        if (num_args == 2) {
           bool result = ColorEdit4(label, col);
           lua_pushboolean(L, result);
           return 1;
        }
        ImGuiColorEditFlags flags = (ImGuiColorEditFlags)luaL_checkinteger(L, 3);
        if (num_args == 3) {
           bool result = ColorEdit4(label, col, flags);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    L_WARN("No matching overload found for ColorEdit4");
    return 0;
}

static int lua_Selectable(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 4 && num_args >= 1 && lua_isstring(L, 1)) {
        const char * label = (const char *)luaL_checkstring(L, 1);
        if (num_args == 1) {
           bool result = Selectable(label);
           lua_pushboolean(L, result);
           return 1;
        }
        bool selected = (bool)lua_toboolean(L, 2);
        if (num_args == 2) {
           bool result = Selectable(label, selected);
           lua_pushboolean(L, result);
           return 1;
        }
        ImGuiSelectableFlags flags = (ImGuiSelectableFlags)luaL_checkinteger(L, 3);
        if (num_args == 3) {
           bool result = Selectable(label, selected, flags);
           lua_pushboolean(L, result);
           return 1;
        }
        const char* size_str = luaL_checkstring(L, 4);
        const ImVec2  size = GetGlobalImVec2(L, size_str);
        if (num_args == 4) {
           bool result = Selectable(label, selected, flags, size);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    if (num_args <= 4 && num_args >= 2 && lua_isstring(L, 1) && lua_isstring(L, 2)) {
        const char * label = (const char *)luaL_checkstring(L, 1);
        const char* p_selected_str = luaL_checkstring(L, 2);
        bool  p_selected = GetGlobalbool(L, p_selected_str);
        if (num_args == 2) {
           bool result = Selectable(label, &p_selected);
           SetGlobalbool(L, p_selected_str, p_selected);
           lua_pushboolean(L, result);
           return 1;
        }
        ImGuiSelectableFlags flags = (ImGuiSelectableFlags)luaL_checkinteger(L, 3);
        if (num_args == 3) {
           bool result = Selectable(label, &p_selected, flags);
           SetGlobalbool(L, p_selected_str, p_selected);
           lua_pushboolean(L, result);
           return 1;
        }
        const char* size_str = luaL_checkstring(L, 4);
        const ImVec2  size = GetGlobalImVec2(L, size_str);
        if (num_args == 4) {
           bool result = Selectable(label, &p_selected, flags, size);
           SetGlobalbool(L, p_selected_str, p_selected);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    L_WARN("No matching overload found for Selectable");
    return 0;
}

static int lua_BeginListBox(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 2 && num_args >= 1 && lua_isstring(L, 1)) {
        const char * label = (const char *)luaL_checkstring(L, 1);
        if (num_args == 1) {
           bool result = BeginListBox(label);
           lua_pushboolean(L, result);
           return 1;
        }
        const char* size_str = luaL_checkstring(L, 2);
        const ImVec2  size = GetGlobalImVec2(L, size_str);
        if (num_args == 2) {
           bool result = BeginListBox(label, size);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    L_WARN("No matching overload found for BeginListBox");
    return 0;
}

static int lua_EndListBox(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 0 && num_args >= 0) {
        if (num_args == 0) {
           EndListBox();
           return 0;
        }
    }
    L_WARN("No matching overload found for EndListBox");
    return 0;
}

static int lua_BeginTooltip(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 0 && num_args >= 0) {
        if (num_args == 0) {
           BeginTooltip();
           return 0;
        }
    }
    L_WARN("No matching overload found for BeginTooltip");
    return 0;
}

static int lua_EndTooltip(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 0 && num_args >= 0) {
        if (num_args == 0) {
           EndTooltip();
           return 0;
        }
    }
    L_WARN("No matching overload found for EndTooltip");
    return 0;
}

static int lua_BeginPopup(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 2 && num_args >= 1 && lua_isstring(L, 1)) {
        const char * str_id = (const char *)luaL_checkstring(L, 1);
        if (num_args == 1) {
           bool result = BeginPopup(str_id);
           lua_pushboolean(L, result);
           return 1;
        }
        ImGuiWindowFlags flags = (ImGuiWindowFlags)luaL_checkinteger(L, 2);
        if (num_args == 2) {
           bool result = BeginPopup(str_id, flags);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    L_WARN("No matching overload found for BeginPopup");
    return 0;
}

static int lua_EndPopup(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 0 && num_args >= 0) {
        if (num_args == 0) {
           EndPopup();
           return 0;
        }
    }
    L_WARN("No matching overload found for EndPopup");
    return 0;
}

static int lua_OpenPopup(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 2 && num_args >= 1 && lua_isstring(L, 1)) {
        const char * str_id = (const char *)luaL_checkstring(L, 1);
        if (num_args == 1) {
           OpenPopup(str_id);
           return 0;
        }
        ImGuiPopupFlags popup_flags = (ImGuiPopupFlags)luaL_checkinteger(L, 2);
        if (num_args == 2) {
           OpenPopup(str_id, popup_flags);
           return 0;
        }
    }
    if (num_args <= 2 && num_args >= 1 && lua_isinteger(L, 1)) {
        ImGuiID id = (ImGuiID)luaL_checkinteger(L, 1);
        if (num_args == 1) {
           OpenPopup(id);
           return 0;
        }
        ImGuiPopupFlags popup_flags = (ImGuiPopupFlags)luaL_checkinteger(L, 2);
        if (num_args == 2) {
           OpenPopup(id, popup_flags);
           return 0;
        }
    }
    L_WARN("No matching overload found for OpenPopup");
    return 0;
}

static int lua_CloseCurrentPopup(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 0 && num_args >= 0) {
        if (num_args == 0) {
           CloseCurrentPopup();
           return 0;
        }
    }
    L_WARN("No matching overload found for CloseCurrentPopup");
    return 0;
}

static int lua_Columns(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 3 && num_args >= 0) {
        if (num_args == 0) {
           Columns();
           return 0;
        }
        int count = (int)luaL_checkinteger(L, 1);
        if (num_args == 1) {
           Columns(count);
           return 0;
        }
        const char * id = (const char *)luaL_checkstring(L, 2);
        if (num_args == 2) {
           Columns(count, id);
           return 0;
        }
        bool border = (bool)lua_toboolean(L, 3);
        if (num_args == 3) {
           Columns(count, id, border);
           return 0;
        }
    }
    L_WARN("No matching overload found for Columns");
    return 0;
}

static int lua_NextColumn(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 0 && num_args >= 0) {
        if (num_args == 0) {
           NextColumn();
           return 0;
        }
    }
    L_WARN("No matching overload found for NextColumn");
    return 0;
}

static int lua_IsItemHovered(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 1 && num_args >= 0) {
        if (num_args == 0) {
           bool result = IsItemHovered();
           lua_pushboolean(L, result);
           return 1;
        }
        ImGuiHoveredFlags flags = (ImGuiHoveredFlags)luaL_checkinteger(L, 1);
        if (num_args == 1) {
           bool result = IsItemHovered(flags);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    L_WARN("No matching overload found for IsItemHovered");
    return 0;
}

static int lua_IsItemActive(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 0 && num_args >= 0) {
        if (num_args == 0) {
           bool result = IsItemActive();
           lua_pushboolean(L, result);
           return 1;
        }
    }
    L_WARN("No matching overload found for IsItemActive");
    return 0;
}

static int lua_IsItemClicked(lua_State* L) {
    int num_args = lua_gettop(L);
    if (num_args <= 1 && num_args >= 0) {
        if (num_args == 0) {
           bool result = IsItemClicked();
           lua_pushboolean(L, result);
           return 1;
        }
        ImGuiMouseButton mouse_button = (ImGuiMouseButton)luaL_checkinteger(L, 1);
        if (num_args == 1) {
           bool result = IsItemClicked(mouse_button);
           lua_pushboolean(L, result);
           return 1;
        }
    }
    L_WARN("No matching overload found for IsItemClicked");
    return 0;
}
} // namespace ImGui
static const struct luaL_Reg imgui_lib[] = {
    {"SetNextWindowPos", ImGui::lua_SetNextWindowPos},
    {"EndPopup", ImGui::lua_EndPopup},
    {"PushID", ImGui::lua_PushID},
    {"EndListBox", ImGui::lua_EndListBox},
    {"Columns", ImGui::lua_Columns},
    {"PopID", ImGui::lua_PopID},
    {"Spacing", ImGui::lua_Spacing},
    {"EndCombo", ImGui::lua_EndCombo},
    {"SliderFloat", ImGui::lua_SliderFloat},
    {"InputFloat", ImGui::lua_InputFloat},
    {"NextColumn", ImGui::lua_NextColumn},
    {"CloseCurrentPopup", ImGui::lua_CloseCurrentPopup},
    {"BeginListBox", ImGui::lua_BeginListBox},
    {"Checkbox", ImGui::lua_Checkbox},
    {"InputInt", ImGui::lua_InputInt},
    {"Begin", ImGui::lua_Begin},
    {"DragFloat", ImGui::lua_DragFloat},
    {"RadioButton", ImGui::lua_RadioButton},
    {"DragInt", ImGui::lua_DragInt},
    {"TextUnformatted", ImGui::lua_TextUnformatted},
    {"EndTooltip", ImGui::lua_EndTooltip},
    {"BeginPopup", ImGui::lua_BeginPopup},
    {"InputText", ImGui::lua_InputText},
    {"End", ImGui::lua_End},
    {"Button", ImGui::lua_Button},
    {"PopStyleVar", ImGui::lua_PopStyleVar},
    {"EndChild", ImGui::lua_EndChild},
    {"BeginCombo", ImGui::lua_BeginCombo},
    {"BeginTooltip", ImGui::lua_BeginTooltip},
    {"ColorEdit3", ImGui::lua_ColorEdit3},
    {"PushStyleColor", ImGui::lua_PushStyleColor},
    {"OpenPopup", ImGui::lua_OpenPopup},
    {"Selectable", ImGui::lua_Selectable},
    {"IsItemClicked", ImGui::lua_IsItemClicked},
    {"IsItemActive", ImGui::lua_IsItemActive},
    {"SetNextWindowSize", ImGui::lua_SetNextWindowSize},
    {"PushStyleVar", ImGui::lua_PushStyleVar},
    {"IsItemHovered", ImGui::lua_IsItemHovered},
    {"Separator", ImGui::lua_Separator},
    {"ProgressBar", ImGui::lua_ProgressBar},
    {"SameLine", ImGui::lua_SameLine},
    {"BeginChild", ImGui::lua_BeginChild},
    {"ColorEdit4", ImGui::lua_ColorEdit4},
    {"PopStyleColor", ImGui::lua_PopStyleColor},
    {NULL, NULL}  // Sentinel
};

int luaopen_imgui_lib(lua_State *L) {
    luaL_newlibtable(L, imgui_lib);
    luaL_setfuncs(L, imgui_lib, 0);
    lua_setglobal(L, "imgui");
    return 1;
}
