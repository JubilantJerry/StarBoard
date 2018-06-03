cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(StarBoard_RTS_tests VERSION 0.1.0 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_library(add_one MODULE add_one.c)

add_executable(tests test_main.cpp test_data.cpp test_foreign_call.cpp)
target_link_libraries(tests native_interface native_loader ${CMAKE_DL_LIBS})