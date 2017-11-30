cmake_minimum_required(VERSION 2.6)
add_executable(Server src/main.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")