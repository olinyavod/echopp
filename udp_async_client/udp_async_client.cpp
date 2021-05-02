#include "udp_async_client.h"

using endpoint_iterator = udp::resolver::iterator;

void resolve(asio::io_context& io_context, 
	const std::string& host,
	const std::uint16_t port,
	const std::vector<std::string>& messages)
{
	const auto query = std::make_shared<udp::resolver::query>(host, std::to_string(port));
	const auto resolver = std::make_shared<udp::resolver>(io_context);

	resolver->async_resolve(*query,
		[&io_context, resolver, &messages](asio::error_code err, endpoint_iterator endpoints)
		{
			if(err)
			{
				std::cerr << "Error: " << err << ".\n";
				return 0;
			}
			const endpoint_iterator end;
			auto ep = std::find_if(endpoints, end, [](udp::endpoint ep)
				{
					return ep.protocol() == udp::v4();
				});

			for (const auto& msg : messages)
				connection::start(io_context, *ep, msg);
		});
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

		std::string host = "localhost";
		std::uint16_t port = 9900;
		std::vector<std::string> messages;

		client::parse_client_args(argc, argv, host, port, messages);

		asio::io_context io_context;

		resolve(io_context, host, port, messages);

		io_context.run();
		
		return 0;
	}
	catch (std::exception& ex)
	{
		std::cerr << "Error: " << ex.what() << ".\n";
		return 1;
	}
}
