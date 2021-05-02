add_executable(udp_async_server "udp_async_server.cpp" "udp_async_server.h")

target_link_libraries(udp_async_server ${CONAN_LIBS} Threads::Threads)
