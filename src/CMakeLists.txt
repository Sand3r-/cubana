cmake_minimum_required (VERSION 3.21)

# Create target
set(TARGET cubana)
add_executable(${TARGET})

# Set debugger working directory
set_property(TARGET ${TARGET} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Specify the required C standard
set_target_properties(${TARGET} PROPERTIES C_STANDARD 17)
set_target_properties(${TARGET} PROPERTIES CMAKE_C_STANDARD_REQUIRED True)

# Enable warnings and warnings-as-errors
if(MSVC)
    # TODO: Consider enabling at some point
    # target_compile_options(${TARGET} PRIVATE /Wall )
    # target_compile_options(${TARGET} PRIVATE /WX )
    target_compile_options(${TARGET} PRIVATE /wd4820)
    target_compile_options(${TARGET} PRIVATE /wd5045)
    target_compile_options(${TARGET} PRIVATE /wd4668)
    target_compile_options(${TARGET} PRIVATE /wd4061)
endif(MSVC)

# Specify extenral sources for the target
# Import IMGUI_COLOR_TEXT_EDIT_SOURCES and IMGUI_COLOR_TEXT_EDIT_INCLUDE_DIRECTORIES
include(ImGuiColorTextEdit)

# Specify sources for the target
set(SOURCES
    app.c
    argparser.c
    buffer.c
    collision.c
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
    level_loader.c
    main.c
    math/mat.c
    math/vec.c
    math/scalar_impl.c
    memory/arena.c
    memory/thread_scratch.c
    physics.c
    platform.c
    renderer/renderer.c
    renderer/renderervk.c
    renderer/igintegration.c
    renderer/i3integration.c
    renderer/imfont_storage.c
    renderer/vkhelper.c
    replay.c
    script/scripting.c
    script/scriptreg.c
    script/imgui_lua.cpp
    timer.c
    types_str.c
    ui/scripteditor.cpp
    ui/ui.c
    ui/cld_parser.c
    world.c
    window.c
)

if (WIN32)
    list(APPEND SOURCES os/crash_handler_win32.cpp)
    list(APPEND SOURCES os/os_memory_win32.c)
    list(APPEND SOURCES os/os_error_win32.c)
else(WIN32)
    list(APPEND SOURCES os/crash_handler_generic.cpp)
    list(APPEND SOURCES os/os_memory_linux.c)
    list(APPEND SOURCES os/os_error_linux.c)
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
    nfd
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
    CUBANA_DEVELOPMENT
    USE_VK_RENDERER=1
    CIMGUI_DEFINE_ENUMS_AND_STRUCTS
    DEBUG_ARENA_ALLOCATIONS
)

# For logging purposes define relative file name for each file
define_file_basename_for_sources(${TARGET})
