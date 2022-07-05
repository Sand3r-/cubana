cmake_minimum_required (VERSION 3.21)

# Create target
set(TARGET cubana)
add_executable(${TARGET})

# Set debugger working directory
set_property(TARGET ${TARGET} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Specify the required C standard
set_target_properties(${TARGET} PROPERTIES C_STANDARD 17)
set_target_properties(${TARGET} PROPERTIES CMAKE_C_STANDARD_REQUIRED True)

# Specify sources for the target
list(APPEND SOURCES app.c)
list(APPEND SOURCES argparser.c)
list(APPEND SOURCES buffer.c)
list(APPEND SOURCES culibc.c)
list(APPEND SOURCES entity.c)
list(APPEND SOURCES error.c)
list(APPEND SOURCES external.c)
list(APPEND SOURCES file.c)
list(APPEND SOURCES gamepad.c)
list(APPEND SOURCES game.c)
list(APPEND SOURCES input.c)
list(APPEND SOURCES log/log.c)
list(APPEND SOURCES main.c)
list(APPEND SOURCES math/mat.c)
list(APPEND SOURCES math/vec.c)
list(APPEND SOURCES math/scalar.c)
list(APPEND SOURCES memory/stackallocator.c)
list(APPEND SOURCES memory/linearallocator.c)
list(APPEND SOURCES platform.c)
list(APPEND SOURCES renderer/renderer.c)
list(APPEND SOURCES renderer/renderervk.c)
list(APPEND SOURCES renderer/igintegration.c)
list(APPEND SOURCES renderer/i3integration.c)
list(APPEND SOURCES window.c)
target_sources(${TARGET} PRIVATE ${SOURCES})

# Specify libraries for the target
list(APPEND LIBRARIES Vulkan::Vulkan)
list(APPEND LIBRARIES SDL2)
list(APPEND LIBRARIES SDL2main)
list(APPEND LIBRARIES cimgui)
list(APPEND LIBRARIES cim3d)
if (UNIX)
    list(APPEND LIBRARIES m) # Math library
endif (UNIX)


target_link_libraries(${TARGET} ${LIBRARIES})

# Specify include directories for the target
list(APPEND INCLUDES .)
target_include_directories(${TARGET} PRIVATE ${INCLUDES})

# Specify compile definitions
target_compile_definitions(${TARGET} PRIVATE
    CUBANA_DEVELOPMENT USE_VK_RENDERER=1 CIMGUI_DEFINE_ENUMS_AND_STRUCTS )

# For logging purposes define relative file name for each file
define_file_basename_for_sources(${TARGET})
