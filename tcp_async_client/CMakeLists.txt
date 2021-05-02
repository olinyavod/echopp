add_executable(tcp_async_client "tcp_async_client.cpp" "../utils.hpp")

if (WIN32)
	target_link_libraries(tcp_async_client ${CONAN_LIBS} Threads::Threads)
else()
	target_link_libraries(tcp_async_client ${CONAN_LIBS} Threads::Threads stdc++fs)	
endif()