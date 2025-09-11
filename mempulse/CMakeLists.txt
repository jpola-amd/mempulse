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
	LibraryContext.h  
	Logging.h  
	mempulse.h  
	MempulseContextImpl.h
	SafeCall.h  
	Utils.h
)

set(SOURCES
	Error.cpp  
	MempulseContextImpl.cpp
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
	PUBLIC ${CMAKE_SOURCE_DIR}
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
    target_link_libraries(mempulse PRIVATE ${DXGI_LIB} ${SETUPAPI_LIB})
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
# installation
# ----------------------------------------
set_target_properties(mempulse PROPERTIES PUBLIC_HEADER "mempulse.h")

install(TARGETS mempulse
        RUNTIME PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mempulse)

#install(TARGETS mempulse
#	LIBRARY DESTINATION lib
#	ARCHIVE DESTINATION lib
#	RUNTIME DESTINATION bin
#	RUNTIME PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mempulse
#)
