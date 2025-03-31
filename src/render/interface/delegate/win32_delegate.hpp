#if defined (_WIN32)
#pragma once
// RedFox
#include "RF/interface/delegate.hpp"

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
		
		RF::video_mode_t current_video_mode() override;
		std::vector<RF::video_mode_t> enumerate_video_modes() override;

		DEVMODE to_native_video_mode_win32(RF::video_mode_t mode);

		void terminate() override;
		void poll_events() override;
		char32_t to_keysym(RF::virtual_key_t) override;
		RF::window *create_window(RF::window_info) override;
	};
} // namespace RF
#endif // _WIN32