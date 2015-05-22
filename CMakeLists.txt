cmake_minimum_required(VERSION 2.6)
project(network)

add_executable(network main.cpp)

add_library(simpleList simpleList.cpp)
target_link_libraries(network simpleList)

add_library(server server.cpp)
target_link_libraries(network server)


add_library(msgLog msgLog.cpp)
target_link_libraries(network msgLog)

target_link_libraries(server pthread)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FALGS} -std=c++11")
install(TARGETS network RUNTIME DESTINATION bin)
