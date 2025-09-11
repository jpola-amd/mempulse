function(_copy_file_to_target target thefile)
    add_custom_command(
        TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${thefile}
        $<TARGET_FILE_DIR:${target}>
        VERBATIM
    )
endfunction()
