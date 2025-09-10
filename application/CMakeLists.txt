set(SOURCES
	main.cpp
)

add_executable(gpumempulse ${SOURCES})
target_link_libraries(gpumempulse mempulse)

if (HIP_PLATFORM STREQUAL "amd")                                            
    target_compile_definitions(gpumempulse PRIVATE __HIP_PLATFORM_AMD__)   
elseif (HIP_PLATFORM STREQUAL "nvidia")                                     
    target_compile_definitions(gpumempulse PRIVATE __HIP_PLATFORM_NVIDIA__)
else()                                                                      
    message(FATAL_ERROR "Unsupported HIP platform: ${HIP_PLATFORM}. Supported platforms are AMD and NVIDIA.")
endif()   
