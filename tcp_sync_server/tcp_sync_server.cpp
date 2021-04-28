#include <iostream>
#include <algorithm>

#include <asio.hpp>

using asio::ip::tcp;

int main(int argc, char *argv[])
{
	try
	{
		std::uint16_t port = 9090;

		if (argc == 2)
			port = std::atoi(argv[1]);

		const auto io_context = std::make_shared<asio::io_context>();

		const auto acceptor = std::make_shared<tcp::acceptor>(*io_context, tcp::endpoint(tcp::v4(), port));

		std::cout << "Start echo server on: " << acceptor->local_endpoint() << ".\n";
		char buff[1024];

		while(true)
		{
			tcp::socket sock(*io_context);
			acceptor->accept(sock);

			const auto bytes = read(sock,
				asio::buffer(buff),
				[&buff](asio::error_code error, std::size_t bytes)->std::size_t
				{
					if(error)
						return 0 ;

					auto* end = buff + bytes ;
					return std::find(buff, end, '\n') < end ? 0 : 1 ;			                        	
				});

			std::string msg(buff, bytes);

			sock.write_some(asio::buffer(msg));

			sock.close();			
		}
		
		return 0;
	}
	catch(std::exception& ex)
	{
		std::cerr << "Error: " << ex.what() << ".\n";
		return 1;
	}
}