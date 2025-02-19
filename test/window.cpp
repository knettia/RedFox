// RedFox
#include <RedFox/core/base.hpp>
#include <RedFox/render/interface.hpp>

RF::framework_t choose_api()
{
	#if defined (__linux__)
	return RF::framework_t::X11; // default to X11 for now
	#elif defined (__APPLE__)
	return RF::framework_t::Cocoa;
	#elif defined (_WIN32)
	return RF::framework_t::Win32;
	#endif
}

int main()
{
	RF::window_info window_info
	{
		.title = "RedFox Window",
		.framework = RF::framework_t::Cocoa,
		.graphics = RF::graphics_t::Vulkan,
		.size = RF::uivec2(500, 500),
		.monitor = 0
	};

	RF::window *window = RF::create_window(window_info);

	RF::window_info window_info2
	{
		.title = "RedFox Window 2",
		.framework = RF::framework_t::Cocoa,
		.graphics = RF::graphics_t::Vulkan,
		.size = RF::uivec2(250, 250),
		.monitor = 0
	};

	RF::window *window2 = RF::create_window(window_info2);

	for (;;)
	{
		window->poll_events();
		window2->poll_events();
	}

	delete window;
	delete window2;

	return 0;
}