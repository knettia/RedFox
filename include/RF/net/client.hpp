#pragma once

#include "RF/handle_ptr.hpp"
#include "RF/library.hpp"
#include "RF/net/message.hpp"
#include "RF/net/utils.hpp"

namespace RF::net::client
{
	enum class connection_state
	{
		Disconnected,
		Connecting,
		Connected
	};

	using on_disconnect_t = void();
	using on_receive_t = void(const RF::net::message &);
	using on_timeout_t = void();
	using on_connection_state_change_t = void(const RF::net::client::connection_state);
	using on_server_state_change_t = void(const RF::net::socket_state);

	class client_impl_m
	{
	public:
		client_impl_m();
			
		void connect(const std::string_view server_addr,
		             const unsigned short server_port,
		             const std::string_view &agent = "Unnamed",
			     seconds connection_fallout = seconds(5),
		             milliseconds heartbeat_interval = milliseconds(1000),
			     milliseconds heartbeat_timeout = milliseconds(2500),
		             milliseconds server_timeout = milliseconds(10000));
		void disconnect();

		// poll once: process incoming datagrams and heartbeats/timeouts
		void process();
		void send(const RF::net::message &msg);

		connection_state connection_state();

		bool callback_on_disconnect_define(std::string_view id, on_disconnect_t ptr);
		bool callback_on_disconnect_undefine(std::string_view id);

		bool callback_on_receive_define(std::string_view id, on_receive_t ptr);
		bool callback_on_receive_undefine(std::string_view id);

		bool callback_on_timeout_define(std::string_view id, on_timeout_t ptr);
		bool callback_on_timeout_undefine(std::string_view id);

		bool callback_on_server_state_change_define(std::string_view id, on_server_state_change_t ptr);
		bool callback_on_server_state_change_undefine(std::string_view id);

		bool callback_on_connection_state_change_define(std::string_view id, on_connection_state_change_t ptr);
		bool callback_on_connection_state_change_undefine(std::string_view id);
	private:
		// callbacks
		RF::abstract_function_t<on_disconnect_t> on_disconnect;
		RF::abstract_function_t<on_receive_t> on_receive;
		RF::abstract_function_t<on_timeout_t> on_timeout;
		RF::abstract_function_t<on_server_state_change_t> on_server_state_change;
		RF::abstract_function_t<on_connection_state_change_t> on_connection_state_change;

		std::atomic<bool> handshake_complete_;
		RF::net::client::connection_state conn_state_;
		RF::net::socket_state server_socket_state_;
		time_point connect_deadline_;

		asio::io_context io_;
		asio::ip::udp::socket socket_;
		asio::ip::udp::endpoint server_endpoint_;
		std::string agent_;
		milliseconds heartbeat_interval_;
		milliseconds heartbeat_timeout_;
		milliseconds server_timeout_;
		time_point last_heartbeat_sent_;
		time_point last_heartbeat_received_;

		void initial_connection();
		void receive_all();
	};

	using client_m = RF::handle_ptr<client_impl_m>;
} // namespace RF::net::client
