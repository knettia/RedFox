// delegate.hpp
#pragma once

#include "redfox/core/types/vector/vec2.hpp" // RF::uivec2
#include "redfox/core/interface/monitor.hpp"
#include <string>
#include <tuple>

namespace RF
{
	enum class window_API
	{
		X11,
		Wayland,
		Cocoa,
		Win32
	};

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
		RF::delegate_info info;
	public:
		delegate(RF::delegate_info info) : info(std::move(info)) { }

		int monitor_count();

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