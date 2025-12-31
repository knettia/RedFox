#pragma once

#include <asio.hpp>
#include "RF/handle_ptr.hpp"
#include "RF/library.hpp"
#include "RF/net/message.hpp"
#include "RF/net/utils.hpp"

namespace RF::net::server
{
	enum class disconnect_reason
	{
		Voluntary,
		Timeout
	};

	struct client_info
	{
		asio::ip::udp::endpoint endpoint;
		std::string agent;
		RF::net::socket_state state;
		time_point last_heartbeat;
	};

	using on_connect_t = void(const client_info &);
	using on_disconnect_t = void(const client_info &, disconnect_reason);
	using on_receive_t = void(const client_info &, const RF::net::message &);
	using on_state_change_t = void(const client_info &, RF::net::socket_state, RF::net::socket_state);

	class server_impl_m
	{
	public:
		server_impl_m();

		// construct server bound to port
		void host(unsigned short port,
		         milliseconds heartbeat_timeout = milliseconds(5000),
		         milliseconds disconnect_timeout = milliseconds(15000),
		         milliseconds heartbeat_interval = milliseconds(1000));

		// poll once: process incoming datagrams and run heartbeat/timeouts (non-blocking)
		void process();

		// send application message to a client
		void send(const asio::ip::udp::endpoint &client_endpoint, const RF::net::message &msg);

		// convenience: send to all clients
		void broadcast(const RF::net::message &msg);

		bool callback_on_connect_define(std::string_view id, std::function<on_connect_t> ptr);
		bool callback_on_connect_undefine(std::string_view id);

		bool callback_on_disconnect_define(std::string_view id, std::function<on_disconnect_t> ptr);
		bool callback_on_disconnect_undefine(std::string_view id);

		bool callback_on_receive_define(std::string_view id, std::function<on_receive_t> ptr);
		bool callback_on_receive_undefine(std::string_view id);

		bool callback_on_state_change_define(std::string_view id, std::function<on_state_change_t> ptr);
		bool callback_on_state_change_undefine(std::string_view id);
	private:
		// callbacks
		std::atomic<bool> hosting_;
			
		RF::abstract_function_t<on_connect_t> on_connect;
		RF::abstract_function_t<on_disconnect_t> on_disconnect;
		RF::abstract_function_t<on_receive_t> on_receive;
		RF::abstract_function_t<on_state_change_t> on_state_change;

		asio::io_context io_;

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

	using server_m = RF::handle_ptr<server_impl_m>;

} // namespace RF::net
