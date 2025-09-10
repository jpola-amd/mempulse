cmake_minimum_required(VERSION 3.16)
project(MemPulse VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_EXAMPLES "Build basic example" ON)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_APPLICATION "Build command line application" ON)
option(BUILD_SHARED "Build as shared library" OFF)
set(HIP_PLATFORM "amd" CACHE STRING "Choose between AMD and NVIDIA")

#include(${CMAKE_SOURCE_DIR}/cmake/FindHIP.cmake)

#if(ROCM_PATH)
#    set(HIP_DIR ${ROCM_PATH})
#    message(STATUS "Using ROCM_PATH from CMake variable: ${HIP_DIR}")
#elseif(DEFINED ENV{ROCM_PATH})
#    set(HIP_DIR $ENV{ROCM_PATH})
#    message(STATUS "Using ROCM_PATH from environment: ${HIP_DIR}")
#else()
#    if(WIN32)
#        set(HIP_DIR "C:/opt/rocm")
#    else()
#        set(HIP_DIR "/opt/rocm")
#    endif()
#    message(STATUS "Using default ROCM_PATH: ${HIP_DIR}")
#endif()

list(APPEND CMAKE_PREFIX_PATH "/opt/rocm/lib/cmake/hip")
#AddHIP(HIP_DIR ${HIP_DIR})
find_package(HIP REQUIRED CONFIG)

#include_directories(${CMAKE_SOURCE_DIR}/include)

# Platform-specific libraries
if(WIN32)
    set(DXGI_LIB dxgi)
    set(SETUPAPI_LIB setupapi)
endif()

add_subdirectory(src)

if(BUILD_APPLICATION)
	add_subdirectory(application)
endif()

if(BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

if(BUILD_TESTS)
	add_subdirectory(tests)
endif()

# Generate and install pkg-config file
configure_file(mempulse.pc.in mempulse.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/mempulse.pc DESTINATION lib/pkgconfig)
