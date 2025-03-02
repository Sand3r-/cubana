cmake_minimum_required(VERSION 3.10)
project(UnityTests C)

# Not a top-notch practice, but should be fine here
file(GLOB TEST_SOURCES "test*.c")

foreach(TEST_SRC ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SRC})
    target_link_libraries(${TEST_NAME} Unity)
endforeach()
