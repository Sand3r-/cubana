cmake_minimum_required (VERSION 3.21)

set(TARGET cim3d)
add_library(${TARGET} SHARED im3d.cpp cim3d.cpp)
target_include_directories(${TARGET} PRIVATE ../../src PUBLIC .)
