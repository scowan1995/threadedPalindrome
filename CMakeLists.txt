cmake_minimum_required(VERSION 3.3)
project(C__Threading)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp threads.cpp)
add_executable(C__Threading ${SOURCE_FILES})