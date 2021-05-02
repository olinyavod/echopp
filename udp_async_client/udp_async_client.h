#pragma once

#include <iostream>
#include <string>
#include <memory>

#include <asio.hpp>

#include "..\utils.hpp"

using asio::ip::udp;

class connection : public std::enable_shared_from_this<connection>
{
private:
	udp::socket socket_;
	udp::endpoint endpoint_;

	connection(asio::io_context& io_context,
		const udp::endpoint& endpoint)
		:socket_(io_context, udp::endpoint(udp::v4(), 0)),
		endpoint_(endpoint)
	{

	}

public:
	static auto start(asio::io_context& io_context, const udp::endpoint& endpoint, const std::string message)
	{
		std::shared_ptr<connection> self(new connection(io_context, endpoint));
		
		self->async_send(message);
		
		return self;
	}

private:
	void async_send(const std::string& message)
	{
		const auto self = shared_from_this();
		auto msg = std::make_shared<std::string>(message);

		socket_.async_send_to(
			asio::buffer(*msg),
			endpoint_,
			[self, msg](asio::error_code err, std::size_t bytes)
			{
				if(err)
				{
					std::cerr << "Error: " << err << ".\n";

					return;
				}
				
				std::cout << "Sent message: " << *msg << ".\n";

				self->async_read(*msg);
			});
	}

	void async_read(const std::string& message)
	{
		const auto self = shared_from_this();
		const auto buffer = std::make_shared<std::array<char, 1024>>();
		auto msg = std::make_shared<std::string>(message);
		const auto sender_ep = std::make_shared<udp::endpoint>();
		socket_.async_receive_from(asio::buffer(*buffer),
			*sender_ep,
			[self, buffer, sender_ep, msg](asio::error_code err, std::size_t bytes)
			{
				if(err)
				{
					std::cerr << "Error: " << err << ".\n";
					return;
				}
				
				const std::string_view echo(buffer->data(), bytes);

				std::cout << "server "<< *sender_ep << " echoed our " << *msg << ": " << (echo == *msg ? "OK" : "FAIL") << std::endl;			
			});
	}	
};
