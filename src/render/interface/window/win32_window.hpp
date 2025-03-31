#if defined (_WIN32)
#pragma once
// RedFox
#include "RF/interface/window.hpp"

// Windows
#include <windows.h>

namespace RF
{
	class win32_window : public RF::window
	{
	private:
		HWND handle_window_;

		void handle_window_restore_() override;
		void handle_window_fullscreen_() override;

		void handle_flag_update_(RF::window_flag_bit_t flag, bool enabled) override;

		// Locks the cursor to the middle position of the window 
		void lock_cursor_();

		// Aligns the window in the centre of the screes
		void align_window_();
	public:
		~win32_window() override;
		win32_window(RF::reference_ptr<RF::delegate> delegate, RF::window_info info);
		
		void win32_call_close_callback(); // HACK: find better way to fix
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
#endif // _WIN32