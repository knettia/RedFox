// delegate.hpp
#pragma once

#include "redfox/core/types/vector/vec2.hpp" // RF::uivec2
#include "redfox/core/interface/monitor.hpp"
#include <string>
#include <tuple>

namespace RF
{
	enum class WindowAPI
	{
		X11,
		Wayland,
		Cocoa,
		Win32
	};

	enum class GraphicsAPI
	{
		Vulkan,
		Metal,
		DirectX
	};

	struct DelegateInfo
	{
		std::string name;
		RF::WindowAPI api;
	};

	class Delegate
	{
	private:
		RF::DelegateInfo info;
	public:
		Delegate(RF::DelegateInfo info) : info(std::move(info)) { }

		void query_monitor_count(int *c);
		void query_master_monitor(int *i);

		void query_monitor_data(int i, RF::MonitorData *dst);
	};

	struct WindowCreateInfo
	{
		std::string title;
		RF::GraphicsAPI api;
		RF::uivec2 size;
		int index;
	};
}