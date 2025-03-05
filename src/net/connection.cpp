#include "RedFox/net/types/connection.hpp" // header

#include "RedFox/net/base/utils.hpp"
#include "RedFox/net/types/socket.hpp"
#include "RedFox/net/types/message.hpp"

RF::net::connection::connection(RF::net::socket &socket)
:
	socket_(&socket)
{ }

RF::net::connection::connection(RF::net::socket &socket, const asio::ip::udp::endpoint endpoint)
:
	socket_(&socket),
	endpoint_(endpoint)
{ }

bool RF::net::connection::operator==(const connection &other) const
{
	return socket_ == other.socket_ && endpoint_ == other.endpoint_;
}

bool RF::net::connection::operator!=(const connection &other) const
{
	return !(*this == other);
}

asio::ip::udp::endpoint RF::net::connection::endpoint()
{
	return this->endpoint_;
}

void RF::net::connection::send(const RF::net::message message)
{
	std::vector<char> buffer;
	
	std::uint16_t id { message.id() };
	buffer.insert(buffer.end(), RF::net::data(id), RF::net::data(id) + sizeof(std::uint16_t));
	
	std::uint16_t nbodies { static_cast<std::uint16_t>(message.bodies().size()) };
	buffer.insert(buffer.end(), RF::net::data(nbodies), RF::net::data(nbodies) + sizeof(std::uint16_t));
	
	std::span<const RF::net::body> bodies { message.bodies() };
	for (const RF::net::body &body : bodies)
	{
		std::uint16_t size { body.size() };
		buffer.insert(buffer.end(), RF::net::data(size), RF::net::data(size) + sizeof(std::uint16_t));
		
		std::span<const char> data { body.data() };
		buffer.insert(buffer.end(), data.begin(), data.end());
	}

	this->socket_->post_to_connection(*this, buffer);
}