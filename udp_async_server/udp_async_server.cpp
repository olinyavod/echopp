#include "udp_async_server.h"

int main(int argc, char* argv[])
{
	try 
	{
		const std::uint16_t port = 9900;

		auto io_context = std::make_unique<asio::io_context>();

		connection::start(*io_context, port);

		io_context->post([port]()
			{
				std::cout << "Started on " << port << "\n.";
			});
		
		io_context->run();
		
		return 0;
	}
	catch (std::exception& ex)
	{
		std::cerr << "Error: " << ex.what() << ".\n";
		return 1;
	}
}
