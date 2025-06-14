#pragma once

#include "RF/definitions.hpp"
#if defined (__LINUX__) || defined (__BSD_KERNEL__)

// RedFox
#include "RF/interface/delegate.hpp"
#include "../dyfuncs/x11_dyfuncs.hpp"

// X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace RF
{
	// forward declare
	class x11_window;

	class x11_delegate : public RF::delegate
	{
	protected:
		// X11 functions
		const RF::x11_dyfuncs dyfuncs_;

		Display *display_ = nullptr;
		std::unordered_map<Window, RF::x11_window*> window_map_;
	public:
		~x11_delegate();
		x11_delegate(RF::delegate_info info);

		RF::video_mode_t current_video_mode() override;
		std::vector<RF::video_mode_t> enumerate_video_modes() override;

		void register_x11_window(Window x_window, RF::x11_window *window);
		void deregister_x11_window(Window x_window);
		void dispatch_x11_event(XEvent &event);

		void terminate() override;
		void poll_events() override;
		char32_t to_keysym(RF::virtual_key_t) override;
		RF::window *create_window(RF::window_info) override;

		Display *get_x11_display();
	};
} // namespace RF

#endif
