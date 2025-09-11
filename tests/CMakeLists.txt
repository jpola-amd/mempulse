set(HEADERS
	#	utils.h
)
set(SOURCES
	#utils.cpp
)

set (TESTS
	Library
	Meminfo
)

foreach(test ${TESTS})
	add_test(
			NAME ${test}
			COMMAND mempulse_test --gtest_filter=Test${test}.*
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		)
		set (TEST_SOURCES ${TEST_SOURCES} Test${test}.cpp)
endforeach()

add_executable(mempulse_test ${TEST_SOURCES} ${TEST_HEADERS})
target_link_libraries(mempulse_test
	GTest::gtest
	GTest::gtest_main
	mempulse
)

if (HIP_PLATFORM STREQUAL "amd")
    target_compile_definitions(mempulse_test PRIVATE __HIP_PLATFORM_AMD__)
elseif (HIP_PLATFORM STREQUAL "nvidia")
    target_compile_definitions(mempulse_test PRIVATE __HIP_PLATFORM_NVIDIA__)
else()
    message(FATAL_ERROR "Unsupported HIP platform: ${HIP_PLATFORM}. Supported platforms are AMD and NVIDIA.")
endif()

install(TARGETS mempulse_test
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

