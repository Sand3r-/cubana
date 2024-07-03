
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

static void register_enums(lua_State* L) {
    using namespace ImGui;
    // ImGuiWindowFlags_
    lua_createtable(L, 32, 0);
    lua_pushinteger(L, ImGuiWindowFlags_None);
    lua_setfield(L, -2, "None");
    lua_pushinteger(L, ImGuiWindowFlags_NoTitleBar);
    lua_setfield(L, -2, "NoTitleBar");
    lua_pushinteger(L, ImGuiWindowFlags_NoResize);
    lua_setfield(L, -2, "NoResize");
    lua_pushinteger(L, ImGuiWindowFlags_NoMove);
    lua_setfield(L, -2, "NoMove");
    lua_pushinteger(L, ImGuiWindowFlags_NoScrollbar);
    lua_setfield(L, -2, "NoScrollbar");
    lua_pushinteger(L, ImGuiWindowFlags_NoScrollWithMouse);
    lua_setfield(L, -2, "NoScrollWithMouse");
    lua_pushinteger(L, ImGuiWindowFlags_NoCollapse);
    lua_setfield(L, -2, "NoCollapse");
    lua_pushinteger(L, ImGuiWindowFlags_AlwaysAutoResize);
    lua_setfield(L, -2, "AlwaysAutoResize");
    lua_pushinteger(L, ImGuiWindowFlags_NoBackground);
    lua_setfield(L, -2, "NoBackground");
    lua_pushinteger(L, ImGuiWindowFlags_NoSavedSettings);
    lua_setfield(L, -2, "NoSavedSettings");
    lua_pushinteger(L, ImGuiWindowFlags_NoMouseInputs);
    lua_setfield(L, -2, "NoMouseInputs");
    lua_pushinteger(L, ImGuiWindowFlags_MenuBar);
    lua_setfield(L, -2, "MenuBar");
    lua_pushinteger(L, ImGuiWindowFlags_HorizontalScrollbar);
    lua_setfield(L, -2, "HorizontalScrollbar");
    lua_pushinteger(L, ImGuiWindowFlags_NoFocusOnAppearing);
    lua_setfield(L, -2, "NoFocusOnAppearing");
    lua_pushinteger(L, ImGuiWindowFlags_NoBringToFrontOnFocus);
    lua_setfield(L, -2, "NoBringToFrontOnFocus");
    lua_pushinteger(L, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    lua_setfield(L, -2, "AlwaysVerticalScrollbar");
    lua_pushinteger(L, ImGuiWindowFlags_AlwaysHorizontalScrollbar);
    lua_setfield(L, -2, "AlwaysHorizontalScrollbar");
    lua_pushinteger(L, ImGuiWindowFlags_AlwaysUseWindowPadding);
    lua_setfield(L, -2, "AlwaysUseWindowPadding");
    lua_pushinteger(L, ImGuiWindowFlags_NoNavInputs);
    lua_setfield(L, -2, "NoNavInputs");
    lua_pushinteger(L, ImGuiWindowFlags_NoNavFocus);
    lua_setfield(L, -2, "NoNavFocus");
    lua_pushinteger(L, ImGuiWindowFlags_UnsavedDocument);
    lua_setfield(L, -2, "UnsavedDocument");
    lua_pushinteger(L, ImGuiWindowFlags_NoDocking);
    lua_setfield(L, -2, "NoDocking");
    lua_pushinteger(L, ImGuiWindowFlags_NoNav);
    lua_setfield(L, -2, "NoNav");
    lua_pushinteger(L, ImGuiWindowFlags_NoDecoration);
    lua_setfield(L, -2, "NoDecoration");
    lua_pushinteger(L, ImGuiWindowFlags_NoInputs);
    lua_setfield(L, -2, "NoInputs");
    lua_pushinteger(L, ImGuiWindowFlags_NavFlattened);
    lua_setfield(L, -2, "NavFlattened");
    lua_pushinteger(L, ImGuiWindowFlags_ChildWindow);
    lua_setfield(L, -2, "ChildWindow");
    lua_pushinteger(L, ImGuiWindowFlags_Tooltip);
    lua_setfield(L, -2, "Tooltip");
    lua_pushinteger(L, ImGuiWindowFlags_Popup);
    lua_setfield(L, -2, "Popup");
    lua_pushinteger(L, ImGuiWindowFlags_Modal);
    lua_setfield(L, -2, "Modal");
    lua_pushinteger(L, ImGuiWindowFlags_ChildMenu);
    lua_setfield(L, -2, "ChildMenu");
    lua_pushinteger(L, ImGuiWindowFlags_DockNodeHost);
    lua_setfield(L, -2, "DockNodeHost");
    lua_setglobal(L, "ImGuiWindowFlags");

    // ImGuiInputTextFlags_
    lua_createtable(L, 23, 0);
    lua_pushinteger(L, ImGuiInputTextFlags_None);
    lua_setfield(L, -2, "None");
    lua_pushinteger(L, ImGuiInputTextFlags_CharsDecimal);
    lua_setfield(L, -2, "CharsDecimal");
    lua_pushinteger(L, ImGuiInputTextFlags_CharsHexadecimal);
    lua_setfield(L, -2, "CharsHexadecimal");
    lua_pushinteger(L, ImGuiInputTextFlags_CharsUppercase);
    lua_setfield(L, -2, "CharsUppercase");
    lua_pushinteger(L, ImGuiInputTextFlags_CharsNoBlank);
    lua_setfield(L, -2, "CharsNoBlank");
    lua_pushinteger(L, ImGuiInputTextFlags_AutoSelectAll);
    lua_setfield(L, -2, "AutoSelectAll");
    lua_pushinteger(L, ImGuiInputTextFlags_EnterReturnsTrue);
    lua_setfield(L, -2, "EnterReturnsTrue");
    lua_pushinteger(L, ImGuiInputTextFlags_CallbackCompletion);
    lua_setfield(L, -2, "CallbackCompletion");
    lua_pushinteger(L, ImGuiInputTextFlags_CallbackHistory);
    lua_setfield(L, -2, "CallbackHistory");
    lua_pushinteger(L, ImGuiInputTextFlags_CallbackAlways);
    lua_setfield(L, -2, "CallbackAlways");
    lua_pushinteger(L, ImGuiInputTextFlags_CallbackCharFilter);
    lua_setfield(L, -2, "CallbackCharFilter");
    lua_pushinteger(L, ImGuiInputTextFlags_AllowTabInput);
    lua_setfield(L, -2, "AllowTabInput");
    lua_pushinteger(L, ImGuiInputTextFlags_CtrlEnterForNewLine);
    lua_setfield(L, -2, "CtrlEnterForNewLine");
    lua_pushinteger(L, ImGuiInputTextFlags_NoHorizontalScroll);
    lua_setfield(L, -2, "NoHorizontalScroll");
    lua_pushinteger(L, ImGuiInputTextFlags_AlwaysOverwrite);
    lua_setfield(L, -2, "AlwaysOverwrite");
    lua_pushinteger(L, ImGuiInputTextFlags_ReadOnly);
    lua_setfield(L, -2, "ReadOnly");
    lua_pushinteger(L, ImGuiInputTextFlags_Password);
    lua_setfield(L, -2, "Password");
    lua_pushinteger(L, ImGuiInputTextFlags_NoUndoRedo);
    lua_setfield(L, -2, "NoUndoRedo");
    lua_pushinteger(L, ImGuiInputTextFlags_CharsScientific);
    lua_setfield(L, -2, "CharsScientific");
    lua_pushinteger(L, ImGuiInputTextFlags_CallbackResize);
    lua_setfield(L, -2, "CallbackResize");
    lua_pushinteger(L, ImGuiInputTextFlags_CallbackEdit);
    lua_setfield(L, -2, "CallbackEdit");
    lua_pushinteger(L, ImGuiInputTextFlags_EscapeClearsAll);
    lua_setfield(L, -2, "EscapeClearsAll");
    lua_pushinteger(L, ImGuiInputTextFlags_AlwaysInsertMode);
    lua_setfield(L, -2, "AlwaysInsertMode");
    lua_setglobal(L, "ImGuiInputTextFlags");

    // ImGuiPopupFlags_
    lua_createtable(L, 11, 0);
    lua_pushinteger(L, ImGuiPopupFlags_None);
    lua_setfield(L, -2, "None");
    lua_pushinteger(L, ImGuiPopupFlags_MouseButtonLeft);
    lua_setfield(L, -2, "MouseButtonLeft");
    lua_pushinteger(L, ImGuiPopupFlags_MouseButtonRight);
    lua_setfield(L, -2, "MouseButtonRight");
    lua_pushinteger(L, ImGuiPopupFlags_MouseButtonMiddle);
    lua_setfield(L, -2, "MouseButtonMiddle");
    lua_pushinteger(L, ImGuiPopupFlags_MouseButtonMask_);
    lua_setfield(L, -2, "MouseButtonMask_");
    lua_pushinteger(L, ImGuiPopupFlags_MouseButtonDefault_);
    lua_setfield(L, -2, "MouseButtonDefault_");
    lua_pushinteger(L, ImGuiPopupFlags_NoOpenOverExistingPopup);
    lua_setfield(L, -2, "NoOpenOverExistingPopup");
    lua_pushinteger(L, ImGuiPopupFlags_NoOpenOverItems);
    lua_setfield(L, -2, "NoOpenOverItems");
    lua_pushinteger(L, ImGuiPopupFlags_AnyPopupId);
    lua_setfield(L, -2, "AnyPopupId");
    lua_pushinteger(L, ImGuiPopupFlags_AnyPopupLevel);
    lua_setfield(L, -2, "AnyPopupLevel");
    lua_pushinteger(L, ImGuiPopupFlags_AnyPopup);
    lua_setfield(L, -2, "AnyPopup");
    lua_setglobal(L, "ImGuiPopupFlags");

    // ImGuiSelectableFlags_
    lua_createtable(L, 6, 0);
    lua_pushinteger(L, ImGuiSelectableFlags_None);
    lua_setfield(L, -2, "None");
    lua_pushinteger(L, ImGuiSelectableFlags_DontClosePopups);
    lua_setfield(L, -2, "DontClosePopups");
    lua_pushinteger(L, ImGuiSelectableFlags_SpanAllColumns);
    lua_setfield(L, -2, "SpanAllColumns");
    lua_pushinteger(L, ImGuiSelectableFlags_AllowDoubleClick);
    lua_setfield(L, -2, "AllowDoubleClick");
    lua_pushinteger(L, ImGuiSelectableFlags_Disabled);
    lua_setfield(L, -2, "Disabled");
    lua_pushinteger(L, ImGuiSelectableFlags_AllowItemOverlap);
    lua_setfield(L, -2, "AllowItemOverlap");
    lua_setglobal(L, "ImGuiSelectableFlags");

    // ImGuiComboFlags_
    lua_createtable(L, 9, 0);
    lua_pushinteger(L, ImGuiComboFlags_None);
    lua_setfield(L, -2, "None");
    lua_pushinteger(L, ImGuiComboFlags_PopupAlignLeft);
    lua_setfield(L, -2, "PopupAlignLeft");
    lua_pushinteger(L, ImGuiComboFlags_HeightSmall);
    lua_setfield(L, -2, "HeightSmall");
    lua_pushinteger(L, ImGuiComboFlags_HeightRegular);
    lua_setfield(L, -2, "HeightRegular");
    lua_pushinteger(L, ImGuiComboFlags_HeightLarge);
    lua_setfield(L, -2, "HeightLarge");
    lua_pushinteger(L, ImGuiComboFlags_HeightLargest);
    lua_setfield(L, -2, "HeightLargest");
    lua_pushinteger(L, ImGuiComboFlags_NoArrowButton);
    lua_setfield(L, -2, "NoArrowButton");
    lua_pushinteger(L, ImGuiComboFlags_NoPreview);
    lua_setfield(L, -2, "NoPreview");
    lua_pushinteger(L, ImGuiComboFlags_HeightMask_);
    lua_setfield(L, -2, "HeightMask_");
    lua_setglobal(L, "ImGuiComboFlags");

    // ImGuiHoveredFlags_
    lua_createtable(L, 16, 0);
    lua_pushinteger(L, ImGuiHoveredFlags_None);
    lua_setfield(L, -2, "None");
    lua_pushinteger(L, ImGuiHoveredFlags_ChildWindows);
    lua_setfield(L, -2, "ChildWindows");
    lua_pushinteger(L, ImGuiHoveredFlags_RootWindow);
    lua_setfield(L, -2, "RootWindow");
    lua_pushinteger(L, ImGuiHoveredFlags_AnyWindow);
    lua_setfield(L, -2, "AnyWindow");
    lua_pushinteger(L, ImGuiHoveredFlags_NoPopupHierarchy);
    lua_setfield(L, -2, "NoPopupHierarchy");
    lua_pushinteger(L, ImGuiHoveredFlags_DockHierarchy);
    lua_setfield(L, -2, "DockHierarchy");
    lua_pushinteger(L, ImGuiHoveredFlags_AllowWhenBlockedByPopup);
    lua_setfield(L, -2, "AllowWhenBlockedByPopup");
    lua_pushinteger(L, ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
    lua_setfield(L, -2, "AllowWhenBlockedByActiveItem");
    lua_pushinteger(L, ImGuiHoveredFlags_AllowWhenOverlapped);
    lua_setfield(L, -2, "AllowWhenOverlapped");
    lua_pushinteger(L, ImGuiHoveredFlags_AllowWhenDisabled);
    lua_setfield(L, -2, "AllowWhenDisabled");
    lua_pushinteger(L, ImGuiHoveredFlags_NoNavOverride);
    lua_setfield(L, -2, "NoNavOverride");
    lua_pushinteger(L, ImGuiHoveredFlags_RectOnly);
    lua_setfield(L, -2, "RectOnly");
    lua_pushinteger(L, ImGuiHoveredFlags_RootAndChildWindows);
    lua_setfield(L, -2, "RootAndChildWindows");
    lua_pushinteger(L, ImGuiHoveredFlags_DelayNormal);
    lua_setfield(L, -2, "DelayNormal");
    lua_pushinteger(L, ImGuiHoveredFlags_DelayShort);
    lua_setfield(L, -2, "DelayShort");
    lua_pushinteger(L, ImGuiHoveredFlags_NoSharedDelay);
    lua_setfield(L, -2, "NoSharedDelay");
    lua_setglobal(L, "ImGuiHoveredFlags");

    // ImGuiCol_
    lua_createtable(L, 56, 0);
    lua_pushinteger(L, ImGuiCol_Text);
    lua_setfield(L, -2, "Text");
    lua_pushinteger(L, ImGuiCol_TextDisabled);
    lua_setfield(L, -2, "TextDisabled");
    lua_pushinteger(L, ImGuiCol_WindowBg);
    lua_setfield(L, -2, "WindowBg");
    lua_pushinteger(L, ImGuiCol_ChildBg);
    lua_setfield(L, -2, "ChildBg");
    lua_pushinteger(L, ImGuiCol_PopupBg);
    lua_setfield(L, -2, "PopupBg");
    lua_pushinteger(L, ImGuiCol_Border);
    lua_setfield(L, -2, "Border");
    lua_pushinteger(L, ImGuiCol_BorderShadow);
    lua_setfield(L, -2, "BorderShadow");
    lua_pushinteger(L, ImGuiCol_FrameBg);
    lua_setfield(L, -2, "FrameBg");
    lua_pushinteger(L, ImGuiCol_FrameBgHovered);
    lua_setfield(L, -2, "FrameBgHovered");
    lua_pushinteger(L, ImGuiCol_FrameBgActive);
    lua_setfield(L, -2, "FrameBgActive");
    lua_pushinteger(L, ImGuiCol_TitleBg);
    lua_setfield(L, -2, "TitleBg");
    lua_pushinteger(L, ImGuiCol_TitleBgActive);
    lua_setfield(L, -2, "TitleBgActive");
    lua_pushinteger(L, ImGuiCol_TitleBgCollapsed);
    lua_setfield(L, -2, "TitleBgCollapsed");
    lua_pushinteger(L, ImGuiCol_MenuBarBg);
    lua_setfield(L, -2, "MenuBarBg");
    lua_pushinteger(L, ImGuiCol_ScrollbarBg);
    lua_setfield(L, -2, "ScrollbarBg");
    lua_pushinteger(L, ImGuiCol_ScrollbarGrab);
    lua_setfield(L, -2, "ScrollbarGrab");
    lua_pushinteger(L, ImGuiCol_ScrollbarGrabHovered);
    lua_setfield(L, -2, "ScrollbarGrabHovered");
    lua_pushinteger(L, ImGuiCol_ScrollbarGrabActive);
    lua_setfield(L, -2, "ScrollbarGrabActive");
    lua_pushinteger(L, ImGuiCol_CheckMark);
    lua_setfield(L, -2, "CheckMark");
    lua_pushinteger(L, ImGuiCol_SliderGrab);
    lua_setfield(L, -2, "SliderGrab");
    lua_pushinteger(L, ImGuiCol_SliderGrabActive);
    lua_setfield(L, -2, "SliderGrabActive");
    lua_pushinteger(L, ImGuiCol_Button);
    lua_setfield(L, -2, "Button");
    lua_pushinteger(L, ImGuiCol_ButtonHovered);
    lua_setfield(L, -2, "ButtonHovered");
    lua_pushinteger(L, ImGuiCol_ButtonActive);
    lua_setfield(L, -2, "ButtonActive");
    lua_pushinteger(L, ImGuiCol_Header);
    lua_setfield(L, -2, "Header");
    lua_pushinteger(L, ImGuiCol_HeaderHovered);
    lua_setfield(L, -2, "HeaderHovered");
    lua_pushinteger(L, ImGuiCol_HeaderActive);
    lua_setfield(L, -2, "HeaderActive");
    lua_pushinteger(L, ImGuiCol_Separator);
    lua_setfield(L, -2, "Separator");
    lua_pushinteger(L, ImGuiCol_SeparatorHovered);
    lua_setfield(L, -2, "SeparatorHovered");
    lua_pushinteger(L, ImGuiCol_SeparatorActive);
    lua_setfield(L, -2, "SeparatorActive");
    lua_pushinteger(L, ImGuiCol_ResizeGrip);
    lua_setfield(L, -2, "ResizeGrip");
    lua_pushinteger(L, ImGuiCol_ResizeGripHovered);
    lua_setfield(L, -2, "ResizeGripHovered");
    lua_pushinteger(L, ImGuiCol_ResizeGripActive);
    lua_setfield(L, -2, "ResizeGripActive");
    lua_pushinteger(L, ImGuiCol_Tab);
    lua_setfield(L, -2, "Tab");
    lua_pushinteger(L, ImGuiCol_TabHovered);
    lua_setfield(L, -2, "TabHovered");
    lua_pushinteger(L, ImGuiCol_TabActive);
    lua_setfield(L, -2, "TabActive");
    lua_pushinteger(L, ImGuiCol_TabUnfocused);
    lua_setfield(L, -2, "TabUnfocused");
    lua_pushinteger(L, ImGuiCol_TabUnfocusedActive);
    lua_setfield(L, -2, "TabUnfocusedActive");
    lua_pushinteger(L, ImGuiCol_DockingPreview);
    lua_setfield(L, -2, "DockingPreview");
    lua_pushinteger(L, ImGuiCol_DockingEmptyBg);
    lua_setfield(L, -2, "DockingEmptyBg");
    lua_pushinteger(L, ImGuiCol_PlotLines);
    lua_setfield(L, -2, "PlotLines");
    lua_pushinteger(L, ImGuiCol_PlotLinesHovered);
    lua_setfield(L, -2, "PlotLinesHovered");
    lua_pushinteger(L, ImGuiCol_PlotHistogram);
    lua_setfield(L, -2, "PlotHistogram");
    lua_pushinteger(L, ImGuiCol_PlotHistogramHovered);
    lua_setfield(L, -2, "PlotHistogramHovered");
    lua_pushinteger(L, ImGuiCol_TableHeaderBg);
    lua_setfield(L, -2, "TableHeaderBg");
    lua_pushinteger(L, ImGuiCol_TableBorderStrong);
    lua_setfield(L, -2, "TableBorderStrong");
    lua_pushinteger(L, ImGuiCol_TableBorderLight);
    lua_setfield(L, -2, "TableBorderLight");
    lua_pushinteger(L, ImGuiCol_TableRowBg);
    lua_setfield(L, -2, "TableRowBg");
    lua_pushinteger(L, ImGuiCol_TableRowBgAlt);
    lua_setfield(L, -2, "TableRowBgAlt");
    lua_pushinteger(L, ImGuiCol_TextSelectedBg);
    lua_setfield(L, -2, "TextSelectedBg");
    lua_pushinteger(L, ImGuiCol_DragDropTarget);
    lua_setfield(L, -2, "DragDropTarget");
    lua_pushinteger(L, ImGuiCol_NavHighlight);
    lua_setfield(L, -2, "NavHighlight");
    lua_pushinteger(L, ImGuiCol_NavWindowingHighlight);
    lua_setfield(L, -2, "NavWindowingHighlight");
    lua_pushinteger(L, ImGuiCol_NavWindowingDimBg);
    lua_setfield(L, -2, "NavWindowingDimBg");
    lua_pushinteger(L, ImGuiCol_ModalWindowDimBg);
    lua_setfield(L, -2, "ModalWindowDimBg");
    lua_pushinteger(L, ImGuiCol_COUNT);
    lua_setfield(L, -2, "COUNT");
    lua_setglobal(L, "ImGuiCol");

    // ImGuiStyleVar_
    lua_createtable(L, 29, 0);
    lua_pushinteger(L, ImGuiStyleVar_Alpha);
    lua_setfield(L, -2, "Alpha");
    lua_pushinteger(L, ImGuiStyleVar_DisabledAlpha);
    lua_setfield(L, -2, "DisabledAlpha");
    lua_pushinteger(L, ImGuiStyleVar_WindowPadding);
    lua_setfield(L, -2, "WindowPadding");
    lua_pushinteger(L, ImGuiStyleVar_WindowRounding);
    lua_setfield(L, -2, "WindowRounding");
    lua_pushinteger(L, ImGuiStyleVar_WindowBorderSize);
    lua_setfield(L, -2, "WindowBorderSize");
    lua_pushinteger(L, ImGuiStyleVar_WindowMinSize);
    lua_setfield(L, -2, "WindowMinSize");
    lua_pushinteger(L, ImGuiStyleVar_WindowTitleAlign);
    lua_setfield(L, -2, "WindowTitleAlign");
    lua_pushinteger(L, ImGuiStyleVar_ChildRounding);
    lua_setfield(L, -2, "ChildRounding");
    lua_pushinteger(L, ImGuiStyleVar_ChildBorderSize);
    lua_setfield(L, -2, "ChildBorderSize");
    lua_pushinteger(L, ImGuiStyleVar_PopupRounding);
    lua_setfield(L, -2, "PopupRounding");
    lua_pushinteger(L, ImGuiStyleVar_PopupBorderSize);
    lua_setfield(L, -2, "PopupBorderSize");
    lua_pushinteger(L, ImGuiStyleVar_FramePadding);
    lua_setfield(L, -2, "FramePadding");
    lua_pushinteger(L, ImGuiStyleVar_FrameRounding);
    lua_setfield(L, -2, "FrameRounding");
    lua_pushinteger(L, ImGuiStyleVar_FrameBorderSize);
    lua_setfield(L, -2, "FrameBorderSize");
    lua_pushinteger(L, ImGuiStyleVar_ItemSpacing);
    lua_setfield(L, -2, "ItemSpacing");
    lua_pushinteger(L, ImGuiStyleVar_ItemInnerSpacing);
    lua_setfield(L, -2, "ItemInnerSpacing");
    lua_pushinteger(L, ImGuiStyleVar_IndentSpacing);
    lua_setfield(L, -2, "IndentSpacing");
    lua_pushinteger(L, ImGuiStyleVar_CellPadding);
    lua_setfield(L, -2, "CellPadding");
    lua_pushinteger(L, ImGuiStyleVar_ScrollbarSize);
    lua_setfield(L, -2, "ScrollbarSize");
    lua_pushinteger(L, ImGuiStyleVar_ScrollbarRounding);
    lua_setfield(L, -2, "ScrollbarRounding");
    lua_pushinteger(L, ImGuiStyleVar_GrabMinSize);
    lua_setfield(L, -2, "GrabMinSize");
    lua_pushinteger(L, ImGuiStyleVar_GrabRounding);
    lua_setfield(L, -2, "GrabRounding");
    lua_pushinteger(L, ImGuiStyleVar_TabRounding);
    lua_setfield(L, -2, "TabRounding");
    lua_pushinteger(L, ImGuiStyleVar_ButtonTextAlign);
    lua_setfield(L, -2, "ButtonTextAlign");
    lua_pushinteger(L, ImGuiStyleVar_SelectableTextAlign);
    lua_setfield(L, -2, "SelectableTextAlign");
    lua_pushinteger(L, ImGuiStyleVar_SeparatorTextBorderSize);
    lua_setfield(L, -2, "SeparatorTextBorderSize");
    lua_pushinteger(L, ImGuiStyleVar_SeparatorTextAlign);
    lua_setfield(L, -2, "SeparatorTextAlign");
    lua_pushinteger(L, ImGuiStyleVar_SeparatorTextPadding);
    lua_setfield(L, -2, "SeparatorTextPadding");
    lua_pushinteger(L, ImGuiStyleVar_COUNT);
    lua_setfield(L, -2, "COUNT");
    lua_setglobal(L, "ImGuiStyleVar");

    // ImGuiColorEditFlags_
    lua_createtable(L, 29, 0);
    lua_pushinteger(L, ImGuiColorEditFlags_None);
    lua_setfield(L, -2, "None");
    lua_pushinteger(L, ImGuiColorEditFlags_NoAlpha);
    lua_setfield(L, -2, "NoAlpha");
    lua_pushinteger(L, ImGuiColorEditFlags_NoPicker);
    lua_setfield(L, -2, "NoPicker");
    lua_pushinteger(L, ImGuiColorEditFlags_NoOptions);
    lua_setfield(L, -2, "NoOptions");
    lua_pushinteger(L, ImGuiColorEditFlags_NoSmallPreview);
    lua_setfield(L, -2, "NoSmallPreview");
    lua_pushinteger(L, ImGuiColorEditFlags_NoInputs);
    lua_setfield(L, -2, "NoInputs");
    lua_pushinteger(L, ImGuiColorEditFlags_NoTooltip);
    lua_setfield(L, -2, "NoTooltip");
    lua_pushinteger(L, ImGuiColorEditFlags_NoLabel);
    lua_setfield(L, -2, "NoLabel");
    lua_pushinteger(L, ImGuiColorEditFlags_NoSidePreview);
    lua_setfield(L, -2, "NoSidePreview");
    lua_pushinteger(L, ImGuiColorEditFlags_NoDragDrop);
    lua_setfield(L, -2, "NoDragDrop");
    lua_pushinteger(L, ImGuiColorEditFlags_NoBorder);
    lua_setfield(L, -2, "NoBorder");
    lua_pushinteger(L, ImGuiColorEditFlags_AlphaBar);
    lua_setfield(L, -2, "AlphaBar");
    lua_pushinteger(L, ImGuiColorEditFlags_AlphaPreview);
    lua_setfield(L, -2, "AlphaPreview");
    lua_pushinteger(L, ImGuiColorEditFlags_AlphaPreviewHalf);
    lua_setfield(L, -2, "AlphaPreviewHalf");
    lua_pushinteger(L, ImGuiColorEditFlags_HDR);
    lua_setfield(L, -2, "HDR");
    lua_pushinteger(L, ImGuiColorEditFlags_DisplayRGB);
    lua_setfield(L, -2, "DisplayRGB");
    lua_pushinteger(L, ImGuiColorEditFlags_DisplayHSV);
    lua_setfield(L, -2, "DisplayHSV");
    lua_pushinteger(L, ImGuiColorEditFlags_DisplayHex);
    lua_setfield(L, -2, "DisplayHex");
    lua_pushinteger(L, ImGuiColorEditFlags_Uint8);
    lua_setfield(L, -2, "Uint8");
    lua_pushinteger(L, ImGuiColorEditFlags_Float);
    lua_setfield(L, -2, "Float");
    lua_pushinteger(L, ImGuiColorEditFlags_PickerHueBar);
    lua_setfield(L, -2, "PickerHueBar");
    lua_pushinteger(L, ImGuiColorEditFlags_PickerHueWheel);
    lua_setfield(L, -2, "PickerHueWheel");
    lua_pushinteger(L, ImGuiColorEditFlags_InputRGB);
    lua_setfield(L, -2, "InputRGB");
    lua_pushinteger(L, ImGuiColorEditFlags_InputHSV);
    lua_setfield(L, -2, "InputHSV");
    lua_pushinteger(L, ImGuiColorEditFlags_DefaultOptions_);
    lua_setfield(L, -2, "DefaultOptions_");
    lua_pushinteger(L, ImGuiColorEditFlags_DisplayMask_);
    lua_setfield(L, -2, "DisplayMask_");
    lua_pushinteger(L, ImGuiColorEditFlags_DataTypeMask_);
    lua_setfield(L, -2, "DataTypeMask_");
    lua_pushinteger(L, ImGuiColorEditFlags_PickerMask_);
    lua_setfield(L, -2, "PickerMask_");
    lua_pushinteger(L, ImGuiColorEditFlags_InputMask_);
    lua_setfield(L, -2, "InputMask_");
    lua_setglobal(L, "ImGuiColorEditFlags");

    // ImGuiSliderFlags_
    lua_createtable(L, 7, 0);
    lua_pushinteger(L, ImGuiSliderFlags_None);
    lua_setfield(L, -2, "None");
    lua_pushinteger(L, ImGuiSliderFlags_AlwaysClamp);
    lua_setfield(L, -2, "AlwaysClamp");
    lua_pushinteger(L, ImGuiSliderFlags_Logarithmic);
    lua_setfield(L, -2, "Logarithmic");
    lua_pushinteger(L, ImGuiSliderFlags_NoRoundToFormat);
    lua_setfield(L, -2, "NoRoundToFormat");
    lua_pushinteger(L, ImGuiSliderFlags_NoInput);
    lua_setfield(L, -2, "NoInput");
    lua_pushinteger(L, ImGuiSliderFlags_InvalidMask_);
    lua_setfield(L, -2, "InvalidMask_");
    lua_pushinteger(L, ImGuiSliderFlags_ClampOnInput);
    lua_setfield(L, -2, "ClampOnInput");
    lua_setglobal(L, "ImGuiSliderFlags");

    // ImGuiMouseButton_
    lua_createtable(L, 4, 0);
    lua_pushinteger(L, ImGuiMouseButton_Left);
    lua_setfield(L, -2, "Left");
    lua_pushinteger(L, ImGuiMouseButton_Right);
    lua_setfield(L, -2, "Right");
    lua_pushinteger(L, ImGuiMouseButton_Middle);
    lua_setfield(L, -2, "Middle");
    lua_pushinteger(L, ImGuiMouseButton_COUNT);
    lua_setfield(L, -2, "COUNT");
    lua_setglobal(L, "ImGuiMouseButton");

    // ImGuiCond_
    lua_createtable(L, 5, 0);
    lua_pushinteger(L, ImGuiCond_None);
    lua_setfield(L, -2, "None");
    lua_pushinteger(L, ImGuiCond_Always);
    lua_setfield(L, -2, "Always");
    lua_pushinteger(L, ImGuiCond_Once);
    lua_setfield(L, -2, "Once");
    lua_pushinteger(L, ImGuiCond_FirstUseEver);
    lua_setfield(L, -2, "FirstUseEver");
    lua_pushinteger(L, ImGuiCond_Appearing);
    lua_setfield(L, -2, "Appearing");
    lua_setglobal(L, "ImGuiCond");

}

static const struct luaL_Reg imgui_lib[] = {
    {"BeginCombo", ImGui::lua_BeginCombo},
    {"PopStyleColor", ImGui::lua_PopStyleColor},
    {"PopID", ImGui::lua_PopID},
    {"InputText", ImGui::lua_InputText},
    {"Separator", ImGui::lua_Separator},
    {"Columns", ImGui::lua_Columns},
    {"SameLine", ImGui::lua_SameLine},
    {"Spacing", ImGui::lua_Spacing},
    {"IsItemActive", ImGui::lua_IsItemActive},
    {"Begin", ImGui::lua_Begin},
    {"SliderFloat", ImGui::lua_SliderFloat},
    {"PushStyleColor", ImGui::lua_PushStyleColor},
    {"End", ImGui::lua_End},
    {"EndTooltip", ImGui::lua_EndTooltip},
    {"RadioButton", ImGui::lua_RadioButton},
    {"EndCombo", ImGui::lua_EndCombo},
    {"ProgressBar", ImGui::lua_ProgressBar},
    {"BeginChild", ImGui::lua_BeginChild},
    {"OpenPopup", ImGui::lua_OpenPopup},
    {"CloseCurrentPopup", ImGui::lua_CloseCurrentPopup},
    {"NextColumn", ImGui::lua_NextColumn},
    {"DragFloat", ImGui::lua_DragFloat},
    {"PushID", ImGui::lua_PushID},
    {"InputFloat", ImGui::lua_InputFloat},
    {"BeginListBox", ImGui::lua_BeginListBox},
    {"EndChild", ImGui::lua_EndChild},
    {"IsItemHovered", ImGui::lua_IsItemHovered},
    {"PopStyleVar", ImGui::lua_PopStyleVar},
    {"DragInt", ImGui::lua_DragInt},
    {"BeginPopup", ImGui::lua_BeginPopup},
    {"Checkbox", ImGui::lua_Checkbox},
    {"TextUnformatted", ImGui::lua_TextUnformatted},
    {"BeginTooltip", ImGui::lua_BeginTooltip},
    {"SetNextWindowSize", ImGui::lua_SetNextWindowSize},
    {"EndPopup", ImGui::lua_EndPopup},
    {"IsItemClicked", ImGui::lua_IsItemClicked},
    {"SetNextWindowPos", ImGui::lua_SetNextWindowPos},
    {"PushStyleVar", ImGui::lua_PushStyleVar},
    {"InputInt", ImGui::lua_InputInt},
    {"Button", ImGui::lua_Button},
    {"EndListBox", ImGui::lua_EndListBox},
    {"Selectable", ImGui::lua_Selectable},
    {NULL, NULL}  // Sentinel
};

int luaopen_imgui_lib(lua_State *L) {
    luaL_newlibtable(L, imgui_lib);
    luaL_setfuncs(L, imgui_lib, 0);
    lua_setglobal(L, "ImGui");
    register_enums(L);
    return 1;
}
