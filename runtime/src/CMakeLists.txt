cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(StarBoard_RTS_src VERSION 0.1.0 LANGUAGES CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
add_library(test_lib test_lib.cpp)
add_executable(demo demo.cpp)