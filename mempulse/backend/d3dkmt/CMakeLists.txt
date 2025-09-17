set(HEADERS
	DeviceD3dkmt.h
	BackendD3dkmt.h
)
set(SOURCES
	DeviceD3dkmt.cpp
	BackendD3dkmt.cpp
)

add_library(mempulse_backend_d3dkmt STATIC ${HEADERS} ${SOURCES})
set_target_properties(mempulse_backend_d3dkmt PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(mempulse_backend_d3dkmt 
	PRIVATE ${CMAKE_SOURCE_DIR} 
)

target_link_libraries(mempulse_backend_d3dkmt
	PRIVATE gdi32 
	PRIVATE mempulse_backend_hip
)