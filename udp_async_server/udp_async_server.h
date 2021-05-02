#pragma once

#include <iostream>
#include <string>
#include <memory>

#include <asio.hpp>
#include <utility>

using asio::ip::udp;
using socket_ptr = std::shared_ptr<udp::socket>;

class connection : public std::enable_shared_from_this<connection>
{
private:
	socket_ptr socket_;

	connection(socket_ptr socket)
		: socket_(std::move(socket))
	{

	}

public:
	using connection_ptr = std::shared_ptr<connection>;
	static connection_ptr start(asio::io_context& io_context, std::uint16_t port)
	{
		const auto socket = std::make_shared<udp::socket>(io_context, udp::endpoint(udp::v4(), port));

		connection_ptr self(new connection(socket));

		self->do_receive();
		
		return self;
	}

private:
	void do_receive()
	{
		auto buffer = std::make_shared<std::array<char, 1024>>();
		auto self = shared_from_this();
		auto sender_ep = std::make_shared<udp::endpoint>();
		
		socket_->async_receive_from(asio::buffer(*buffer),
			*sender_ep,
			[self, buffer, sender_ep](asio::error_code err, std::size_t bytes)
			{
				if(err)
				{
					std::cerr << "Error: " << err << ".\n";
					self->do_receive();
					return;
				}

				std::string_view msg(buffer->data(), bytes);

				std::cout << "[Server] Receive message form " << *sender_ep << ": " << msg << ".\n";
				
				self->do_send(*sender_ep, msg);
			});
	}

	void do_send(const udp::endpoint& sender, const std::string_view& message)
	{
		auto self = shared_from_this();
		auto msg = std::make_shared<std::string>(message.data(), message.size());
		auto ep = std::make_shared<udp::endpoint>(sender);

		socket_->async_send_to(asio::buffer(*msg),
			*ep,
			[self, ep, msg](asio::error_code err, std::size_t bytes)
			{
				self->do_receive();
			});
	}
};
