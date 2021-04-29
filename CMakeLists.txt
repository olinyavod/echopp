cmake_minimum_required(VERSION 3.10)

project(echo)
add_definitions("-std=c++11")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
 conan_basic_setup()

add_subdirectory(tcp_async_server)
add_subdirectory(tcp_sync_server)

add_subdirectory(tcp_async_client)
add_subdirectory(tcp_sync_client)

add_subdirectory(udp_sync_client)
add_subdirectory("udp_sync_server")
