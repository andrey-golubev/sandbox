cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Wall -Werror -O3")

set(TARGET1 "binary_tree")
add_executable(${TARGET1}
    binary_tree.cpp
)
target_include_directories(${TARGET1} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
