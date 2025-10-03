# FindDrm.cmake
#
# CMake Find module for DRM libraries.
# Provides imported targets:
#   Drm::Drm        – core libdrm
#   Drm::amdgpu     – libdrm_amdgpu (optional component)
#
# Usage:
#   find_package(Drm REQUIRED)                    # core DRM only
#   find_package(Drm REQUIRED COMPONENTS amdgpu)  # plus AMDGPU component
#
# Variables exposed (for compatibility):
#   Drm_FOUND
#   Drm_VERSION
#   Drm_DRM_FOUND
#   Drm_amdgpu_FOUND
#   Drm_DRM_INCLUDE_DIRS / Drm_DRM_LIBRARIES
#   Drm_amdgpu_INCLUDE_DIRS / Drm_amdgpu_LIBRARIES
#
# You may set PKG_CONFIG_PATH or use standard CMake pkg-config envs to help discovery.

cmake_minimum_required(VERSION 3.16)

include_guard(GLOBAL)
include(CMakeFindDependencyMacro)
include(FindPackageHandleStandardArgs)

# Components handling (only "amdgpu" is defined)
set(_Drm_KNOWN_COMPONENTS amdgpu)
set(_Drm_REQUESTED_COMPONENTS ${Drm_FIND_COMPONENTS})

# If the targets are already defined, short-circuit.
if(TARGET Drm::Drm)
    # Derive Drm_FOUND from presence of core target.
    set(Drm_DRM_FOUND TRUE)
    if(TARGET Drm::amdgpu)
        set(Drm_amdgpu_FOUND TRUE)
    endif()

    # Validate requested components
    set(_Drm_MISSING_COMPONENTS "")
    foreach(_comp IN LISTS _Drm_REQUESTED_COMPONENTS)
        if(_comp STREQUAL "amdgpu")
            if(NOT TARGET Drm::amdgpu)
                list(APPEND _Drm_MISSING_COMPONENTS "${_comp}")
            endif()
        else()
            message(WARNING "[FindDrm] Unknown component requested: ${_comp}")
        endif()
    endforeach()

    set(Drm_VERSION "${Drm_VERSION}")
    find_package_handle_standard_args(Drm
            REQUIRED_VARS Drm_DRM_FOUND
            HANDLE_COMPONENTS
            VERSION_VAR Drm_VERSION)
    return()
endif()

# Try pkg-config first (preferred)
find_package(PkgConfig QUIET)

set(Drm_DRM_FOUND FALSE)
set(Drm_amdgpu_FOUND FALSE)
set(Drm_VERSION "")

if(PkgConfig_FOUND)
    # Core libdrm
    pkg_check_modules(PC_DRM QUIET IMPORTED_TARGET libdrm)
    if(PC_DRM_FOUND)
        add_library(Drm::Drm INTERFACE IMPORTED)
        set_property(TARGET Drm::Drm PROPERTY INTERFACE_LINK_LIBRARIES PkgConfig::PC_DRM)
        set(Drm_DRM_FOUND TRUE)
        set(Drm_DRM_INCLUDE_DIRS ${PC_DRM_INCLUDE_DIRS})
        set(Drm_DRM_LIBRARIES PkgConfig::PC_DRM)
        if(PC_DRM_VERSION)
            set(Drm_VERSION "${PC_DRM_VERSION}")
        endif()
    endif()

    # Optional AMDGPU component
    pkg_check_modules(PC_DRM_AMDGPU QUIET IMPORTED_TARGET libdrm_amdgpu)
    if(PC_DRM_AMDGPU_FOUND)
        add_library(Drm::amdgpu INTERFACE IMPORTED)
        set_property(TARGET Drm::amdgpu PROPERTY INTERFACE_LINK_LIBRARIES PkgConfig::PC_DRM_AMDGPU)
        set(Drm_amdgpu_FOUND TRUE)
        set(Drm_amdgpu_INCLUDE_DIRS ${PC_DRM_AMDGPU_INCLUDE_DIRS})
        set(Drm_amdgpu_LIBRARIES PkgConfig::PC_DRM_AMDGPU)
        # Prefer the newer of the two versions if both are present
        if(PC_DRM_AMDGPU_VERSION)
            if(Drm_VERSION)
                if(PC_DRM_AMDGPU_VERSION VERSION_GREATER Drm_VERSION)
                    set(Drm_VERSION "${PC_DRM_AMDGPU_VERSION}")
                endif()
            else()
                set(Drm_VERSION "${PC_DRM_AMDGPU_VERSION}")
            endif()
        endif()
    endif()
endif()

# Fallback: manual discovery if pkg-config failed (best-effort, still modern targets)
if(NOT Drm_DRM_FOUND)
    find_path(DRM_INCLUDE_DIR NAMES xf86drm.h)
    find_library(DRM_LIBRARY  NAMES drm)

    if(DRM_INCLUDE_DIR AND DRM_LIBRARY)
        add_library(Drm::Drm UNKNOWN IMPORTED)
        set_target_properties(Drm::Drm PROPERTIES
                IMPORTED_LOCATION "${DRM_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${DRM_INCLUDE_DIR}")
        set(Drm_DRM_FOUND TRUE)
        set(Drm_DRM_INCLUDE_DIRS "${DRM_INCLUDE_DIR}")
        set(Drm_DRM_LIBRARIES Drm::Drm)
    endif()
endif()

if(NOT Drm_amdgpu_FOUND)
    find_path(DRM_AMDGPU_INCLUDE_DIR NAMES amdgpu.h)
    find_library(DRM_AMDGPU_LIBRARY  NAMES drm_amdgpu)

    if(DRM_AMDGPU_INCLUDE_DIR AND DRM_AMDGPU_LIBRARY)
        add_library(Drm::amdgpu UNKNOWN IMPORTED)
        set_target_properties(Drm::amdgpu PROPERTIES
                IMPORTED_LOCATION "${DRM_AMDGPU_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${DRM_AMDGPU_INCLUDE_DIR}")
        set(Drm_amdgpu_FOUND TRUE)
        set(Drm_amdgpu_INCLUDE_DIRS "${DRM_AMDGPU_INCLUDE_DIR}")
        set(Drm_amdgpu_LIBRARIES Drm::amdgpu)
    endif()
endif()

# Validate requested components and overall package
set(_Drm_MISSING_COMPONENTS "")
foreach(_comp IN LISTS _Drm_REQUESTED_COMPONENTS)
    if(_comp STREQUAL "amdgpu")
        if(NOT Drm_amdgpu_FOUND)
            list(APPEND _Drm_MISSING_COMPONENTS "${_comp}")
        endif()
    else()
        message(WARNING "[FindDrm] Unknown component requested: ${_comp}")
    endif()
endforeach()

# Core package is considered found if libdrm is present
find_package_handle_standard_args(Drm
        REQUIRED_VARS Drm_DRM_FOUND
        HANDLE_COMPONENTS
        VERSION_VAR Drm_VERSION)

# Back-compat result var
set(Drm_FOUND ${Drm_FOUND})

# Provide helpful status messages (QUIET respects find_package QUIET)
if(Drm_FOUND)
    if(NOT Drm_FIND_QUIETLY)
        message(STATUS "Found Drm: version='${Drm_VERSION}' core=${Drm_DRM_FOUND} amdgpu=${Drm_amdgpu_FOUND}")
    endif()
else()
    if(_Drm_MISSING_COMPONENTS)
        if(NOT Drm_FIND_QUIETLY)
            message(STATUS "Drm missing components: ${_Drm_MISSING_COMPONENTS}")
        endif()
    endif()
endif()

# Clean up internal vars from the cache scope
mark_as_advanced(
        DRM_INCLUDE_DIR DRM_LIBRARY
        DRM_AMDGPU_INCLUDE_DIR DRM_AMDGPU_LIBRARY
)
