cmake_minimum_required(VERSION 3.16)
project(MemPulse VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_EXAMPLES "Build basic example" ON)
option(BUILD_TESTS "Build tests" OFF)
set(HIP_PLATFORM "AMD" CACHE STRING "Choose between AMD and NVIDIA")

include(${CMAKE_SOURCE_DIR}/cmake/FindHIP.cmake)

if(ROCM_PATH)
    set(HIP_DIR ${ROCM_PATH})
    message(STATUS "Using ROCM_PATH from CMake variable: ${HIP_DIR}")
elseif(DEFINED ENV{ROCM_PATH})
    set(HIP_DIR $ENV{ROCM_PATH})
    message(STATUS "Using ROCM_PATH from environment: ${HIP_DIR}")
else()
    if(WIN32)
        set(HIP_DIR "C:/opt/rocm")
    else()
        set(HIP_DIR "/opt/rocm")
    endif()
    message(STATUS "Using default ROCM_PATH: ${HIP_DIR}")
endif()

AddHIP(HIP_DIR ${HIP_DIR})

include_directories(${CMAKE_SOURCE_DIR}/include)

# Platform-specific libraries
if(WIN32)
    set(DXGI_LIB dxgi)
    set(SETUPAPI_LIB setupapi)
endif()


set(SOURCES
    include/mempulse/mempulse.h
    include/mempulse/memory_reporter.h
    include/mempulse/memory_reporter_factory.h
    include/mempulse/config.h
    src/memory_reporter.cpp
    src/memory_reporter_factory.cpp
    src/assert.h
)

# # Platform specific SOURCES
if(WIN32)
    list(APPEND SOURCES
        src/platform/windows/d3dkmt_memory_reporter.cpp
        src/platform/windows/d3dkmt_memory_reporter.h
        src/platform/linux/hip_memory_reporter.cpp
        src/platform/linux/hip_memory_reporter.h
    )
else()
    list(APPEND SOURCES
        src/platform/linux/hip_memory_reporter.cpp
        src/platform/linux/hip_memory_reporter.h
    )
endif()


add_library(mempulse ${SOURCES})
target_link_libraries(mempulse hip)
target_compile_definitions(mempulse PRIVATE
    $<$<CONFIG:Debug>:MEMPULSE_ASSERT_ENABLED>
)


if(WIN32)
    target_link_libraries(mempulse ${DXGI_LIB} ${SETUPAPI_LIB})
endif()

if (HIP_PLATFORM STREQUAL "AMD")
    target_compile_definitions(mempulse PRIVATE __HIP_PLATFORM_AMD__)
elseif (HIP_PLATFORM STREQUAL "NVIDIA")
    target_compile_definitions(mempulse PRIVATE __HIP_PLATFORM_NVIDIA__)
else()
    message(FATAL_ERROR "Unsupported HIP platform: ${HIP_PLATFORM}. Supported platforms are AMD and NVIDIA.")
endif()


if(BUILD_EXAMPLES)
if(WIN32)
    add_executable(mempulse_example examples/mempulse_basic.cpp)
    target_link_libraries(mempulse_example mempulse)
    if (HIP_PLATFORM STREQUAL "AMD")
        target_compile_definitions(mempulse_example PRIVATE __HIP_PLATFORM_AMD__)
    elseif (HIP_PLATFORM STREQUAL "NVIDIA")
        target_compile_definitions(mempulse_example PRIVATE __HIP_PLATFORM_NVIDIA__)
    else()
        message(FATAL_ERROR "Unsupported HIP platform: ${HIP_PLATFORM}. Supported platforms are AMD and NVIDIA.")
    endif()

    add_executable(mempulse_pqc examples/mempulse_pqc.cpp)
    target_link_libraries(mempulse_pqc mempulse pdh)
    if (HIP_PLATFORM STREQUAL "AMD")
        target_compile_definitions(mempulse_pqc PRIVATE __HIP_PLATFORM_AMD__)
    elseif (HIP_PLATFORM STREQUAL "NVIDIA")
        target_compile_definitions(mempulse_pqc PRIVATE __HIP_PLATFORM_NVIDIA__)
    else()
        message(FATAL_ERROR "Unsupported HIP platform: ${HIP_PLATFORM}. Supported platforms are AMD and NVIDIA.")
    endif()

    add_executable(mempulse_d3dkmtq examples/mempulse_d3dkmtq.cpp)
    target_link_libraries(mempulse_d3dkmtq mempulse)

    add_executable(mempulse_d3dkmtq2 examples/mempulse_d3dkmtq2.cpp)
    target_link_libraries(mempulse_d3dkmtq2 mempulse)

    add_executable(mempulse_d3dkmtq3 examples/mempulse_d3dkmtq3.cpp)
    target_link_libraries(mempulse_d3dkmtq3 mempulse)

    add_executable(mempulse_d3dkmtq4 examples/mempulse_d3dkmtq4.cpp)
    target_link_libraries(mempulse_d3dkmtq4 mempulse)
endif() # WIN32
endif()

if(BUILD_TESTS)
    add_executable(mempulse_test tests/test_mempulse.cpp)
    target_link_libraries(mempulse_test mempulse)
    if (HIP_PLATFORM STREQUAL "AMD")
        target_compile_definitions(mempulse_test PRIVATE __HIP_PLATFORM_AMD__)
    elseif (HIP_PLATFORM STREQUAL "NVIDIA")
        target_compile_definitions(mempulse_test PRIVATE __HIP_PLATFORM_NVIDIA__)
    else()
        message(FATAL_ERROR "Unsupported HIP platform: ${HIP_PLATFORM}. Supported platforms are AMD and NVIDIA.")
    endif()
endif()

# Install
install(TARGETS mempulse
LIBRARY DESTINATION lib
ARCHIVE DESTINATION lib
RUNTIME DESTINATION bin
)
install(DIRECTORY include/ DESTINATION include)

if (BUILD_EXAMPLES)
    install(TARGETS mempulse_example
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
endif()

if(BUILD_TESTS)
    install(TARGETS mempulse_test
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
endif()


# Generate and install pkg-config file
configure_file(mempulse.pc.in mempulse.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/mempulse.pc DESTINATION lib/pkgconfig)
