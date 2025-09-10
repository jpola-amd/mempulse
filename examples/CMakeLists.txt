if(WIN32)
    add_executable(mempulse_example mempulse_basic.cpp)
    target_link_libraries(mempulse_example mempulse)
    if (HIP_PLATFORM STREQUAL "AMD")
        target_compile_definitions(mempulse_example PRIVATE __HIP_PLATFORM_AMD__)
    elseif (HIP_PLATFORM STREQUAL "NVIDIA")
        target_compile_definitions(mempulse_example PRIVATE __HIP_PLATFORM_NVIDIA__)
    else()
        message(FATAL_ERROR "Unsupported HIP platform: ${HIP_PLATFORM}. Supported platforms are AMD and NVIDIA.")
    endif()

    add_executable(mempulse_pqc mempulse_pqc.cpp)
    target_link_libraries(mempulse_pqc mempulse pdh)
    if (HIP_PLATFORM STREQUAL "AMD")
        target_compile_definitions(mempulse_pqc PRIVATE __HIP_PLATFORM_AMD__)
    elseif (HIP_PLATFORM STREQUAL "NVIDIA")
        target_compile_definitions(mempulse_pqc PRIVATE __HIP_PLATFORM_NVIDIA__)
    else()
        message(FATAL_ERROR "Unsupported HIP platform: ${HIP_PLATFORM}. Supported platforms are AMD and NVIDIA.")
    endif()

    add_executable(mempulse_d3dkmtq mempulse_d3dkmtq.cpp)
    target_link_libraries(mempulse_d3dkmtq mempulse)

    add_executable(mempulse_d3dkmtq2 mempulse_d3dkmtq2.cpp)
    target_link_libraries(mempulse_d3dkmtq2 mempulse)

    add_executable(mempulse_d3dkmtq3 mempulse_d3dkmtq3.cpp)
    target_link_libraries(mempulse_d3dkmtq3 mempulse)

    add_executable(mempulse_d3dkmtq4 mempulse_d3dkmtq4.cpp)
    target_link_libraries(mempulse_d3dkmtq4 mempulse)


	install(TARGETS mempulse_example
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

endif() # WIN32

