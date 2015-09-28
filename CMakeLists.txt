cmake_minimum_required(VERSION 2.6)
project(network)

add_executable(network main.cpp)


add_library(UDPSocket udp_socket.cpp)
target_link_libraries(network UDPSocket)

add_library(address ip_addr.cpp)
target_link_libraries(network address)

add_library(udp_receiver udp_receiver.cpp)
target_link_libraries(network udp_receiver)

add_library(ip_pkg ip_pkg.cpp)
target_link_libraries(network ip_pkg)

add_library(run_lock run_lock.cpp)
target_link_libraries(network run_lock)

add_library(rw_mutex rw_mutex.cpp)
target_link_libraries(network rw_mutex)

add_library(msgLog msgLog.cpp)
target_link_libraries(network msgLog)



target_link_libraries(network pthread)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FALGS} -std=c++11")
install(TARGETS network RUNTIME DESTINATION bin)
