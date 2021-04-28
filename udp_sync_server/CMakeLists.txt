add_executable(udp_sync_server "udp_sync_server.cpp" )

target_link_libraries(udp_sync_server ${CONAN_LIBS} Threads::Threads)
