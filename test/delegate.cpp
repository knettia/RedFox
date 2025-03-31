// RedFox
#include "RF/interface/graphics.hpp"
#include "RF/interface/framework.hpp"
#include "RF/interface/virtual_key.hpp"
#include <RF/interface/delegate.hpp>

#include <RF/definitions.hpp>
#include <RF/monitor.hpp>
#include <RF/log.hpp>

RF::framework_t choose_framework()
{
#if defined (__DARWIN__)
	return RF::framework_t::Cocoa;
#elif defined (__WINDOWS__)
	return RF::framework_t::Win32;
#endif
}

struct
{
	RF::delegate *delegate;
	RF::window *main_window;
	bool running = true;

	void setup_delegate()
	{
		RF::delegate_info delegate_info
		{
			.name = "RedFox Delegate",
			.framework = choose_framework()
		};

		this->delegate = RF::delegate::create(delegate_info);
	}

	void setup_window()
	{
		RF::window_info window_info
		{
			.title = "RedFox Window",
			.graphics = RF::graphics_t::Vulkan,
			.size = RF::uivec2(1600, 900)
		};
		
		this->main_window = this->delegate->create_window(window_info);
	}

	void clean_up()
	{
		this->main_window->close();
		this->delegate->terminate();
	}
} game_process;

void handle_termination()
{
	/*
	 * simply terminate the process for now
	 * but we could do absolutely anything here
	 * e.g. create warnings for saving
	 * or simply not allow the program to close
	 */
	game_process.running = false; // terminate
}

int main()
{
	RF::monitor_m::activate(true);
	RF::monitor_m::set_save_path("crash.log2");

	game_process.setup_delegate();
	game_process.delegate->set_terminate_callback(
	[]() -> void
	{
		RF::logf::info("user requested termination via RF::delegate");
		handle_termination();
	});
	
	game_process.setup_window();
	game_process.main_window->set_close_callback(
	[](RF::window *window) -> void
	{
		RF::logf::info("user requested termination via RF::window");
		handle_termination();
	});
	
	game_process.main_window->set_virtual_key_event_callback(
	[](RF::window *window, RF::virtual_key_t key, RF::key_state_t state) -> void
	{
		if (state == RF::key_state_t::Triggered)
		{
			if (key == RF::virtual_key_t::A)
			{
				window->set_flag(RF::window_flag_bit_t::Fullscreen, true);
			}
			if (key == RF::virtual_key_t::S)
			{
				window->set_flag(RF::window_flag_bit_t::Fullscreen, false);
			}
		}
	});
	
	while (game_process.running)
	{ game_process.delegate->poll_events(); }

	game_process.clean_up();
	
	return 0;
}
