#include "RF/net/server.hpp"

RF::net::server::server_m::server_m(asio::io_context &io,
                                    unsigned short port,
                                    milliseconds heartbeat_timeout,
                                    milliseconds disconnect_timeout,
                                    milliseconds heartbeat_interval)
:
	socket_(io, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)),
	heartbeat_timeout_(heartbeat_timeout),
	disconnect_timeout_(disconnect_timeout),
	heartbeat_interval_(heartbeat_interval),
	last_heartbeat_sent_(steady_clock::now())
{
	socket_.non_blocking(true);
}

void RF::net::server::server_m::process()
{
	this->receive_all();

	const auto now = steady_clock::now();

	// send heartbeats if needed
	if (now - last_heartbeat_sent_ > heartbeat_interval_)
	{
		this->send_heartbeats();
		last_heartbeat_sent_ = now;
	}

	// check client timeouts and state changes
	std::vector<asio::ip::udp::endpoint> to_remove;

	for (auto &kv : clients_)
	{
		auto &ci = kv.second;
		auto prev_state = ci.state;
		auto time_since = std::chrono::duration_cast<milliseconds>(now - ci.last_heartbeat);

		if (time_since > disconnect_timeout_)
		{
			if (on_disconnect)
			{
				on_disconnect(ci, disconnect_reason::Timeout);
			}
			to_remove.push_back(ci.endpoint);
		}
		else if (time_since > heartbeat_timeout_ && ci.state != client_state::Dormant)
		{
			ci.state = client_state::Dormant;
			if (on_state_change)
			{
				on_state_change(ci, prev_state, ci.state);
			}
		}
		else if (time_since <= heartbeat_timeout_ && ci.state != client_state::Aroused)
		{
			ci.state = client_state::Aroused;
			if (on_state_change)
			{
				on_state_change(ci, prev_state, ci.state);
			}
		}
	}

	for (auto &ep : to_remove)
	{
		clients_.erase(ep);
	}
}


void RF::net::server::server_m::send(const asio::ip::udp::endpoint &client_endpoint,
          const RF::net::message &msg)
{
	auto buf = RF::net::serialize_public(msg);
	this->socket_.send_to(asio::buffer(buf), client_endpoint);
	// no flush necessary for UDP
}

void RF::net::server::server_m::broadcast(const RF::net::message &msg)
{
	auto buf = RF::net::serialize_public(msg);
	for (auto &kv : this->clients_)
	{
		socket_.send_to(asio::buffer(buf), kv.second.endpoint);
	}
}

void RF::net::server::server_m::receive_all()
{
	std::vector<std::uint8_t> recv_buf(65536);
	for (;;)
	{
		asio::ip::udp::endpoint sender;
		try
		{
			std::size_t len = socket_.receive_from(asio::buffer(recv_buf), sender);
			if (len == 0) continue;

			auto it = clients_.find(sender);
			if (it == clients_.end())
			{
				// new client
				client_info ci;
				ci.endpoint = sender;
				ci.agent = "";
				ci.state = client_state::Aroused;
				ci.last_heartbeat = steady_clock::now();
				clients_.emplace(sender, ci);
				it = clients_.find(sender);
				if (on_connect)
				{
					on_connect(it->second);
				}
			}

			client_info &ci = it->second;
			ci.last_heartbeat = steady_clock::now();

			RF::net::internal_message im = deserialize_internal(recv_buf.data(), len);

			if (im.id == RF::net::internal_message_id_t::Application)
			{
				if (on_receive)
				{
					on_receive(ci, to_public(im));
				}
			}
			else if (im.id == RF::net::internal_message_id_t::Disconnect)
			{
				if (on_disconnect)
				{
					on_disconnect(ci, disconnect_reason::Voluntary);
				}
				clients_.erase(sender);
			}
			else if (im.id == RF::net::internal_message_id_t::SetAgent)
			{
				// interpret payload as UTF-8 string for agent
				ci.agent = std::string(im.payload.front().begin(), im.payload.front().end());
			}
			else if (im.id == RF::net::internal_message_id_t::Heartbeat)
			{
				// nothing else to do; we already updated last_heartbeat
			}
		}
		catch (const asio::system_error &e)
		{
			if (e.code() == asio::error::would_block || e.code() == asio::error::try_again)
			{
				// no more data
				break;
			}
			else
			{
				// other socket error: rethrow
				throw e;
			}
		}
	}
}

void RF::net::server::server_m::send_heartbeats()
{
	RF::net::internal_message hb;
	hb.id = RF::net::internal_message_id_t::Heartbeat;
	hb.app_id = 0;
	hb.payload.clear();
	auto buf = serialize_internal(hb);

	for (auto &kv : clients_)
	{
		socket_.send_to(asio::buffer(buf), kv.second.endpoint);
	}
}
