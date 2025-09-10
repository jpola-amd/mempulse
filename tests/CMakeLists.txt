add_executable(mempulse_test test_mempulse.cpp)
target_link_libraries(mempulse_test mempulse)
if (HIP_PLATFORM STREQUAL "amd")
    target_compile_definitions(mempulse_test PRIVATE __HIP_PLATFORM_AMD__)
elseif (HIP_PLATFORM STREQUAL "nvidia")
    target_compile_definitions(mempulse_test PRIVATE __HIP_PLATFORM_NVIDIA__)
else()
    message(FATAL_ERROR "Unsupported HIP platform: ${HIP_PLATFORM}. Supported platforms are AMD and NVIDIA.")
endif()

install(TARGETS mempulse_test
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

