# Helper function to add preprocesor definition of FILE_BASENAME
# to pass the filename without directory path for debugging use.
#
# Example:
#
#   define_file_basename_for_sources(my_target)
#
# Will add -DFILE_BASENAME="filename" for each source file depended on
# by my_target, where filename is the name of the file.
#
# Source: https://stackoverflow.com/a/27990434 author: Colin D Bennett
function(define_file_basename_for_sources targetname)
    get_target_property(SOURCE_FILES "${targetname}" SOURCES)
    foreach(SOURCE_FILE ${SOURCE_FILES})
        # Get source file's current list of compile definitions.
        get_property(DEFS SOURCE "${SOURCE_FILE}"
            PROPERTY COMPILE_DEFINITIONS)
        # Add the FILE_BASENAME=filename compile definition to the list.
        get_filename_component(BASENAME "${SOURCE_FILE}" NAME)
        list(APPEND DEFS "FILE_BASENAME=\"${BASENAME}\"")
        # Set the updated compile definitions on the source file.
        set_property(
            SOURCE "${SOURCE_FILE}"
            PROPERTY COMPILE_DEFINITIONS ${DEFS})
    endforeach()
endfunction()