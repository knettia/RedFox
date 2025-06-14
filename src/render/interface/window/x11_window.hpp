#pragma once

#include "RF/definitions.hpp"
#if defined (__LINUX__) || defined (__BSD_KERNEL__)

// RedFox
#include "RF/interface/video_mode.hpp"
#include "RF/interface/window.hpp"

#include "../dyfuncs/x11_dyfuncs.hpp"

// X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace RF
{
	// forward declare
	class x11_delegate;

	class x11_window : public RF::window
	{
	private:
		const RF::x11_dyfuncs dyfuncs_;

		RF::reference_ptr<x11_delegate> x11_delegate_;

		Window window_ = 0;
		Atom wm_delete_window_;

		void handle_window_restore_() override;
		void handle_window_fullscreen_() override;

		void handle_flag_update_(RF::window_flag_bit_t flag, bool enabled) override;
		void handle_set_cursor_position_(const RF::uivec2 point) override;
	public:
		~x11_window() override;
		x11_window(RF::reference_ptr<RF::delegate> delegate, RF::window_info info);

		void handle_x11_event(const XEvent &event);

		void update_window_state(RF::window_state_t state);
		
		void handle_virtual_key_down(RF::virtual_key_t key);
		void handle_virtual_key_up(RF::virtual_key_t key);
		
		void handle_mouse_key_down(RF::mouse_key_t key);
		void handle_mouse_key_up(RF::mouse_key_t key);

		void handle_mouse_update(RF::uivec2 position);
		
		vk::ResultValue<vk::SurfaceKHR> create_surface(vk::Instance instance, const vk::AllocationCallbacks *allocator) override;

		void close() override;
		void focus() override;
		void minimise() override;

		void set_size(RF::uivec2 size) override;
	};
} // namespace RF

#endif
