#pragma once

#include <asio.hpp>
#include "RF/net/message.hpp"
#include "RF/net/utils.hpp"

namespace RF::net::server
{
	using steady_clock = std::chrono::steady_clock;
	using time_point = std::chrono::steady_clock::time_point;
	using milliseconds = std::chrono::milliseconds;
	using seconds = std::chrono::seconds;

	enum client_state
	{
		Aroused,
		Dormant
	};

	enum disconnect_reason
	{
		Voluntary,
		Timeout
	};

	struct client_info
	{
		asio::ip::udp::endpoint endpoint;
		std::string agent;
		client_state state;
		time_point last_heartbeat;
	};

	using on_connect_t = std::function<void(const client_info &)>;
	using on_disconnect_t = std::function<void(const client_info &, disconnect_reason)>;
	using on_receive_t = std::function<void(const client_info &, const RF::net::message &)>;
	using on_state_change_t = std::function<void(const client_info &, client_state, client_state)>;

	class server_m
	{
	public:
		// construct server bound to port
		server_m(asio::io_context &io, unsigned short port,
		         milliseconds heartbeat_timeout = milliseconds(5000),
		         milliseconds disconnect_timeout = milliseconds(15000),
		         milliseconds heartbeat_interval = milliseconds(1000));
		
		// callbacks
		// TODO: make them internal like RF::window
		on_connect_t on_connect;
		on_disconnect_t on_disconnect;
		on_receive_t on_receive;
		on_state_change_t on_state_change;

		// poll once: process incoming datagrams and run heartbeat/timeouts (non-blocking)
		void process();

		// send application message to a client
		void send(const asio::ip::udp::endpoint &client_endpoint, const RF::net::message &msg);

		// convenience: send to all clients
		void broadcast(const RF::net::message &msg);
	private:
		asio::ip::udp::socket socket_;
		std::unordered_map<asio::ip::udp::endpoint, client_info, RF::net::asio_endpoint_hash, RF::net::asio_endpoint_equal> clients_;
		milliseconds heartbeat_timeout_;
		milliseconds disconnect_timeout_;
		milliseconds heartbeat_interval_;
		time_point last_heartbeat_sent_;

		// receive loop non-blocking, process messages immediately
		void receive_all();

		void send_heartbeats();
	};


} // namespace RF::net
