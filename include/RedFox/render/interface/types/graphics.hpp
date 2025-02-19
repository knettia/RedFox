#pragma once

#include <string>

namespace RF
{
	enum class graphics_t
	{
		Vulkan,
		Metal,
		DirectX
	};

	inline std::string to_string(RF::graphics_t t)
	{
		switch (t)
		{
			case RF::graphics_t::Vulkan:
			{ return std::string("Vulkan"); }
			
			case RF::graphics_t::Metal:
			{ return std::string("Metal"); }

			case RF::graphics_t::DirectX:
			{ return std::string("DirectX"); }
		}
	}
} // namespace RF