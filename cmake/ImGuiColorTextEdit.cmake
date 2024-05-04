# This file contains list of sources comprising ImGuiColorTextEdit
# The variable IMGUI_COLOR_TEXT_EDIT_SOURCES contains all files
# needed to link to this imgui extension, while
# IMGUI_COLOR_TEXT_EDIT_INCLUDE_DIRECTORIES contains paths to all
# include directories neccessary.

set(IMGUI_COLOR_TEXT_EDIT_INCLUDE_DIRECTORIES
    ${CMAKE_SOURCE_DIR}/external/ImGuiColorTextEdit
)

set(IMGUI_COLOR_TEXT_EDIT_SOURCES
    ${CMAKE_SOURCE_DIR}/external/ImGuiColorTextEdit/LanguageDefinitions.cpp
    ${CMAKE_SOURCE_DIR}/external/ImGuiColorTextEdit/TextEditor.cpp
    ${CMAKE_SOURCE_DIR}/external/ImGuiColorTextEdit/TextEditor.h
)