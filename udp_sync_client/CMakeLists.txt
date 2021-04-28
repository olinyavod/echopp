	
add_executable(udp_sync_client udp_sync_client.cpp)

target_link_libraries(udp_sync_client ${CONAN_LIBS} Threads::Threads)