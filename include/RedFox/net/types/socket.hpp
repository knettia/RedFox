#pragma once

// std
#include <atomic>
#include <memory>
#include <vector>
#include <thread>

// asio
#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>

#if defined(RF_DEBUGGER)
#define RF_NET_CLIENT
#define RF_NET_SERVER
#endif

namespace RF::net
{
	class message;
	class connection;

	class socket
	{
	private:
		asio::io_context context_;
		asio::executor_work_guard<asio::io_context::executor_type> work_guard_;
		asio::ip::udp::socket socket_;

		std::atomic_bool running_;
		std::thread thread_;

		std::vector<char> buffer_;
		std::function<void(RF::net::connection, RF::net::message)> message_callback_;

		void call_message_callback_(RF::net::connection, RF::net::message);
		void start_up_context_();
		void listen_();
	public:
		#if defined(RF_NET_CLIENT)
		socket();
		#endif

		#if defined(RF_NET_SERVER)
		socket(std::uint16_t port);
		#endif

		socket(const socket&) = delete;
		socket(socket&&) = delete;
		socket& operator=(const socket&) = delete;
		socket& operator=(socket&&) = delete;
		~socket();

		void post_to_connection(RF::net::connection connection, std::vector<char> data);
		void set_message_callback(std::function<void(RF::net::connection, RF::net::message)> callback);
		void end_process();
	};
} // namespace RF::net