set(HEADERS
	${CMAKE_SOURCE_DIR}/include/mempulse/mempulse.h
	${CMAKE_SOURCE_DIR}/include/mempulse/memory_info.h
	${CMAKE_SOURCE_DIR}/include/mempulse/memory_usage.h
    ${CMAKE_SOURCE_DIR}/include/mempulse/memory_reporter.h
    ${CMAKE_SOURCE_DIR}/include/mempulse/memory_reporter_factory.h
    ${CMAKE_SOURCE_DIR}/include/mempulse/config.h
)

set(SOURCES
    memory_reporter_factory.cpp
	mempulse.cpp
	memory_info.cpp
	memory_usage.cpp
)

# # Platform specific SOURCES
if(WIN32)
    list(APPEND SOURCES
        platform/windows/d3dkmt_memory_reporter.cpp
        platform/windows/d3dkmt_memory_reporter.h
        platform/linux/hip_memory_reporter.cpp
        platform/linux/hip_memory_reporter.h
    )
else()
    list(APPEND SOURCES
        platform/linux/hip_memory_reporter.cpp
        platform/linux/hip_memory_reporter.h
    )
endif()
if (BUILD_SHARED)
	add_library(mempulse SHARED ${HEADERS} ${SOURCES})
else()
	add_library(mempulse STATIC ${HEADERS} ${SOURCES})
endif()

target_include_directories(mempulse 
	PUBLIC ${CMAKE_SOURCE_DIR}/include
	PRIVATE ${hip_INCLUDE_DIRS})
#target_link_libraries(mempulse PRIVATE ${HIP_LIBRARIES})
target_link_libraries(mempulse PRIVATE hip::host)
target_link_directories(mempulse PRIVATE ${HIP_LIB_INSTALL_DIR})
target_compile_definitions(mempulse PRIVATE
    $<$<CONFIG:Debug>:MEMPULSE_ASSERT_ENABLED>
)

if(WIN32)
    target_link_libraries(mempulse ${DXGI_LIB} ${SETUPAPI_LIB})
endif()

if (HIP_PLATFORM STREQUAL "amd")
    target_compile_definitions(mempulse PRIVATE __HIP_PLATFORM_AMD__)
elseif (HIP_PLATFORM STREQUAL "nvidia")
    target_compile_definitions(mempulse PRIVATE __HIP_PLATFORM_NVIDIA__)
else()
    message(FATAL_ERROR "Unsupported HIP platform: ${HIP_PLATFORM}. Supported platforms are AMD and NVIDIA.")
endif()

install(TARGETS mempulse
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include)

