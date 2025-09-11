cmake_minimum_required(VERSION 3.16)
project(MemPulse VERSION 1.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

option(MEMPULSE_EXPORT_API "Export dll symbols" ON)
option(BUILD_BACKEND_DRM "Build DRM backend (linux only)" OFF)
option(BUILD_BACKEND_HIP "Build DRM backend" ON)
option(BUILD_BACKEND_D3DKMT "Build D3D kmt backend (windows only)" ON)
option(BUILD_EXAMPLES "Build basic example" OFF)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_APPLICATION "Build command line application" ON)
set(HIP_PLATFORM "amd" CACHE STRING "Choose between AMD and NVIDIA")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
if (NOT HIP_DIR)
	if(WIN32)
		set(HIP_DIR "c:/Program Files/AMD/ROCm/6.2/lib/cmake/hip")
	else()
		set(HIP_DIR "/opt/rocm/lib/cmake/hip")
	endif()
endif()

include("${CMAKE_SOURCE_DIR}/cmake/CompileOptions.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/WindowsFileVersion.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/CopyFileToTarget.cmake")

find_package(HIP REQUIRED CONFIG)

# Platform-specific libraries
if(WIN32)
    set(DXGI_LIB dxgi)
    set(SETUPAPI_LIB setupapi)
endif()

include(GNUInstallDirs)

# workaround for dll's copies
if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
endif()

add_subdirectory(mempulse)
if(BUILD_APPLICATION)
	find_package(Boost REQUIRED COMPONENTS system log program_options)
	add_subdirectory(application)
endif()
if(BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()
if(BUILD_TESTS)
	enable_testing()
	find_package(GTest REQUIRED)
	add_subdirectory(tests)
endif()

# Generate and install pkg-config file
configure_file(mempulse.pc.in mempulse.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/mempulse.pc DESTINATION lib/pkgconfig)
