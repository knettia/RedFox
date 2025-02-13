// monitor.hpp
#pragma once

#include "redfox/core/types/vector/vec2.hpp" // RF::uivec2
#include <string>

namespace RF
{
	struct MonitorData
	{
		std::string name;
		RF::uivec2 resolution;
		uint16_t refresh_rate;
	};
}