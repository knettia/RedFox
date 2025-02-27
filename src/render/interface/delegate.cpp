#include "RedFox/render/interface/delegate.hpp" // header

#if defined (__APPLE__)
#include "RedFox/render/interface/delegate/cocoa_delegate.hpp"
#elif defined (_WIN32)
#include "RedFox/render/interface/delegate/win32_delegate.hpp"
#endif

#include <stdexcept>

RF::delegate::delegate(RF::delegate_info info) : info_(info)
{
	if (created_)
	{ throw std::runtime_error("internal inconsistency: multiple instances of RF::delegate are not allowed"); }

	created_ = true;
}

RF::delegate::~delegate()
{ created_ = false; }

#include "RedFox/core/utils/string_utilities.hpp" // RF::format_view
#include "RedFox/render/interface/types/framework.hpp"
RF::delegate *RF::delegate::create(RF::delegate_info info)
{
	auto supported_APIs = RF::frameworks_supported();
	if (std::find(supported_APIs.begin(), supported_APIs.end(), info.framework) == supported_APIs.end())
	{ throw std::runtime_error(RF::format_view("invalid framework \'<0>\' for delegate \'<1>\'", RF::to_string(info.framework), info.name)); }

	switch (info.framework)
	{
		#if defined (__linux)
		// TODO: implement RF::x11_delegate and RF::wl_delegate
		#elif defined (__APPLE__)
		case RF::framework_t::Cocoa:
		{ return new RF::cocoa_delegate(info); }
		#elif defined (_WIN32)
		case RF::framework_t::Win32:
		{ return new RF::win32_delegate(info); }
		#endif

		default:
		{ throw std::runtime_error("fatal error in framework switch"); }
	}
}