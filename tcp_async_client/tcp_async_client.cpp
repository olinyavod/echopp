#include <iostream>
#include <memory>
#include <filesystem>
#include <algorithm>

#include <asio.hpp>

using asio::ip::tcp;
using std::filesystem::path;
using io_context_ptr = std::shared_ptr<asio::io_context>;
using socket_ptr = std::shared_ptr<tcp::socket>;
using endpoint_iterator = tcp::resolver::iterator;

class connection : public std::enable_shared_from_this<connection>, asio::noncopyable
{
private:
	static const std::size_t max_buffer = 1024;	
	tcp::socket sock_;
	endpoint_iterator ep_;
	bool started_ = false;
	std::string message_;

public:
	using connection_ptr = std::shared_ptr<connection>;
	static auto start(const io_context_ptr& io_context, 
		const endpoint_iterator& endpoint,
		const std::string& message)
	{
		connection_ptr self(new connection(io_context, endpoint));

		self->start(message);

		return self;
	}
	
	auto started() const { return started_; }
	
	auto start(const std::string& message)->void
	{
		if (started())
			return;

		auto self = shared_from_this();
		auto msg = std::make_shared<std::string>(message);

		asio::async_connect(sock_, 
			ep_,
			[self, msg](asio::error_code err, endpoint_iterator endpoint)
			{
				self->on_connected(err, *msg);
			});
	}

	auto stop()->void
	{
		if (!started())
			return;

		started_ = false;
		sock_.close();
	}	

private:
	connection(const io_context_ptr& io_context, const endpoint_iterator& ep)
		: sock_(*io_context)
		, ep_(ep)
		, message_()
	{
		
	}
	
	auto on_connected(const asio::error_code& err, const std::string& message)->void
	{
		if (err)
			return;
		
		started_ = true;

		message_ = message;
		do_write(message);
	}

	auto do_read()->void
	{
		if (!started())
			return;
		
		const auto buffer = std::make_shared<std::array<char, max_buffer>>();
		const auto self = shared_from_this();
		
		asio::async_read(sock_,
			asio::buffer(*buffer), 
			[self, buffer](asio::error_code err, std::size_t bytes)->std::size_t
			{
				return self->read_completion(err, buffer->data(), bytes);
			},
			[self, buffer](asio::error_code err, std::size_t bytes)
			{
				self->on_read(err, buffer->data(), bytes);
			});
	}

	auto read_completion(const asio::error_code& error, char* buffer, std::size_t bytes) const ->std::size_t
	{
		if (error)
			return 0;
		
		auto* end = buffer + bytes;
		return std::find(buffer, end, '\n') < end ? 0 : 1;
	}

	auto on_read(const asio::error_code& err, const char* buffer, const std::size_t bytes) const ->void
	{
		const std::string msg(buffer, bytes - 1);

		std::cout << "[Echo] " << message_
			<< ": "
			<< (msg == message_ ? "OK" : "Fail")
			<< std::endl;
	}

	auto do_write(const std::string& message)->void
	{
		if (!started())
			return;

		const auto msg = std::make_shared<std::string>(message + '\n');
		auto self = shared_from_this();
		sock_.async_write_some(asio::buffer(*msg), 
			[self, msg](asio::error_code err, std::size_t bytes)
			{
				self->on_write(err);
			});
	}

	auto on_write(const asio::error_code& err)->void
	{
		do_read();
	}
};

auto resolve(asio::io_context& io_context, 
	const std::string& host, 
	const std::uint16_t port)->endpoint_iterator
{
	const tcp::resolver::query query(host, std::to_string(port));
	tcp::resolver resolver(io_context);

	return resolver.resolve(query);
}

int main(int argc, char *argv[])
{
	try
	{
		const auto io_context = std::make_shared<asio::io_context>();
		const std::uint16_t port = 9090;
		const std::string host = "localhost";
		const auto endpoints = resolve(*io_context, host, port);

		for(int i = 1; i<argc; ++i)
		{
			connection::start(io_context, endpoints, argv[i]);
		}
		
		io_context->run();
		
		return 0;
	}
	catch(std::exception& ex)
	{
		std::cerr << "Error: " << ex.what() << "\n.";
		return 1;
	}
}