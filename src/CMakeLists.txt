cmake_minimum_required (VERSION 3.21)

# Create target
set(TARGET cubana)
add_executable(${TARGET})

# Set debugger working directory
set_property(TARGET ${TARGET} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Specify the required C standard
set_target_properties(${TARGET} PROPERTIES C_STANDARD 17)
set_target_properties(${TARGET} PROPERTIES CMAKE_C_STANDARD_REQUIRED True)

# Specify extenral sources for the target
# Import IMGUI_COLOR_TEXT_EDIT_SOURCES and IMGUI_COLOR_TEXT_EDIT_INCLUDE_DIRECTORIES
include(ImGuiColorTextEdit)

# Specify sources for the target
set(SOURCES
    app.c
    argparser.c
    buffer.c
    culibc.c
    entity.c
    error.c
    event.c
    external.c
    file.c
    gamepad.c
    game.c
    input.c
    log/log.c
    main.c
    math/mat.c
    math/vec.c
    math/scalar.c
    memory/stackallocator.c
    memory/linearallocator.c
    platform.c
    renderer/renderer.c
    renderer/renderervk.c
    renderer/igintegration.c
    renderer/i3integration.c
    script/scripting.c
    script/scriptreg.c
    script/imgui_registrar.cpp
    timer.c
    ui/scripteditor.cpp
    ui/ui.c
    window.c
)

if (WIN32)
    list(APPEND SOURCES os/crash_handler_win32.cpp)
else(WIN32)
    list(APPEND SOURCES os/crash_handler_generic.cpp)
endif(WIN32)

target_sources(${TARGET} PRIVATE ${SOURCES} ${IMGUI_COLOR_TEXT_EDIT_SOURCES})
target_include_directories(${TARGET} PRIVATE ${IMGUI_COLOR_TEXT_EDIT_INCLUDE_DIRECTORIES})

# Specify libraries for the target
set(LIBRARIES
    Vulkan::Vulkan
    SDL2
    SDL2main
    cimgui
    cim3d
    lua_shared
)

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
