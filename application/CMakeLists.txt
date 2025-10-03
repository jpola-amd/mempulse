set(SOURCES
	main.cpp
)

add_executable(gpumempulse ${SOURCES})
target_include_directories(gpumempulse PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(gpumempulse PRIVATE mempulse)

install(TARGETS gpumempulse
        RUNTIME)


