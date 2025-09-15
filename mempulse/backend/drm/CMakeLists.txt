set(HEADERS
	Authentificate.h  
	BackendDrm.h  
	check_drm.h  
	Defines.h  
	DeviceDrm.h  
	DrmAmdgpu.h  
	DrmDevice.h  
	DrmDevices.h  
	DrmFileAmdgpu.h  
	DrmVersionPtr.h  
	File.h  
	HardwarePci.h
)
set(SOURCES
	Authentificate.cpp  
	BackendDrm.cpp  
	DeviceDrm.cpp  
	DrmAmdgpu.cpp  
	DrmDevice.cpp  
	DrmDevices.cpp  
	DrmFileAmdgpu.cpp  
	File.cpp  
	HardwarePci.cpp
)

add_library(mempulse_backend_drm STATIC ${HEADERS} ${SOURCES})
set_target_properties(mempulse_backend_drm PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(mempulse_backend_drm 
	PRIVATE ${CMAKE_SOURCE_DIR} 
)

target_link_libraries(mempulse_backend_drm 
	PRIVATE Drm::Drm Drm::amdgpu
)


