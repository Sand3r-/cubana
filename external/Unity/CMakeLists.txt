cmake_minimum_required(VERSION 3.10)

project(UnityLibrary C)
add_library(Unity STATIC unity.c)
target_include_directories(Unity PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
