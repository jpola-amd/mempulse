

set(SOURCES
	main.cpp
)

add_executable(gpumempulse ${SOURCES})
target_link_libraries(gpumempulse PRIVATE mempulse)

install(TARGETS gpumempulse
        RUNTIME)


