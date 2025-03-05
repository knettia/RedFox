// local
#include "./internal.hpp"

// RedFox
#include <RedFox/core/base.hpp>

#define RF_NET_SERVER
#include <RedFox/net/net.hpp>

RF::net::socket server_socket(5503);
std::vector<RF::net::connection> clients;

void on_message(RF::net::connection connection, RF::net::message message)
{
	asio::ip::udp::endpoint endpoint = connection.endpoint();

	auto it = std::find(clients.begin(), clients.end(), connection);
	if (it == clients.end())
	{
		clients.push_back(connection);
		RF::ignoramus::logf(RF::ignoramus_t::info, "New client registered: [<0>]:<1>", endpoint.address(), endpoint.size());
	}

	switch (message.id<internal::msg_t>())
	{
		case (internal::msg_t::Ping):
		{
			RF::ignoramus::logf(RF::ignoramus_t::info, "Received ping from [<0>]:<1>", endpoint.address(), endpoint.size());
			connection.send(RF::net::message(internal::msg_t::PingAll));
			break;
		}
		
		case (internal::msg_t::PingAll):
		{
			RF::ignoramus::logf(RF::ignoramus_t::info, "Received super ping request from [<0>]:<1>", endpoint.address(), endpoint.size());
			RF::net::message message(internal::msg_t::Ping);
			
			for (RF::net::connection client : clients)
			{
				if (client != connection)
				{
					client.send(message);
	
					asio::ip::udp::endpoint client_endpoint = client.endpoint();
					RF::ignoramus::logf(RF::ignoramus_t::info, "Sent ping to [<0>]:<1>", client_endpoint.address(), client_endpoint.size());
				}
			}

			break;
		}
		
		default:
		{
			RF::ignoramus::logf(RF::ignoramus_t::warning, "Unaccounted message with id <0> from [<0>]:<1>", message.id(), endpoint.address(), endpoint.size());
			break;
		}
	}
}

int main()
{
	server_socket.set_message_callback(on_message);

	for (;;)
	{

	}

	return 0;
}
