#include "RF/interface/delegate.hpp" // header
#include "RF/interface/framework.hpp"

#include "RF/definitions.hpp"

#if defined (__LINUX__) || defined (__BSD_KERNEL__)
#include "./delegate/x11_delegate.hpp"
#elif defined (__APPLE__)
#include "./delegate/cocoa_delegate.hpp"
#elif defined (_WIN32)
#include "./delegate/win32_delegate.hpp"
#endif

#include "RF/exception.hpp"
#include "RF/string.hpp"

RF::delegate::delegate(RF::delegate_info info, const RF::video_mode_t mode) : info_(info), video_mode_(mode)
{
	if (created_)
	{ throw RF::engine_error("Internal inconsistency: multiple instances of RF::delegate are not allowed"); }

	created_ = true;
}

RF::delegate::~delegate()
{ created_ = false; }

RF::delegate *RF::delegate::create(RF::delegate_info info)
{
	auto supported_APIs = RF::frameworks_supported();
	if (std::find(supported_APIs.begin(), supported_APIs.end(), info.framework) == supported_APIs.end())
	{ throw RF::engine_error(RF::format_view("Invalid framework \'<0>\' for delegate \'<1>\'", RF::to_string(info.framework), info.name)); }

	switch (info.framework)
	{
		#if defined (__LINUX__)
		case RF::framework_t::X11:
		{ return new RF::x11_delegate(info); }
		// TODO: implement RF::wl_delegate
		#elif defined (__APPLE__)
		case RF::framework_t::Cocoa:
		{ return new RF::cocoa_delegate(info); }
		#elif defined (__WINDOWS__)
		case RF::framework_t::Win32:
		{ return new RF::win32_delegate(info); }
		#endif

		default:
		{ throw RF::runtime_error("Fatal error: impossible default case reached in RF::framework_t switch"); }
	}
}
