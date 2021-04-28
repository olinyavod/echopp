add_executable(tcp_async_server tcp_async_server.cpp)

target_link_libraries(tcp_async_server ${CONAN_LIBS} Threads::Threads)