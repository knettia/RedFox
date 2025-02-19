#pragma once

#include <string>
#include <vector>

namespace RF
{
	enum class framework_t
	{
		X11,
		Wayland,
		Cocoa,
		Win32
	};

	inline std::vector<RF::framework_t> frameworks_supported()
	{
		#if defined(__linux__)
		return { RF::framework_t::X11, RF::framework_t::Wayland };
		#elif defined(__APPLE__)
		return { RF::framework_t::Cocoa };
		#elif defined(_WIN32)
		return { RF::framework_t::Win32 };
		#else
		return {};
		#endif
	}

	inline std::string to_string(RF::framework_t t)
	{
		switch (t)
		{
			case RF::framework_t::X11:
			{ return std::string("X11"); }
			
			case RF::framework_t::Wayland:
			{ return std::string("Wayland"); }

			case RF::framework_t::Cocoa:
			{ return std::string("Cocoa"); }

			case RF::framework_t::Win32:
			{ return std::string("Win32"); }
		}
	}
} // namespace RF