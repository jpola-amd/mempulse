cmake_minimum_required(VERSION 3.16)
project(mempulse VERSION 1.0.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

option(MEMPULSE_EXPORT_API "Export dll symbols" ON)
option(BUILD_BACKEND_DRM "Build DRM backend (linux only)" ${UNIX})
option(BUILD_BACKEND_HIP "Build HIP backend" ON)
option(BUILD_BACKEND_D3DKMT "Build D3D kmt backend (windows only)" ${WIN32})
option(BUILD_TESTS "Build tests" ON)
option(BUILD_APPLICATION "Build command line application" ON)
option(ENABLE_ASAN "Enable adress sanitizer via libasan" OFF)
option(FORCE_COLORED_OUTPUT "Force gcc/lvm diagnostic color output" OFF)
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
include("${CMAKE_SOURCE_DIR}/cmake/CompilerFeatures.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/WindowsFileVersion.cmake")

check_std_expected(HAVE_STD_EXPECTED)
if (NOT HAVE_STD_EXPECTED)
	message(FATAL_ERROR "std::expected not supported on this toolchain")
	message(FATAL_ERROR "please, use newer version of compiler")
endif()

if (BUILD_BACKEND_DRM)
	message(STATUS "Enable backend DRM (Linux)")
	find_package(Drm REQUIRED COMPONENTS amdgpu)
	set(BUILD_BACKEND_HIP TRUE) # force HIP (dependency)
endif()
if (BUILD_BACKEND_D3DKMT)
	message(STATUS "Enable backend D3DKMT (Windows)")
	set(BUILD_BACKEND_HIP TRUE) # force HIP (dependency)
endif()
if (BUILD_BACKEND_HIP)
	message(STATUS "Enable backend HIP")
	find_package(HIP REQUIRED CONFIG)
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
	add_subdirectory(application)
endif()
if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()

# ---------------------------------------------------------------------
# --- CPack Configuration ---
# ---------------------------------------------------------------------
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "mempulse")
set(CPACK_PACKAGE_VENDOR "AMD")
set(CPACK_PACKAGE_CONTACT "example@amd.com")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/jpola-amd/mempulse")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cross-platform GPU memory monitoring library")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE") 
set(CPACK_RESOURCE_FILE_README  "${CMAKE_CURRENT_SOURCE_DIR}/README.md") 

# Choose default generators (you can also pass -G when calling cpack)
set(CPACK_GENERATOR "ZIP;TGZ")

include(CPack)
