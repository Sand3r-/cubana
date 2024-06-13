import json
import clang.cindex

clang.cindex.Config.set_library_path("C:/Users/Sand3r/Documents/Visual Studio 2019/Projects/llvm-project/build/Release/bin")

# Function to parse the header file and return the translation unit
def parse_header_file(filename):
    index = clang.cindex.Index.create()
    args = ['-x', 'c++', '-std=c++11', '-I../external/imgui/']
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
                'parameters': params
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
    input_header = 'imgui.h'  # Change this to your header file name
    output_json_functions = 'functions.json'
    output_json_enums = 'enums.json'

    translation_unit = parse_header_file(input_header)
    functions = extract_functions(translation_unit)
    # Extract enums from the translation unit
    enums = extract_enums(translation_unit)
    enriched_enums = enrich_enum_values(enums)

    save_to_json(functions, output_json_functions)
    save_to_json(enriched_enums, output_json_enums)

    print(f'Function signatures have been saved to {output_json_functions}')
    print(f'Enums have been saved to {output_json_enums}')

if __name__ == '__main__':
    main()
