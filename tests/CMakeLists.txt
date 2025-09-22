# ----------------------------------------------------
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
)

# Prevent GoogleTest from adding install targets
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)
# ----------------------------------------------------

set(HEADERS
	#	utils.h
)
set(SOURCES
	#utils.cpp
)

set (TESTS
	Library
	Device
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
target_include_directories(mempulse_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(mempulse_test
	GTest::gtest
	GTest::gtest_main
	mempulse
)

install(TARGETS mempulse_test
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

