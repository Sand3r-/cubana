cmake_minimum_required (VERSION 3.21)

# Define an executable target
project(cubana)
add_executable(cubana)

# Specify the required C standard
set_target_properties(cubana PROPERTIES C_STANDARD 17)
set_target_properties(cubana PROPERTIES CMAKE_C_STANDARD_REQUIRED True)

# Add all of the source code for the exe itself
add_subdirectory(src)