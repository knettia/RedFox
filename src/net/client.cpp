#include "RF/net/client.hpp"

RF::net::client::client_m::client_m(asio::io_context &io,
         const std::string_view server_addr,
         const unsigned short server_port,
         const std::string_view &agent,
         milliseconds heartbeat_interval,
         milliseconds server_timeout)
:
	socket_(io, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0)),
	server_endpoint_(asio::ip::address::from_string(server_addr.data()), server_port),
	agent_(agent),
	heartbeat_interval_(heartbeat_interval),
	server_timeout_(server_timeout),
	last_heartbeat_sent_(steady_clock::now()),
	last_heartbeat_received_(steady_clock::now())
{
	socket_.non_blocking(true);
	initial_connection();
}

void RF::net::client::client_m::process()
{
	this->receive_all();

	const time_point now = steady_clock::now();

	// server timeout
	if (now - last_heartbeat_received_ > server_timeout_)
	{
		if (on_server_timeout)
		{
			on_server_timeout();
		}
	}

	// send heartbeat if needed
	if (now - last_heartbeat_sent_ > heartbeat_interval_)
	{
		RF::net::internal_message hb;
		hb.id = RF::net::internal_message_id_t::Heartbeat;
		hb.app_id = 0;
		hb.payload.clear();
		auto buf = serialize_internal(hb);
		this->socket_.send_to(asio::buffer(buf), server_endpoint_);
		last_heartbeat_sent_ = now;
	}
}

void RF::net::client::client_m::send(const RF::net::message &msg)
{
	auto buf = serialize_public(msg);
	this->socket_.send_to(asio::buffer(buf), server_endpoint_);
}

void RF::net::client::client_m::disconnect()
{
	RF::net::internal_message dc;
	dc.id = RF::net::internal_message_id_t::Disconnect;
	dc.app_id = 0;
	dc.payload.clear();
	auto buf = serialize_internal(dc);
	this->socket_.send_to(asio::buffer(buf), server_endpoint_);
	this->socket_.close();
	if (on_disconnect)
	{
		on_disconnect();
	}
}

void RF::net::client::client_m::initial_connection()
{
	// send a zero-length connect packet (nothing special with UDP)
	// but immediately send SetAgent as initial handshake
	RF::net::internal_message im;
	im.id = RF::net::internal_message_id_t::SetAgent;
	im.app_id = 0;
	im.payload.resize(1);
	im.payload.front().assign(agent_.begin(), agent_.end());

	auto buf = serialize_internal(im);
	this->socket_.send_to(asio::buffer(buf), server_endpoint_);

	// after sending agent, we expect server responses; we rely on process() caller to poll
	last_heartbeat_sent_ = steady_clock::now();
	last_heartbeat_received_ = steady_clock::now();
}

void RF::net::client::client_m::receive_all()
{
	std::vector<std::uint8_t> recv_buf(65536);
	for (;;)
	{
		asio::ip::udp::endpoint sender;
		try
		{
			std::size_t len = socket_.receive_from(asio::buffer(recv_buf), sender);
			if (len == 0) continue;

			RF::net::internal_message im = deserialize_internal(recv_buf.data(), len);
			last_heartbeat_received_ = steady_clock::now();

			if (im.id == RF::net::internal_message_id_t::Application)
			{
				if (on_receive)
				{
					on_receive(to_public(im));
				}
			}
			else if (im.id == RF::net::internal_message_id_t::Disconnect)
			{
				if (on_disconnect)
				{
					on_disconnect();
				}
				this->socket_.close();
				return;
			}
			else if (im.id == RF::net::internal_message_id_t::Heartbeat)
			{
				// nothing else to do
			}
		}
		catch (const asio::system_error &e)
		{
			if (e.code() == asio::error::would_block || e.code() == asio::error::try_again)
			{
				break;
			}
			else
			{
				throw;
			}
		}
	}
}
