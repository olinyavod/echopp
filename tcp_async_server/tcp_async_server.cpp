#include <iostream>
#include <string>
#include <memory>

#include <asio.hpp>

using asio::ip::tcp;
using socket_ptr = std::shared_ptr<tcp::socket>;

class connection : public std::enable_shared_from_this<connection>
{
private:
	static const std::size_t max_buffer = 1024;
	socket_ptr sock_;

	connection(socket_ptr& sock)
		: sock_(sock)
	{
		
	}

public:
	using connection_ptr = std::shared_ptr<connection>;

	static auto start(socket_ptr socket)->connection_ptr
	{		
		connection_ptr self(new connection(socket));
		self->start();
		return self;
	}

	~connection()
	{

	}
	
	
private:
	auto start()->void
	{
		const auto buffer = std::make_shared<std::array<char, max_buffer>>();
		auto self = shared_from_this();
		asio::async_read(*sock_,
			asio::buffer(*buffer),
			[self, buffer](asio::error_code err, std::size_t bytes)
			{
				return self->read_completion(err, buffer->data(), bytes);
			},
			[self, buffer](asio::error_code err, std::size_t bytes)
			{
				self->on_read(err, buffer->data(), bytes);
			});
	}

	auto read_completion(const asio::error_code& err, char* buffer, std::size_t bytes) const ->std::size_t
	{
		if (err)
			return 0;
		auto* end = buffer + bytes;
		return std::find(buffer, end, '\n') < end ? 0 : 1;
	}

	auto on_read(const asio::error_code& err, char* buffer, std::size_t bytes)->void
	{
		if (err)
			return;

		const std::string msg(buffer, bytes);

		do_write(msg);
	}

	auto do_write(const std::string& message)->void
	{
		const auto self = shared_from_this();
		const auto msg = std::make_shared<std::string>(message);
		sock_->async_write_some(asio::buffer(*msg), 
			[self, msg](asio::error_code err, std::size_t bytes)
			{
				self->on_write(err, bytes);
			});
	}

	auto on_write(const asio::error_code& err, std::size_t bytes)->void
	{
		
	}
};

void accept(asio::io_context& io_context,  tcp::acceptor& acceptor)
{
	auto sock = std::make_shared<tcp::socket>(io_context);
	acceptor.async_accept(*sock, 
		[&io_context, &acceptor, sock](asio::error_code err)
		{
			if (!err)
				connection::start(sock);
			
			accept(io_context, acceptor);
		});
}

int main(int argc, char *argv[])
{
	try 
	{
		const auto io_context = std::make_shared<asio::io_context>();
		tcp::endpoint ep(tcp::v4(), 9090);
		const auto acceptor = std::make_shared<tcp::acceptor>(*io_context, ep);

		accept(*io_context, *acceptor);
		
		io_context->run();
		
		return 0;
	}
	catch(std::exception& ex)
	{
		std::cerr << "Error: " << ex.what() << ".\n";
		return 1;
	}
}