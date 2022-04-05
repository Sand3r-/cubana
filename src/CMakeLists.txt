cmake_minimum_required (VERSION 3.21)

# Create target
set(TARGET cubana)
add_executable(${TARGET})

# Specify the required C standard
set_target_properties(cubana PROPERTIES C_STANDARD 17)
set_target_properties(cubana PROPERTIES CMAKE_C_STANDARD_REQUIRED True)

# Specify sources for the target
list(APPEND SOURCES main.c)
list(APPEND SOURCES app.c)
list(APPEND SOURCES errorcodes.c)
list(APPEND SOURCES argparser.c)
list(APPEND SOURCES file.c)
list(APPEND SOURCES log/log.c)
target_sources(${TARGET} PRIVATE ${SOURCES})

# Specify libraries for the target
list(APPEND LIBRARIES SDL2)
list(APPEND LIBRARIES SDL2main)
target_link_libraries(${TARGET} ${LIBRARIES})
