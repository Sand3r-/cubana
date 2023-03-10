cmake_minimum_required (VERSION 3.21)

# Create target
set(TARGET cimgui)

add_library(${TARGET} SHARED)

list(APPEND SOURCES cimgui.cpp)
list(APPEND SOURCES ../imgui/imgui.cpp)
list(APPEND SOURCES ../imgui/imgui_draw.cpp)
list(APPEND SOURCES ../imgui/imgui_demo.cpp)
list(APPEND SOURCES ../imgui/imgui_widgets.cpp)
list(APPEND SOURCES ../imgui/imgui_tables.cpp)
list(APPEND SOURCES ../imgui/backends/imgui_impl_vulkan.cpp)
list(APPEND SOURCES ../imgui/backends/imgui_impl_sdl2.cpp)

target_include_directories(${TARGET} PUBLIC ../imgui)
if (WIN32)
	target_compile_definitions(${TARGET} PRIVATE "-DIMGUI_API=__declspec\(dllexport\)")
	target_compile_definitions(${TARGET} PRIVATE "-DIMGUI_IMPL_API=extern \"C\" __declspec\(dllexport\)")
else(WIN32)
	target_compile_definitions(${TARGET} PRIVATE "-DIMGUI_IMPL_API=extern \"C\" ")
endif(WIN32)
target_sources(${TARGET} PRIVATE ${SOURCES})
target_include_directories(${TARGET} PUBLIC .)
target_compile_definitions(${TARGET} PUBLIC CIMGUI_USE_SDL CIMGUI_USE_VULKAN)
target_link_libraries(${TARGET} PRIVATE Vulkan::Vulkan SDL2)
