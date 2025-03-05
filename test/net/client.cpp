// local
#include "./internal.hpp"

// std
#include <vector>
#include <span>

// RedFox
#include <RedFox/core/base.hpp>
#include <RedFox/render/interface.hpp>

#define RF_NET_CLIENT
#include <RedFox/net/net.hpp>

bool process_running { true };

RF::net::socket client_socket;

// TODO: use of string address and port, create endpoint internally
RF::net::connection server_connection(client_socket, "127.0.0.1", 5503);

void on_message(RF::net::connection connection, RF::net::message message)
{
	switch (message.id<internal::msg_t>())
	{
		case (internal::msg_t::Ping):
		{
			RF::ignoramus::logf(RF::ignoramus_t::info, "Received ping from server");
			break;
		}
		
		default:
		{
			RF::ignoramus::logf(RF::ignoramus_t::warning, "Unaccounted message with id <0>", message.id());
			break;
		}
	}
}

void on_terminate()
{ process_running = false; }

void on_close(RF::window *window)
{ on_terminate(); }

void handle_input(RF::window *window, RF::virtual_key_t key, RF::key_state_t state)
{
	if (state == RF::key_state_t::Triggered)
	{
		if (key == RF::virtual_key_t::A)
		{
			RF::net::message message(internal::msg_t::Ping);
			server_connection.send(message);
		}
		
		if (key == RF::virtual_key_t::M)
		{
			RF::net::message message(internal::msg_t::PingAll);
			server_connection.send(message);
		}
	}
}

int main()
{
	// set-up delegate
	RF::delegate_info delegate_info
	{
		.name = "RedFox Delegate",
		.framework = RF::framework_t::Cocoa
	};

	RF::delegate *delegate { RF::delegate::create(delegate_info) };
	delegate->set_terminate_callback(on_terminate);

	RF::window_info window_info
	{
		.title = "RedFox Networking Test",
		.graphics = RF::graphics_t::Vulkan,
		.size = RF::uivec2(250, 250)
	};

	RF::window *window { delegate->create_window(window_info) };
	window->set_close_callback(on_close);
	window->set_virtual_key_event_callback(handle_input);

	// set-up networking
	client_socket.set_message_callback(on_message);

	while (process_running)
	{
		delegate->poll_events();
	}

	// clean-up delegates
	window->close();
	delegate->terminate();

	// clean-up networking
	client_socket.end_process();

	return 0;
}