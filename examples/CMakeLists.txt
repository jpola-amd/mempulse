cmake_minimum_required(VERSION 3.16)
project(Example VERSION 1.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Mempulse CONFIG REQUIRED)

set(SOURCES 
	example.cpp
)

add_executable(example example.cpp)
target_link_libraries(example Mempulse::Mempulse)
