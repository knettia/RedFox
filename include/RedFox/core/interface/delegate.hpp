// delegate.hpp
#pragma once

#include "RedFox/core/types/vector/vec2.hpp" // RF::uivec2
#include "RedFox/core/interface/monitor.hpp"

#include <string>

namespace RF
{
	enum class window_API
	{
		X11,
		Wayland,
		Cocoa,
		Win32
	};

	inline std::vector<window_API> window_API_supported()
	{
		#if defined(__linux__)
		return { RF::window_API::X11, RF::window_API::Wayland };
		#elif defined(__APPLE__)
		return { RF::window_API::Cocoa };
		#elif defined(_WIN32)
		return { RF::window_API::Win32 };
		#else
		return {};
		#endif
	}

	enum class graphics_API
	{
		Vulkan,
		Metal,
		DirectX
	};

	struct delegate_info
	{
		std::string name;
		RF::window_API api;
	};

	class delegate
	{
	private:
		RF::delegate_info info_;

		// internal functions
		int              x11_monitor_count();
		RF::monitor_data x11_monitor_data(int i);

		int              wl_monitor_count();
		RF::monitor_data wl_monitor_data(int i);

		int              cocoa_monitor_count();
		RF::monitor_data cocoa_monitor_data(int i);

		int              win32_monitor_count();
		RF::monitor_data win32_monitor_data(int i);

		std::function<int()>                 func_monitor_count;
		std::function<RF::monitor_data(int)> func_monitor_data;
	public:
		delegate(RF::delegate_info info);

		// public accessors 
		int              monitor_count();
		RF::monitor_data monitor_data(int i);
	};

	struct window_create_info
	{
		std::string title;
		RF::graphics_API api;
		RF::uivec2 size;
		int monitor;
	};
} // namespace RF