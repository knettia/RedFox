#include "RedFox/core/interface/delegate.hpp"

RF::delegate::delegate(RF::delegate_info info)
{
	auto supported_APIs = RF::window_API_supported();
	if (std::find(supported_APIs.begin(), supported_APIs.end(), info.api) == supported_APIs.end())
	{ throw std::runtime_error("unsupported window API for delegate"); }

	#if defined(__linux__)
	if (RF::window_API::X11 == info.api)
	{
		this->func_monitor_count = [this]()
		{ return this->x11_monitor_count(); };

		this->func_monitor_data = [this](int i)
		{ return this->x11_monitor_data(i); };
	}
	else if (RF::window_API::Wayland == info.api)
	{
		this->func_monitor_count = [this]()
		{ return this->wl_monitor_count(); };

		this->func_monitor_data = [this](int i)
		{ return this->wl_monitor_data(i); };
	}
	#elif defined(__APPLE__)
	this->func_monitor_count = [this]()
	{ return this->cocoa_monitor_count(); };

	this->func_monitor_data = [this](int i)
	{ return this->cocoa_monitor_data(i); };
	#elif defined(_WIN32)
	this->func_monitor_count = [this]()
	{ return this->win32_monitor_count(); };

	this->func_monitor_data = [this](int i)
	{ return this->win32_monitor_data(i); };
	#endif

	this->info_ = std::move(info);
}

int RF::delegate::monitor_count()
{ return this->func_monitor_count(); }

RF::monitor_data RF::delegate::monitor_data(int i)
{ return this->func_monitor_data(i); }