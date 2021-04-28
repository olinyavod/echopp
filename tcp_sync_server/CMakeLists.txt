
add_executable(tcp_sync_server tcp_sync_server.cpp)

target_link_libraries(tcp_sync_server ${CONAN_LIBS} Threads::Threads)