cmake_minimum_required (VERSION 3.21)
project(cubana C)

# Include cmake modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(FileBaseNameUtil)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/lib)
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

# Fix problems with relative paths
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Set-up SDL build
# Define static build-only
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_SHARED ON CACHE BOOL "" FORCE)

# Enable threads for debugging purposes (watchdogs)
set(SDL_THREADS ON CACHE BOOL "" FORCE)

# Remove unnecessary dependencies
set(DISABLED_SDL_SUBSYSTEMS SDL_ATOMIC SDL_POWER
    SDL_CPUINFO SDL_SENSOR SDL_LOCALE SDL_MISC)
foreach(SDL_SUBSYSTEM ${DISABLED_SDL_SUBSYSTEMS})
    set(${SDL_SUBSYSTEM} OFF CACHE BOOL "" FORCE)
endforeach(SDL_SUBSYSTEM DISABLED_SDL_SUBSYSTEMS)

add_subdirectory(external/SDL)
add_subdirectory(external/cimgui)
add_subdirectory(external/im3d)

# Add all of the source code for the exe itself
add_subdirectory(src)

# Copy shaders to binary directory
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/shaders)
file(GLOB FILES "shaders/*.spv")
foreach(FILE ${FILES})
    configure_file(${FILE} ${CMAKE_BINARY_DIR}/bin/shaders COPYONLY)
endforeach(FILE ${FILES})
