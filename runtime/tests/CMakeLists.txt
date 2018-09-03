cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(StarBoard_RTS_tests VERSION 0.1.0 LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${Boost_INCLUDE_DIRS})

add_library(add_one MODULE add_one.c)
add_library(slow_nop MODULE slow_nop.c)

add_executable(tests test_main.cpp catch_test_util.cpp
    test_data.cpp
    test_data_reference.cpp
    test_data_serializer.cpp
    test_stream_handle.cpp
    test_foreign_call.cpp
    test_module_port_scheduler.cpp
    test_module.cpp
    test_stream_protocol.cpp
    test_worker.cpp
)

add_executable(child_exit child_exit.c)

target_link_libraries(tests
    data
    data_reference
    data_interface
    data_serializer
    stream_handle
    native_loader
    module_port_scheduler
    module
    module_data_handler
    stream_protocol
    communicator
    worker
    ${CMAKE_THREAD_LIBS_INIT}
    ${CMAKE_DL_LIBS}
)