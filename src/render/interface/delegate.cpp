#include "RedFox/render/interface/delegate.hpp" // header

#include "RedFox/core/utils/string_utilities.hpp" // RF::format_view

RF::delegate::delegate(RF::delegate_info info) : info_(info), windows_() { }

std::unique_ptr<RF::delegate> RF::create_delegate(RF::delegate_info info)
{
	auto supported_APIs = RF::frameworks_supported();
	if (std::find(supported_APIs.begin(), supported_APIs.end(), info.framework) == supported_APIs.end())
	{ throw std::runtime_error(RF::format_view("invalid framework \'<0>\' for delegate \'<1>\'", RF::to_string(info.framework), info.name)); }

	return std::make_unique<RF::delegate>(info);
}