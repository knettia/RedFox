#include "RedFox/render/interface/window.hpp"

#if defined (__APPLE__)
#include "RedFox/render/interface/window/cocoa_window.hpp"
#endif

RF::window::window(RF::window_info info) : info_(std::move(info)), state_(RF::window_state_t::Focused)
{ }

RF::window_info RF::window::get_info() const
{ return this->info_; }

RF::window_state_t RF::window::get_state() const
{ return this->state_; }

#include "RedFox/core/utils/string_utilities.hpp" // RF::format_view
RF::window *RF::create_window(RF::window_info info)
{
	auto supported_APIs = RF::frameworks_supported();
	if (std::find(supported_APIs.begin(), supported_APIs.end(), info.framework) == supported_APIs.end())
	{ throw std::runtime_error(RF::format_view("invalid framework \'<0>\' for window \'<1>\'", RF::to_string(info.framework), info.title)); }

	switch (info.framework)
	{
		#if defined (__APPLE__)
		case RF::framework_t::Cocoa:
		{ return reinterpret_cast<RF::window *>(new RF::cocoa_window(info)); }
		#endif

		default:
		{ throw std::runtime_error("fatal error in framework switch"); }
	}
}