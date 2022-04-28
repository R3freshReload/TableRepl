cmake_minimum_required(VERSION 3.8)
project(TableRepl)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES main.cpp)
add_executable(trepl ${SOURCE_FILES})
target_link_libraries(trepl)