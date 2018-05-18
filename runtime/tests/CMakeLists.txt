cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(StarBoard_RTS_tests VERSION 0.1.0 LANGUAGES CXX C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
add_executable(test_catch test_catch.cpp)
target_link_libraries(test_catch test_lib)