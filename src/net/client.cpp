#include "RF/net/client.hpp"
#include "RF/exception.hpp"

RF::net::client::client_impl_m::client_impl_m()
:
	io_(),
	socket_(io_, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0))
{ }

void RF::net::client::client_impl_m::connect(const std::string_view server_addr,
                                             unsigned short server_port,
                                             const std::string_view &agent,
                                             seconds connection_fallout,
                                             milliseconds heartbeat_interval,
					     milliseconds heartbeat_timeout,
                                             milliseconds server_timeout)
{
	if (conn_state_ != connection_state::Disconnected) throw RF::runtime_error("Client already connecting or connected.");

	server_endpoint_ = asio::ip::udp::endpoint(
		asio::ip::address::from_string(server_addr.data()),
		server_port
	);

	agent_ = agent;
	heartbeat_interval_ = heartbeat_interval;
	heartbeat_timeout_ = heartbeat_timeout;
	server_timeout_ = server_timeout;

	last_heartbeat_sent_ = steady_clock::now();
	last_heartbeat_received_ = steady_clock::now();

	connect_deadline_ = steady_clock::now() + connection_fallout;
	conn_state_ = connection_state::Connecting;
	on_connection_state_change.call(conn_state_);

	socket_.non_blocking(true);
	initial_connection();
}

bool RF::net::client::client_impl_m::callback_on_disconnect_define(std::string_view id, on_disconnect_t ptr)
{
	return this->on_disconnect.define(id.data(), ptr);
}
bool RF::net::client::client_impl_m::callback_on_disconnect_undefine(std::string_view id)
{
	return this->on_disconnect.undefine(id.data());
}

bool RF::net::client::client_impl_m::callback_on_receive_define(std::string_view id, on_receive_t ptr)
{
	return this->on_receive.define(id.data(), ptr);
}
bool RF::net::client::client_impl_m::callback_on_receive_undefine(std::string_view id)
{
	return this->on_receive.undefine(id.data());
}

bool RF::net::client::client_impl_m::callback_on_timeout_define(std::string_view id, on_timeout_t ptr)
{
	return this->on_timeout.define(id.data(), ptr);
}
bool RF::net::client::client_impl_m::callback_on_timeout_undefine(std::string_view id)
{
	return this->on_timeout.undefine(id.data());
}

bool RF::net::client::client_impl_m::callback_on_server_state_change_define(std::string_view id, on_server_state_change_t ptr)
{
	return this->on_server_state_change.define(id.data(), ptr);
}
bool RF::net::client::client_impl_m::callback_on_server_state_change_undefine(std::string_view id)
{
	return this->on_server_state_change.undefine(id.data());
}

bool RF::net::client::client_impl_m::callback_on_connection_state_change_define(std::string_view id, on_connection_state_change_t ptr)
{
	return this->on_connection_state_change.define(id.data(), ptr);
}
bool RF::net::client::client_impl_m::callback_on_connection_state_change_undefine(std::string_view id)
{
	return this->on_connection_state_change.undefine(id.data());
}

RF::net::client::connection_state RF::net::client::client_impl_m::connection_state()
{
	return this->conn_state_;
}

void RF::net::client::client_impl_m::process()
{
	io_.poll();
	receive_all();

	const time_point now = steady_clock::now();

	if (conn_state_ == connection_state::Connecting)
	{
		if (handshake_complete_)
		{
			conn_state_ = connection_state::Connected;
			on_connection_state_change.call(conn_state_);
			last_heartbeat_received_ = now;
		}
		else if (now >= connect_deadline_)
		{
			conn_state_ = connection_state::Disconnected;
			on_connection_state_change.call(conn_state_);
			socket_.close();
			on_timeout.call();
			return;
		}
	}

	if (conn_state_ != connection_state::Connected) return;

	if (now - last_heartbeat_received_ > heartbeat_timeout_)
	{
		if (server_socket_state_ != socket_state::Dormant)
		{
			server_socket_state_ = socket_state::Dormant;
			on_server_state_change.call(server_socket_state_);
		}
	}
	else
	{
		if (server_socket_state_ != socket_state::Aroused)
		{
			server_socket_state_ = socket_state::Aroused;
			on_server_state_change.call(server_socket_state_);
		}
	}

	if (now - last_heartbeat_received_ > server_timeout_)
	{
		on_timeout.call();
		this->disconnect();
	}

	if (now - last_heartbeat_sent_ > heartbeat_interval_)
	{
		RF::net::internal_message hb;
		hb.id = RF::net::internal_message_id_t::Heartbeat;
		hb.app_id = 0;
		hb.payload.clear();
		auto buf = serialize_internal(hb);
		socket_.send_to(asio::buffer(buf), server_endpoint_);
		last_heartbeat_sent_ = now;
	}
}


void RF::net::client::client_impl_m::send(const RF::net::message &msg)
{
	auto buf = serialize_public(msg);
	this->socket_.send_to(asio::buffer(buf), server_endpoint_);
}

void RF::net::client::client_impl_m::disconnect()
{
	RF::net::internal_message dc;
	dc.id = RF::net::internal_message_id_t::Disconnect;
	dc.app_id = 0;
	dc.payload.clear();
	auto buf = serialize_internal(dc);

	this->socket_.send_to(asio::buffer(buf), server_endpoint_);
	this->socket_.close();

	this->on_disconnect.call();
}

void RF::net::client::client_impl_m::initial_connection()
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

void RF::net::client::client_impl_m::receive_all()
{
	if (!handshake_complete_) handshake_complete_ = true;

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
				this->on_receive.call(to_public(im));
			}
			else if (im.id == RF::net::internal_message_id_t::Disconnect)
			{
				this->disconnect();
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
				throw e;
			}
		}
	}
}
