# Add a custom target to copy shaders directory
add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_SOURCE_DIR}/shaders
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders
)

# Add a custom target to copy scripts directory
add_custom_target(copy_scripts ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/scripts ${CMAKE_BINARY_DIR}/bin/scripts
    DEPENDS ${CMAKE_SOURCE_DIR}/scripts
)

# Add a top-level custom target to invoke both copy targets
add_custom_target(copy_files ALL
    DEPENDS copy_shaders copy_scripts
)