#define RF_NET_CLIENT
#define RF_NET_SERVER
#include "RF/net/types/socket.hpp" // header

#include "RF/net/base/utils.hpp"
#include "RF/net/types/connection.hpp"
#include "RF/net/types/message.hpp"

#include "RF/log.hpp"

RF::net::socket::socket()
:
	socket_(context_),
	work_guard_(asio::make_work_guard(context_)),
	running_(true),
	buffer_(1024)
{
	socket_.open(asio::ip::udp::v4());
	thread_ = std::thread(&RF::net::socket::start_up_context_, this);
}

RF::net::socket::socket(std::uint16_t port)
:
	socket_(context_, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)),
	work_guard_(asio::make_work_guard(context_)),
	running_(true),
	buffer_(1024)
{ thread_ = std::thread(&RF::net::socket::start_up_context_, this); }

RF::net::socket::~socket()
{ this->end_process(); }

void RF::net::socket::call_message_callback_(RF::net::connection connection, RF::net::message message)
{
	if (this->message_callback_)
	{ this->message_callback_(connection, message); }
}

void RF::net::socket::start_up_context_()
{
	this->listen_();
	context_.run();
}

void RF::net::socket::listen_()
{
	auto sender = std::make_shared<asio::ip::udp::endpoint>(asio::ip::udp::v4(), 0);

	this->socket_.async_receive_from(asio::buffer(this->buffer_), *sender,
	[this, sender](std::error_code ec, std::size_t length) -> void
	{
		if (ec)
		{
			RF::logf::error("Network error: <0>", ec.message());
			return;
		}

		if (length < 4) 
		{ return; }

		std::uint16_t id;
		std::memcpy(&id, this->buffer_.data(), sizeof(std::uint16_t));

		std::uint16_t nbodies;
		std::memcpy(&nbodies, this->buffer_.data() + sizeof(std::uint16_t), sizeof(std::uint16_t));

		RF::net::message message(id);

		if (nbodies)
		{
			std::vector<RF::net::body> bodies;
			size_t offset = 4;

			for (std::uint16_t i { 0 }; i < nbodies; ++i)
			{
				std::uint16_t size;
				std::memcpy(&size, this->buffer_.data() + offset, sizeof(std::uint16_t));
				offset += sizeof(std::uint16_t);

				std::vector<char> data(this->buffer_.begin() + offset, this->buffer_.begin() + offset + size);
				offset += size;

				bodies.emplace_back(size, data);
			}
			message = RF::net::message(id, bodies);
		}

		this->call_message_callback_(RF::net::connection(*this, *sender), message);
		this->listen_();
	});
}

void RF::net::socket::post_to_connection(RF::net::connection connection, std::vector<char> data)
{
	asio::ip::udp::endpoint endpoint = connection.endpoint();
	
	asio::post(
		this->context_, 
		[this, endpoint, data]() -> void
		{ this->socket_.async_send_to(asio::buffer(data), endpoint, RF::net::return_if_error); }
	);
}

void RF::net::socket::set_message_callback(std::function<void(RF::net::connection, RF::net::message)> callback)
{ this->message_callback_ = callback; }

void RF::net::socket::end_process()
{
	if (running_)
	{
		running_ = false;
		work_guard_.reset();
		context_.stop();

		if (thread_.joinable())
		{ thread_.join(); }
	}
}
