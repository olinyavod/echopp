add_executable(udp_sync_client "udp_sync_client.cpp" "../utils.hpp")

if (WIN32)
	target_link_libraries(udp_sync_client ${CONAN_LIBS} Threads::Threads)
else()
	target_link_libraries(udp_sync_client ${CONAN_LIBS} Threads::Threads stdc++fs)	
endif()