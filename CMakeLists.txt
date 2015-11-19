cmake_minimum_required(VERSION 2.6)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
project(network)

add_library(network SHARED
	src/ip_addr.cpp
	src/ip_pkg.cpp
	src/run_lock.cpp
	src/rw_mutex.cpp
	src/udp_receiver.cpp
	src/udp_socket.cpp
)
add_library(network_static STATIC
	src/ip_addr.cpp
	src/ip_pkg.cpp
	src/run_lock.cpp
	src/rw_mutex.cpp
	src/udp_receiver.cpp
	src/udp_socket.cpp
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FALGS} -std=c++11")
