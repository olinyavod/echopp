#include <iostream>
#include <string>

#include <asio.hpp>

using asio::ip::udp;

int main(int argc, char* argv[])
{
	try 
	{
		std::array<char, 1024> buffer{};
		std::uint16_t port = 9900;

		const auto io_context = std::make_shared<asio::io_context>();
		udp::socket socket(*io_context, udp::endpoint(udp::v4(), port));

		std::cout << "Start listen on " << port << "\n.";
		
		while(true)
		{
			udp::endpoint sender_ep;
			auto bytes = socket.receive_from(asio::buffer(buffer), sender_ep);

			std::string_view msg(buffer.data(), bytes);
			std::cout << "[" << sender_ep << "] Receive message: " << msg << ".\n";

			socket.send_to(asio::buffer(msg), sender_ep);
		}
		
		return 0;
	}
	catch(std::exception& ex)
	{
		return 1;
	}
}
