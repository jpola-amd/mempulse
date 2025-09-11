set(HEADERS
	DeviceDrm.h
	LibraryContextDrm.h
)
set(SOURCES
	DeviceDrm.cpp
	LibraryContextDrm.cpp
)

add_library(mempulse_backend_drm STATIC ${HEADERS} ${SOURCES})
set_target_properties(mempulse_backend_drm PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(mempulse_backend_drm 
	PRIVATE ${CMAKE_SOURCE_DIR} 
	#	PRIVATE ${hip_INCLUDE_DIRS}
)

#target_link_libraries(mempulse_backend_drm 
#	PRIVATE hip::host
#)
#target_link_directories(mempulse_backend_drm 
#	PRIVATE ${HIP_LIB_INSTALL_DIR}
#)


