configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version_info.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/version_info.h
    @ONLY
)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.rc
    @ONLY
)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/Config.h.in 
	${CMAKE_CURRENT_BINARY_DIR}/Config.h 
	@ONLY
)

set(HEADERS
	Device.h  
	Error.h  
	Backend.h  
	Logging.h  
	mempulse.h  
	LibraryContext.h
	SafeCall.h  
	Utils.h
)

set(SOURCES
	Error.cpp  
	LibraryContext.cpp
	mempulse.cpp  
	Utils.cpp
)

add_library(mempulse SHARED 
	${HEADERS} 
	${SOURCES} 
	${CMAKE_CURRENT_BINARY_DIR}/version_info.h
    ${CMAKE_CURRENT_BINARY_DIR}/version.rc
    ${CMAKE_CURRENT_BINARY_DIR}/Config.h
)
set_target_properties(mempulse PROPERTIES VERSION ${PROJECT_VERSION})

if(MEMPULSE_EXPORT_API)
    target_compile_definitions(mempulse PRIVATE MEMPULSE_EXPORT_API)
endif()

target_include_directories(mempulse 
	PRIVATE ${CMAKE_SOURCE_DIR}
	PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

target_compile_definitions(mempulse 
	PRIVATE $<$<CONFIG:Debug>:MEMPULSE_ASSERT_ENABLED>
)

# ----------------------------------------
# backends 
# ----------------------------------------

if(BUILD_BACKEND_D3DKMT)
	add_subdirectory(backend/d3dkmt)
	target_compile_definitions(mempulse PRIVATE MEMPULSE_BUILD_BACKEND_D3DKMT)
	target_link_libraries(mempulse PRIVATE mempulse_backend_d3dkmt)
endif()

if(BUILD_BACKEND_HIP)
	add_subdirectory(backend/hip)
	target_compile_definitions(mempulse PRIVATE MEMPULSE_BUILD_BACKEND_HIP)
	target_link_libraries(mempulse PRIVATE mempulse_backend_hip)
endif()

if(BUILD_BACKEND_DRM)
	add_subdirectory(backend/drm)
	target_compile_definitions(mempulse PRIVATE MEMPULSE_BUILD_BACKEND_DRM)
    target_link_libraries(mempulse PRIVATE mempulse_backend_drm)
endif()

# ----------------------------------------
# library installation
# ----------------------------------------
set_target_properties(mempulse PROPERTIES 
	PUBLIC_HEADER "mempulse.h"
	OUTPUT_NAME "mempulse"
	EXPORT_NAME "Mempulse" # become Mempulse::
)

install(
    TARGETS mempulse
    EXPORT MempulseTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    RUNTIME PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mempulse
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

#install(TARGETS mempulse
#	EXPORT MempulseTargets
#    RUNTIME PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mempulse)

# ---------- Export targets (install tree) ----------
install(
    EXPORT MempulseTargets
    NAMESPACE Mempulse::
    FILE MempulseTargets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Mempulse
)

# ---------- Generate & install Config + Version files ----------
include(CMakePackageConfigHelpers)

# Config file (from template below)
configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/files/MempulseConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/MempulseConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Mempulse
)

# Version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/MempulseConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/MempulseConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/MempulseConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Mempulse
)

# ---------- Export for the build tree (optional but handy) ----------
export(
    EXPORT MempulseTargets
    NAMESPACE Mempulse::
    FILE "${CMAKE_CURRENT_BINARY_DIR}/MempulseTargets.cmake"
)
# Register in the user package registry so find_package can discover the build-tree package
export(PACKAGE Mempulse)

