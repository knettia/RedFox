#pragma once

#include <functional>

#include "RF/net/message.hpp"
#include "RF/net/utils.hpp"

namespace RF::net::client
{
	using on_disconnect_t = std::function<void()>;
	using on_receive_t = std::function<void(const RF::net::message &)>;
	using on_server_timeout_t = std::function<void()>;

	class client_m
	{
	public:
		client_m(asio::io_context &io,
		         const std::string_view server_addr,
		         const unsigned short server_port,
		         const std::string_view &agent = "Unnamed",
		         milliseconds heartbeat_interval = milliseconds(1000),
		         milliseconds server_timeout = milliseconds(10000));

		// callbacks
		on_disconnect_t on_disconnect;
		on_receive_t on_receive;
		on_server_timeout_t on_server_timeout;

		// poll once: process incoming datagrams and heartbeats/timeouts
		void process();
		void send(const RF::net::message &msg);
		void disconnect();
	private:
		asio::ip::udp::socket socket_;
		asio::ip::udp::endpoint server_endpoint_;
		std::string agent_;
		milliseconds heartbeat_interval_;
		milliseconds server_timeout_;
		time_point last_heartbeat_sent_;
		time_point last_heartbeat_received_;

		void initial_connection();
		void receive_all();
	};

} // namespace RF::net::client
