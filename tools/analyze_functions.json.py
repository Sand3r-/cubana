import json
from collections import defaultdict, Counter

# Read and parse the JSON file
with open('functions.json', 'r') as file:
    functions = json.load(file)

# Initialize counters
arg_count = Counter()
type_counter = Counter()

# Analyze the functions
for func in functions:
    num_params = len(func['parameters'])
    arg_count[num_params] += 1
    for param in func['parameters']:
        type_counter[param['type']] += 1

# Print statistics
print("Number of functions by argument count:")
for num_args, count in sorted(arg_count.items()):
    print(f"{num_args} arguments: {count} functions")

print("\nTypes and their occurrences:")
for param_type, count in type_counter.most_common():
    print(f"{param_type}: {count} occurrences")
