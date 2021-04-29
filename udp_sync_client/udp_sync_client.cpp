#include <iostream>
#include <string>
#include <clocale>

#include <asio.hpp>

#include "../utils.hpp"

using asio::ip::udp;
using endpoint_iterator = udp::resolver::iterator;
using asio::detail::thread_group;

endpoint_iterator resolve(asio::io_context& io_context, 
	const std::string& host,
	const std::uint16_t port)
{
	const udp::resolver::query query(host, std::to_string(port));
	udp::resolver resolver(io_context);

	return resolver.resolve(query);
}

void sync_echo(std::mutex& mutex,
	asio::io_context& io_context, 
	const endpoint_iterator& endpoints,
	const std::string& msg)
{
	udp::socket socket(io_context, udp::endpoint(udp::v4(), 0));
	endpoint_iterator end;
	auto ep = std::find_if(
		endpoints,
		end,
		[](udp::endpoint p)
		{
			return p.protocol() == udp::v4();
		});
	
	socket.send_to(asio::buffer(msg), *ep);

	std::array<char, 1024> buffer{};
	udp::endpoint sender_ep;
	
	auto bytes = socket.receive_from(asio::buffer(buffer), sender_ep);

	std::string_view echo(buffer.data(), bytes);
	
	{
		std::lock_guard<std::mutex> lock(mutex);
		std::cout << "server echoed our " << msg << ": " << (echo == msg ? "OK" : "FAIL") << std::endl;
	}
	
	socket.close();	
}

int main(int argc, char* argv[])
{
	try 
	{
		if(argc < 2)
		{
			client::print_usage(argv[0]);
			return 1;
		}
		
		std::setlocale(LC_ALL, "");
		
		std::string host = "localhost";
		std::uint16_t port = 9900;
		std::vector<std::string> messages;
		
		client::parse_client_args(argc, argv, host, port, messages);
		
		std::mutex mutex;
		asio::io_context io_context;
		auto endpoints = resolve(io_context, host, port);

		thread_group threads;
		
		for(auto& msg : messages)
		{
			threads.create_thread([msg, &io_context, &endpoints, &mutex]()
				{
					try 
					{
						sync_echo(mutex, io_context, endpoints, msg);
					}
					catch (std::exception& ex)
					{
						std::cerr << "Error: " << ex.what() << ".\n";
					}
				});
		}

		threads.join();
		
		return 0;
	}
	catch(std::exception& ex)
	{
		std::cerr << "Error: " << ex.what() << ".\n";
		return 1;
	}
	catch(...)
	{
		return 1;
	}
}