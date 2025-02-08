# Add a custom target to copy assets directory
add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/assets/fonts
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets/fonts ${CMAKE_BINARY_DIR}/bin/assets/fonts
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/assets/lua_docs
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets/lua_docs ${CMAKE_BINARY_DIR}/bin/assets/lua_docs
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/assets/levels
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets/levels ${CMAKE_BINARY_DIR}/bin/assets/levels
    DEPENDS ${CMAKE_SOURCE_DIR}/assets/fonts ${CMAKE_SOURCE_DIR}/assets/lua_docs ${CMAKE_SOURCE_DIR}/assets/levels
)

# Add a custom target to copy shaders directory
add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/bin/shaders
    DEPENDS ${CMAKE_SOURCE_DIR}/shaders
)

# Add a custom target to copy scripts directory
add_custom_target(copy_scripts ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin/scripts
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/scripts ${CMAKE_BINARY_DIR}/bin/scripts
    DEPENDS ${CMAKE_SOURCE_DIR}/scripts
)

# Add a top-level custom target to invoke both copy targets
add_custom_target(copy_files ALL
    DEPENDS copy_assets copy_shaders copy_scripts
)