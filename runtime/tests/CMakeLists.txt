cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(StarBoard_RTS_tests VERSION 0.1.0 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${Boost_INCLUDE_DIRS})

add_library(add_one MODULE add_one.c)

add_executable(tests test_main.cpp
    test_data.cpp
    test_foreign_call.cpp
    test_module_port_scheduler.cpp
    test_module.cpp)
target_link_libraries(tests
    data
    native_interface
    native_loader
    module_port_scheduler
    ${CMAKE_THREAD_LIBS_INIT}
    ${CMAKE_DL_LIBS})