#pragma once

#include "RF/reference_ptr.hpp"

// asio
#include <asio/ip/udp.hpp>

namespace RF::net
{
	class socket;
	class message;

	struct connection
	{
	private:
		RF::reference_ptr<RF::net::socket> socket_;
		asio::ip::udp::endpoint endpoint_;
	public:
		connection(RF::net::socket &socket);
		connection(RF::net::socket &socket, const asio::ip::udp::endpoint endpoint);
		connection(RF::net::socket &socket, const std::string_view address, const std::uint16_t port);
		bool operator==(const RF::net::connection &other) const;
		bool operator!=(const RF::net::connection &other) const;

		asio::ip::udp::endpoint endpoint();
		void send(const RF::net::message message);
	};
} // namespace RF::net
