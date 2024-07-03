import json
import argparse

def main():
    parser = argparse.ArgumentParser(description="Generate a documentation file from a JSON input.")
    parser.add_argument('-i', '--input', required=True, help='Path to the input JSON file')
    parser.add_argument('-o', '--output', required=True, help='Path to the output file')
    parser.add_argument('-n', '--name', required=True, help='Header name to be included in the output file')

    args = parser.parse_args()

    # Read the input JSON file
    with open(args.input, 'r') as input_file:
        data = json.load(input_file)

    # Create a dictionary to store functions and their line numbers
    functions = {}

    # Process each entry in the JSON data
    for entry in data:
        name = entry['name']
        line = entry['line']
        if name in functions:
            functions[name].append(line)
        else:
            functions[name] = [line]

    # Write the output file
    with open(args.output, 'w') as output_file:
        output_file.write(f"{args.name}\n")
        for name, lines in functions.items():
            lines_str = " ".join(map(str, lines))
            output_file.write(f"{name} {lines_str}\n")

if __name__ == "__main__":
    main()
