set(HEADERS
	CheckHip.h
	DeviceHip.h
	BackendHip.h
)
set(SOURCES
	DeviceHip.cpp
	BackendHip.cpp
)

add_library(mempulse_backend_hip STATIC ${HEADERS} ${SOURCES})
set_target_properties(mempulse_backend_hip PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(mempulse_backend_hip 
	PRIVATE ${CMAKE_SOURCE_DIR} 
	PUBLIC ${hip_INCLUDE_DIRS}
)

target_link_libraries(mempulse_backend_hip 
	PRIVATE hip::host
)
target_link_directories(mempulse_backend_hip 
	PRIVATE ${HIP_LIB_INSTALL_DIR}
)

if (HIP_PLATFORM STREQUAL "amd")
    target_compile_definitions(mempulse_backend_hip PUBLIC __HIP_PLATFORM_AMD__)
elseif (HIP_PLATFORM STREQUAL "nvidia")
    target_compile_definitions(mempulse_backend_hip PUBLIC __HIP_PLATFORM_NVIDIA__)
else()
    message(FATAL_ERROR "Unsupported HIP platform: ${HIP_PLATFORM}. Supported platforms are AMD and NVIDIA.")
endif()

