add_executable(tcp_sync_client "tcp_sync_client.cpp" "../utils.hpp")

if (WIN32)
	target_link_libraries(tcp_sync_client ${CONAN_LIBS} Threads::Threads)
else()
	target_link_libraries(tcp_sync_client ${CONAN_LIBS} Threads::Threads stdc++fs)	
endif()