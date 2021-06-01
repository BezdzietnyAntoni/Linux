cmake_minimum_required(VERSION 3.19)
project(udp_1)

set(CMAKE_CXX_STANDARD 17)

add_executable(udp_server udp_server.cpp)
add_executable(udp_client udp_client.cpp)