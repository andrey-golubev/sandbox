cmake_minimum_required(VERSION 2.9)
project(problems)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Wall -Werror -O3")

add_executable(shortest_path shortest_path.cpp)
target_include_directories(shortest_path PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(bfs_shortest_reach bfs_shortest_reach.cpp)

# add_executable(trie trie.cpp)

add_executable(running_median running_median.cpp)
