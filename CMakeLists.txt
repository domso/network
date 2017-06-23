cmake_minimum_required(VERSION 3.5)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
project(network)


add_executable(network_test demo/test.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FALGS} -std=c++17 -fPIC")
