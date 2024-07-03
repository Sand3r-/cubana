import json
import clang.cindex
import argparse

clang.cindex.Config.set_library_path("C:/llvm/bin")

# Function to parse the header file and return the translation unit
def parse_header_file(filename, include_paths):
    index = clang.cindex.Index.create()
    args = ['-x', 'c++', '-std=c++11']
    for include_path in include_paths:
        args.append("-I" + include_path)
    return index.parse(filename, args=args)

# Function to extract function signatures from the translation unit
def extract_functions(translation_unit):
    functions = []

    def traverse(cursor):
        if cursor.location.file and cursor.location.file.name != 'imgui.h':
            return
        if cursor.kind == clang.cindex.CursorKind.FUNCTION_DECL:
            # Extract function signature
            return_type = cursor.result_type.spelling
            func_name = cursor.spelling
            params = []
            line_number = cursor.location.line
            for arg in cursor.get_arguments():
                param_type = arg.type.spelling
                param_name = arg.spelling
                param_default_value = None
                # Extract default value if available
                for token in arg.get_tokens():
                    if token.spelling == '=':
                        param_default_value = next(arg.get_tokens()).spelling
                        break
                params.append({
                    'type': param_type,
                    'name': param_name,
                    'default_value': param_default_value
                })
            functions.append({
                'name': func_name,
                'return_type': return_type,
                'parameters': params,
                'line': line_number
            })

        # Recursively traverse children
        for child in cursor.get_children():
            traverse(child)

    # Start traversal from the translation unit's cursor
    traverse(translation_unit.cursor)

    return functions


def extract_enums(translation_unit):
    enums = []

    def traverse(cursor):
        if cursor.location.file and cursor.location.file.name != 'imgui.h':
            return
        if cursor.kind == clang.cindex.CursorKind.ENUM_DECL:
            enum_name = cursor.spelling
            enum_constants = []
            for enum_constant in cursor.get_children():
                if enum_constant.kind == clang.cindex.CursorKind.ENUM_CONSTANT_DECL:
                    constant_name = enum_constant.spelling
                    tokens = list(enum_constant.get_tokens())
                    expression_str = ""
                    if len(tokens) > 1:
                        expression_str = " ".join([t.spelling for t in tokens[2:]])
                    enum_constants.append({
                        'name': constant_name,
                        'expression': expression_str
                    })
            if enum_constants:
                enums.append({
                    'name': enum_name,
                    'constants': enum_constants
                })
        for child in cursor.get_children():
            traverse(child)
    traverse(translation_unit.cursor)
    return enums

def evaluate_expression(expression, constants):
    if not expression:
        return None
    try:
        evaluated_value = eval(expression, {}, constants)
        return evaluated_value
    except Exception as e:
        print(f"Error evaluating expression '{expression}': {e}")
        return None

def enrich_enum_values(enums):
    constants = {}
    for enum in enums:
        base = -1
        for constant in enum['constants']:
            if constant['expression']:
                value = evaluate_expression(constant['expression'], constants)
                base = value
            else:
                value = base + 1 # Default to sequential values if no expression
                base = value
            constants[constant['name']] = value
            constant['value'] = value
    return enums

# Function to save the extracted data to JSON files
def save_to_json(data, output_file):
    with open(output_file, 'w') as json_file:
        json.dump(data, json_file, indent=4)

# Main function
def main():
    parser = argparse.ArgumentParser(description='Extract functions and enum data from a C/C++ file to json files.')
    parser.add_argument('-p', '--libclang_path', type=str, help='Path to libclang binary', default="C:/llvm/bin")
    parser.add_argument('-i', '--input', type=str, help='The input file pattern containing the inference times', default='imgui.h')
    parser.add_argument('-I', '--include_paths', action="extend", nargs="+", type=str, help='Include paths', default=["../external/imgui/"])
    parser.add_argument('-f', '--output_functions', type=str, default="functions.json")
    parser.add_argument('-e', '--output_enums', type=str, default="enums.json")
    args = parser.parse_args()

    clang.cindex.Config.set_library_path(args.libclang_path)

    translation_unit = parse_header_file(args.input, args.include_paths)
    functions = extract_functions(translation_unit)
    # Extract enums from the translation unit
    enums = extract_enums(translation_unit)
    enriched_enums = enrich_enum_values(enums)

    save_to_json(functions, args.output_functions)
    save_to_json(enriched_enums, args.output_enums)

    print(f'Function signatures have been saved to {args.output_functions}')
    print(f'Enums have been saved to {args.output_enums}')

if __name__ == '__main__':
    main()
