#if defined (_WIN32)
#pragma once
// RedFox
#include "RedFox/render/interface/delegate.hpp"

// Windows
#include <windows.h>

namespace RF
{
	class win32_delegate : public RF::delegate
	{
	private:
		WNDCLASS window_class_;
	public:
		~win32_delegate();
		win32_delegate(RF::delegate_info info);

		void terminate() override;
		void poll_events() override;
		char32_t to_keysym(RF::virtual_key_t) override;
		RF::window *create_window(RF::window_info) override;
	};
} // namespace RF
#endif // _WIN32