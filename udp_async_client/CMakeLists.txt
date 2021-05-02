add_executable(udp_async_client "udp_async_client.cpp" "udp_async_client.h")

if (WIN32)
	target_link_libraries(udp_async_client ${CONAN_LIBS} Threads::Threads)
else()
	target_link_libraries(udp_async_client ${CONAN_LIBS} Threads::Threads stdc++fs)	
endif()