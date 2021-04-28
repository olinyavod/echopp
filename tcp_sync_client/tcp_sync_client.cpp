#include <iostream>
#include <asio.hpp>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

using asio::ip::tcp;
using endpoints = tcp::resolver::iterator;
using io_context_ptr = std::shared_ptr<asio::io_context>;

void print_usage(const std::string& name)
{
	std::cout << "Usage " << name << "[options] messages...\n"
		<< "\t-h - Hostname or address sent messages.\n"
		<< "\t-p - Port for connection.\n";		
}

auto resolve(const io_context_ptr& io_context, const std::string& host,
             const std::uint16_t port) -> endpoints
{
	const tcp::resolver::query query(host, std::to_string(port));
	tcp::resolver resolver(*io_context);

	return resolver.resolve(query);
}

void sync_echo(const io_context_ptr& io_context, 
	std::string msg, 
	const endpoints& ep, 
	std::mutex& write_mutex)
{
	auto sock = std::make_shared<tcp::socket>(*io_context);

	asio::connect(*sock, ep);

	std::string send(msg + "\n");
	
	sock->write_some(asio::buffer(send));

	const auto buff_size = 1024;
	char buff[buff_size];
	const auto bytes = read(*sock, 
		asio::buffer(buff, buff_size), 
		[&buff](asio::error_code err, std::size_t bytes)->std::size_t
		{
			if(err)
				return 0;
			
			auto* end = buff + bytes;
			return std::find(buff, end, '\n') < end ? 0 : 1;
		});

	const std::string response(buff, bytes - 1);

	{
		std::lock_guard<std::mutex> l(write_mutex);
		std::cout << "server echoed our " << msg << ": " << (response == msg ? "OK" : "FAIL") << std::endl;	
	}
	
	sock->close();
}

int main(int argc, char *argv[])
{
	try {
		const auto app_name = fs::path(argv[0]).filename().string();
		
		if (argc < 2)
		{
			print_usage(app_name);
			return 1;
		}

		std::uint16_t port = 9090;
		std::string host = "localhost";

		std::vector<char*> messages;

		for (int i = 1; i < argc; ++i)
		{
			if (std::strcmp(argv[i], "-p") == 0
				&& ++i < argc)
			{
				port = std::atoi(argv[i]);
			}
			else if (std::strcmp(argv[i], "-h") == 0
				&& ++i < argc)
			{
				host = argv[i];
			}
			else
				messages.push_back(argv[i]);
		}

		std::mutex write_mutex;
		const auto io_context = std::make_shared<asio::io_context>();

		auto endpoint = resolve(std::shared_ptr<asio::io_context>(io_context), host, port);

		asio::detail::thread_group threads;
		for (auto& m : messages)
		{
			threads.create_thread([io_context, endpoint, m, &write_mutex]()
			{
				sync_echo(io_context_ptr(io_context), std::string(m), endpoint, write_mutex);
			});
			//std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		threads.join();
	
		return 0;
	}
	catch(...)
	{
		return 1;
	}
}