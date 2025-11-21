// local
#include "./internal.hpp"

// std
#include <unordered_map>
#include <string>

// RedFox
#include <RF/log.hpp>
#include <RF/log.hpp>
#include <RF/net/server.hpp>

enum class msg_t : std::uint32_t
{
	Ping,
	PingAll
};

int main()
{
	asio::io_context io;
	RF::net::server::server_m server(io, 5503);

	server.on_connect = [](const RF::net::server::client_info &client)
	{
		RF::logf::info("Client connected: <0>:<1>", client.endpoint.address().to_string(), client.endpoint.port());
	};

	server.on_disconnect = [](const RF::net::server::client_info &client, RF::net::server::disconnect_reason reason)
	{
		RF::logf::warn("Client disconnected: <0>:<1> (reason: <2>)", client.endpoint.address().to_string(), client.endpoint.port(), (int)reason);
	};

	server.on_receive = [&server](const RF::net::server::client_info &client, const RF::net::message &msg)
	{
		auto id = static_cast<msg_t>(msg.id);
		switch (id)
		{
			case msg_t::Ping:
			{
				RF::logf::info("Received Ping from <0>:<1>", client.endpoint.address().to_string(), client.endpoint.port());

				RF::net::message msg;
				msg.id = static_cast<std::uint32_t>(msg_t::Ping);
				msg.payload = {};

				server.send(client.endpoint, msg);
				break;
			}

			case msg_t::PingAll:
			{
				RF::logf::info("Received PingAll request from <0>:<1>", client.endpoint.address().to_string(), client.endpoint.port());

				RF::net::message msg;
				msg.id = static_cast<std::uint32_t>(msg_t::Ping);
				msg.payload = {};

				server.broadcast(msg);
				break;
			}

			default:
			{
				RF::logf::warn("Unrecognised message (<0>) from <1>:<2>", (std::uint32_t)id, client.endpoint.address().to_string(), client.endpoint.port());
				break;
			}
		}
	};

	for (;;)
	{
		server.process();
		io.poll();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	return 0;
}
