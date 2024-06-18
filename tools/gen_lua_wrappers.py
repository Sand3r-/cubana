"""
    Cubana lua bindings generator
    Copyright (c) 2024 Michał Gallus

    Generates C++ bindings for lua functions listed in functions_to_gen.txt.
    Assumes the Current Working Directory (cwd) to be the tools/ dir.

    It's a 2nd stage generator which fetches function signature data from function.json
    to generate the bindings.
    The file 'functions.json' can be obtained by running gen_json_def.py (requires clang
    as well as its python bindings)
"""
import json
import re
from collections import defaultdict

# Define the type conversion mappings
lua_to_cpp = {
    "bool": ("lua_toboolean", "lua_pushboolean", "lua_isboolean"),
    "float": ("luaL_checknumber", "lua_pushnumber", "lua_isnumber"),
    "double": ("luaL_checknumber", "lua_pushnumber", "lua_isnumber"),
    "size_t": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "int": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "unsigned int": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "const char *": ("luaL_checkstring", "lua_pushstring", "lua_isstring"),
    "ImGuiSliderFlags": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "ImGuiInputTextFlags": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "ImGuiTreeNodeFlags": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "ImGuiPopupFlags": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "ImGuiWindowFlags": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "ImGuiColorEditFlags": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "ImGuiSelectableFlags": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "ImGuiTabItemFlags": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "ImGuiDockNodeFlags": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "ImGuiDragDropFlags": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "ImGuiTableFlags": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "ImGuiTableRowFlags": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "ImGuiTableColumnFlags": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "ImGuiTabBarFlags": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "ImGuiHoveredFlags": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "ImGuiMouseButton": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "ImU32": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "ImGuiID": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "ImGuiCol": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "ImGuiStyleVar": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "ImGuiCond": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
    "ImGuiComboFlags": ("luaL_checkinteger", "lua_pushinteger", "lua_isinteger"),
}

special_types = {
    "ImVec2": ("lua_istable", "float", 2),
    "ImVec4": ("lua_istable", "float", 4),
    "char *": ("lua_isstring", "char", None)
}

used_enums = set()
registered_functions = set()
unsupported_types = set()
functions_not_generated = []

def parse_json(json_file):
    with open(json_file, 'r') as f:
        return json.load(f)

def parse_functions_to_generate(txt_file):
    with open(txt_file, 'r') as f:
        return set(line.strip() for line in f.readlines())

def generate_header():
    return """
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
    """


def generate_get_set_global_functions(generated_wrappers):
    functions = []
    wrappers_code = "\n".join(generated_wrappers)

    # Generate Get and Set functions for each supported type
    for cpp_type, (lua_check, lua_push, _) in lua_to_cpp.items():
        cpp_type_no_space = cpp_type.replace(" ", "")
        get_func_name = f"GetGlobal{cpp_type_no_space}"
        set_func_name = get_func_name.replace("Get", "Set")
        if not (get_func_name in wrappers_code or set_func_name in wrappers_code):
            continue
        functions.append(f"""
static inline {cpp_type} {get_func_name}(lua_State* L, const char* name) {{
    lua_getglobal(L, name);
    {cpp_type} value = ({cpp_type}){lua_check}(L, -1);
    lua_pop(L, 1);
    return value;
}}

static inline void {set_func_name}(lua_State* L, const char* name, {cpp_type} value) {{
    {lua_push}(L, value);
    lua_setglobal(L, name);
}}
""")

    # Handle special types like ImVec2, ImVec4
    for special_type, (table_check, base_type, count) in special_types.items():
        if not (table_check == "lua_istable"):
            continue
        get_func_name = f"GetGlobal{special_type}"
        set_func_name = get_func_name.replace("Get", "Set")

        if not (get_func_name in wrappers_code or set_func_name in wrappers_code):
            continue
        functions.append(f"""
static inline {special_type} {get_func_name}(lua_State* L, const char* name) {{
    {special_type} value;
    lua_getglobal(L, name);
    if (lua_istable(L, -1)) {{
""")
        dims = ["x", "y", "z", "w"]
        for index, dim in enumerate(dims[:int(special_type[-1:])]):
            functions[-1] += (f"""        lua_pushnumber(L, {index + 1});
        lua_gettable(L, -2);
        value.{dim} = ({base_type})luaL_checknumber(L, -1);
        lua_pop(L, 1);
""")
        functions[-1] += (f"""    }}
    lua_pop(L, 1);
    return value;
}}


static inline void {set_func_name}(lua_State* L, const char* name, {special_type} value) {{
    lua_newtable(L);
""")
        for index, dim in enumerate(dims[:int(special_type[-1:])]):
            functions[-1] += (f"""    lua_pushnumber(L, {index + 1});
    lua_pushnumber(L, value.{dim});
    lua_settable(L, -3);
""")
        functions[-1] += (f"""    lua_setglobal(L, name);
}}
""")

    # Handle char *
    functions.append(f"""
static inline void GetGlobalCharArray(lua_State* L, const char* name, char* value, size_t size) {{
    lua_getglobal(L, name);
    strncpy(value, luaL_checkstring(L, -1), size);
    lua_pop(L, 1);
}}

static inline void SetGlobalCharArray(lua_State* L, const char* name, const char* value) {{
    lua_pushstring(L, value);
    lua_setglobal(L, name);
}}
""")

    return "\n".join(functions)

def update_globals(wrapper, parameters):
    # Update globals before returning
    for param in parameters:
        param_type = param['type']
        param_name = param['name']
        if re.match(r'.+\*$', param_type) or re.match(r'.+&$', param_type):
            if "const" in param_type:
                continue
            type_no_spaces = param_type[:-1].replace(" ", "")
            if "char" in type_no_spaces:
                wrapper.append(f"           SetGlobalCharArray(L, {param_name}_str, {param_name});")
            else:
                wrapper.append(f"           SetGlobal{type_no_spaces}(L, {param_name}_str, {param_name});")
    return wrapper

def append_call_and_return(wrapper, func, func_name, arg_num, parameters):
    # Call the actual function
    wrapper.append(f"        if (num_args == {arg_num}) {{")
    param_list = []
    for param in parameters:
        if "*" in param["type"] and not "char" in param["type"]:
            param_list.append("&" + param['name'])
        else:
            param_list.append(param['name'])
    param_list = ", ".join(param_list[:arg_num])
    if func['return_type'] != "void":
        wrapper.append(f"           {func['return_type']} result = {func_name}({param_list});")
        wrapper = update_globals(wrapper, parameters[:arg_num])
        if func['return_type'] in lua_to_cpp:
            _, lua_push, _ = lua_to_cpp[func['return_type']]
            wrapper.append(f"           {lua_push}(L, result);")
            wrapper.append(f"           return 1;")
        else:
            unsupported_types.add(func['return_type'])
            functions_not_generated.append(func_name)
            return ""
    else:
        wrapper.append(f"           {func_name}({param_list});")
        wrapper = update_globals(wrapper, parameters)
        wrapper.append(f"           return 0;")
    wrapper.append(f"        }}")

def generate_wrapper_function(func_name, overloads):
    wrapper = []

    wrapper.append(f"static int lua_{func_name}(lua_State* L) {{")
    wrapper.append(f"    int num_args = lua_gettop(L);")

    for i, func in enumerate(overloads):
        conditions = []
        parameters = func['parameters']

        max_arg_num = len(parameters)

        # Check the number of arguments
        conditions.append(f"num_args <= {max_arg_num}")

        required_param_count = 0
        # Check the types of arguments
        for j, param in enumerate(parameters):
            param_type = param['type']
            required_param_count += param["default_value"] is None
            if param_type in lua_to_cpp:
                _, _, lua_check = lua_to_cpp[param_type]
                conditions.append(f"{lua_check}(L, {j + 1})")
                if "ImGui" in param_type and lua_to_cpp[param_type][2] == "lua_isinteger":
                    used_enums.add(param_type)
            elif param_type in special_types:
                conditions.append(f"{special_types[param_type][0]}(L, {j + 1})")
            elif "void" in param_type:
                max_arg_num = j
                break
            elif re.match(r'.+\*$', param_type) or re.match(r'.+&$', param_type):
                conditions.append(f"lua_isstring(L, {j + 1})")
            elif re.match(r'.+\[\d+\]$', param_type):
                conditions.append(f"lua_istable(L, {j + 1})")
            elif param["default_value"] is not None:
                max_arg_num = j
                break
            else:
                unsupported_types.add(param_type)
                functions_not_generated.append(func_name)
                return ""

        # Can happen when an unsupported type in encountered
        # that doesn't occur in all overloads
        if max_arg_num < required_param_count:
            continue

        conditions.insert(1, f"num_args >= {required_param_count}")
        wrapper.append(f"    if ({' && '.join(conditions[:2 + required_param_count])}) {{")

        if required_param_count == 0:
            append_call_and_return(wrapper, func, func_name, 0, parameters)

        # Extract the parameters
        for j, param in enumerate(parameters):
            if j >= max_arg_num:
                break
            param_type = param['type']
            param_name = param['name']

            if param_type in lua_to_cpp:
                lua_check, lua_push, _ = lua_to_cpp[param_type]
                wrapper.append(f"        {param_type} {param_name} = ({param_type}){lua_check}(L, {j + 1});")
            elif param_type in special_types:
                special = special_types[param_type]
                if special[0] == "lua_istable":
                    wrapper.append(f"        {param_type} {param_name};")
                    for k in range(special[2]):
                        wrapper.append(f"        lua_pushnumber(L, {k + 1});")
                        wrapper.append(f"        lua_gettable(L, {j + 1});")
                        wrapper.append(f"        {param_name}.{['x', 'y', 'z', 'w'][k]} = ({special[1]})luaL_checknumber(L, -1);")
                        wrapper.append(f"        lua_pop(L, 1);")
                else:
                    param_type = param_type[:-1] # strip *
                    wrapper.append(f"        {param_type} {param_name}[MAX_STR_BUFFER_SIZE] = {{0}};")
                    wrapper.append(f"        const char* {param_name}_str = luaL_checkstring(L, {j + 1});")
                    wrapper.append(f"        GetGlobalCharArray(L, {param_name}_str, {param_name}, MAX_STR_BUFFER_SIZE);")
            elif re.match(r'.+\*$', param_type) or re.match(r'.+&$', param_type):
                wrapper.append(f"        const char* {param_name}_str = luaL_checkstring(L, {j + 1});")
                type_no_spaces = param_type[:-1].replace(" ", "").replace("const", "")
                wrapper.append(f"        {param_type[:-1]} {param_name} = GetGlobal{type_no_spaces}(L, {param_name}_str);")
            elif re.match(r'.+\[\d+\]$', param_type):
                base_type = param_type.split('[')[0]
                size = int(re.findall(r'\[(\d+)\]', param_type)[0])
                wrapper.append(f"        {base_type} {param_name}[{size}];")
                for k in range(size):
                    wrapper.append(f"        lua_pushnumber(L, {k + 1});")
                    wrapper.append(f"        lua_gettable(L, {j + 1});")
                    if base_type in lua_to_cpp:
                        lua_check, _, _ = lua_to_cpp[base_type]
                        wrapper.append(f"        {param_name}[{k}] = ({base_type}){lua_check}(L, -1);")
                    else:
                        unsupported_types.add(base_type)
                        functions_not_generated.append(func_name)
                        return ""
                    wrapper.append(f"        lua_pop(L, 1);")
            else:
                unsupported_types.add(param_type)
                functions_not_generated.append(func_name)
                return ""

            if j + 1 >= required_param_count:
                append_call_and_return(wrapper, func, func_name, j+1, parameters)

        wrapper.append(f"    }}")

    wrapper.append(f"    L_WARN(\"No matching overload found for {func_name}\");")
    wrapper.append(f"    return 0;")
    wrapper.append("}")

    registered_functions.add(func_name)

    return "\n".join(wrapper)

def generate_enum_reg_function(enums):
    lines = ["\n\nstatic void register_enums(lua_State* L) {"]
    lines.append("    using namespace ImGui;")
    for enum in enums:
        # Omit _ at the end of enum names
        if enum["name"][:-1] not in used_enums:
            continue
        lines.append(f"    // {enum['name']}")
        lines.append(f"    lua_createtable(L, {len(enum['constants'])}, 0);")
        for constant in enum["constants"]:
            lines.append(f"    lua_pushinteger(L, {constant['name']});")
            lines.append(f"    lua_setfield(L, -2, \"{constant['name'].replace(enum['name'], '')}\");")
        lines.append(f"    lua_setglobal(L, \"{enum['name'][:-1]}\");\n")
    lines.append("}\n")

    return "\n".join(lines)


def generate_lua_registration():
    registration = []
    for function in registered_functions:
        registration.append(f'    {{"{function}", ImGui::lua_{function}}}')

    registration_str = ",\n".join(registration)

    lua_registration = f"""
static const struct luaL_Reg imgui_lib[] = {{
{registration_str},
    {{NULL, NULL}}  // Sentinel
}};

int luaopen_imgui_lib(lua_State *L) {{
    luaL_newlibtable(L, imgui_lib);
    luaL_setfuncs(L, imgui_lib, 0);
    lua_setglobal(L, "ImGui");
    register_enums(L);
    return 1;
}}
"""
    return lua_registration

def main():
    functions = parse_json('functions.json')
    enums = parse_json('enums.json')
    functions_to_generate = parse_functions_to_generate('functions_to_gen.txt')

    generated_wrappers = []
    function_groups = defaultdict(list)

    for func in functions:
        function_groups[func['name']].append(func)

    for func_name, func_list in function_groups.items():
        if func_name in functions_to_generate:
            wrapper = generate_wrapper_function(func_name, func_list)
            if wrapper:
                generated_wrappers.append(wrapper)

    with open('../src/script/imgui_lua.cpp', 'w') as f:
        f.write(generate_header())
        f.write(generate_get_set_global_functions(generated_wrappers))
        f.write("\n\n")
        f.write("\n\n".join(generated_wrappers))
        f.write("\n} // namespace ImGui")
        f.write(generate_enum_reg_function(enums))
        f.write(generate_lua_registration())

    if unsupported_types:
        print("Unsupported types encountered:")
        for type in unsupported_types:
            print(f"- {type}")

if __name__ == "__main__":
    main()
