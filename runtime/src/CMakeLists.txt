cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(StarBoard_RTS_src VERSION 0.1.0 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${Boost_INCLUDE_DIRS})

add_library(data STATIC data.cpp)
add_library(data_interface STATIC data_interface.cpp)
add_library(native_loader STATIC native_loader.cpp)
add_library(module_port_scheduler STATIC module_port_scheduler.cpp)
add_library(module STATIC module.cpp)