#include "./win32_window.hpp" // header
#include "../delegate/win32_delegate.hpp"
#include <windows.h>

#define RF_WINDOWSTYLE (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX)

// internal
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	RF::win32_window* window = reinterpret_cast<RF::win32_window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if (!window)
	{
		return 1;
	}

	switch (uMsg)
	{
		case (WM_CLOSE):
		{
			window->win32_call_close_callback();
			return 0;
		}

		case (WM_KEYDOWN):
		{
			UINT scancode = (lParam >> 16) & 0xFF;

			std::optional<RF::virtual_key_t> keyopt = RF::win32_key_map[scancode];

			if (keyopt.has_value())
			{ window->handle_virtual_key_down(keyopt.value()); }
	
			break;
		}

		case (WM_KEYUP):
		{
			UINT scancode = (lParam >> 16) & 0xFF;

			std::optional<RF::virtual_key_t> keyopt = RF::win32_key_map[scancode];
			if (keyopt.has_value())
			{ window->handle_virtual_key_up(keyopt.value()); }

			break;
		}

		case (WM_LBUTTONDOWN):
		{
			window->handle_mouse_key_down(RF::mouse_key_t::LMB);
			break;
		}

		case (WM_LBUTTONUP):
		{
			window->handle_mouse_key_up(RF::mouse_key_t::LMB);
			break;
		}

		case (WM_RBUTTONDOWN):
		{
			window->handle_mouse_key_down(RF::mouse_key_t::RMB);
			break;
		}

		case (WM_RBUTTONUP):
		{
			window->handle_mouse_key_up(RF::mouse_key_t::RMB);
			break;
		}

		case (WM_MBUTTONDOWN):
		{
			window->handle_mouse_key_down(RF::mouse_key_t::MMB);
			break;
		}

		case (WM_MBUTTONUP):
		{
			window->handle_mouse_key_up(RF::mouse_key_t::MMB);
			break;
		}

		case (WM_XBUTTONDOWN):
		{
			UINT button = GET_XBUTTON_WPARAM(wParam);
			if (button == XBUTTON1)
			{
				window->handle_mouse_key_down(RF::mouse_key_t::X1);
			}
			else if (button == XBUTTON2)
			{
				window->handle_mouse_key_down(RF::mouse_key_t::X2);
			}
			break;
		}

		case (WM_XBUTTONUP):
		{
			UINT button = GET_XBUTTON_WPARAM(wParam);
			if (button == XBUTTON1)
			{
				window->handle_mouse_key_up(RF::mouse_key_t::X1);
			}
			else if (button == XBUTTON2)
			{
				window->handle_mouse_key_up(RF::mouse_key_t::X2);
			}
			break;
		}

		case (WM_ACTIVATE):
		{
			if (LOWORD(wParam) == WA_ACTIVE)
			{
				window->update_window_state(RF::window_state_t::Focused);
			}
			else if (window->get_state() != RF::window_state_t::Hidden)
			{
				window->update_window_state(RF::window_state_t::Visible);
			}
			break;
		}

		case (WM_SIZE):
		{
			if (wParam == SIZE_MINIMIZED)
			{
				window->update_window_state(RF::window_state_t::Hidden);
			}
			else if (wParam == SIZE_RESTORED)
			{
				window->update_window_state(RF::window_state_t::Visible);
			}
			break;
		}

		case (WM_MOUSEMOVE):
		{
			std::uint32_t x = LOWORD(lParam);
			std::uint32_t y = HIWORD(lParam);

			window->handle_mouse_update(RF::uivec2(x, y));
			break;
		}

		default: { break; }
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void RF::win32_window::win32_call_close_callback()
{
	if (this->close_callback_)
	{ this->close_callback_(this); }
}

void RF::win32_window::update_window_state(RF::window_state_t new_state)
{
	if (this->state_ != new_state)
	{
		this->state_ = new_state;
		if (this->state_changed_callback_)
		{ this->state_changed_callback_(this, new_state); }

		// To not HACK our way by moving the cursor at the middle of the window,
		// Each time the window gets focused we reclip the cursor, which resets
		// Whenever we reenter the window, for some reason...
		if (new_state == RF::window_state_t::Focused)
		{
			if (this->get_flag(RF::window_flag_bit_t::CursorLocked))
			{
				this->lock_cursor_(this->mouse_position_);
			}
		}

		if (this->get_flag(RF::window_flag_bit_t::Fullscreen))
		{
			if (new_state == RF::window_state_t::Focused)
			{
				this->handle_window_fullscreen_();
				ShowWindow(this->handle_window_, SW_SHOW);
			}
			else
			{
				this->handle_window_restore_();
				ShowWindow(this->handle_window_, SW_MINIMIZE);
			}
		}
	}

}

#include "RF/exception.hpp"

// RF::win32_window implementation:
RF::win32_window::win32_window(RF::reference_ptr<RF::delegate> delegate, RF::window_info info) : RF::window(delegate, info)
{
	// create window
	this->handle_window_ = CreateWindowEx(
		0,
		"RedFox Win32 Delegate",
		this->info_.title.data(),
		RF_WINDOWSTYLE,

		0, 0, 0, 0,

		nullptr,
		nullptr,
		GetModuleHandle(nullptr),
		nullptr
	);

	if (!this->handle_window_)
	{ throw RF::engine_error("Failed to create Win32 window '<0>'", this->info_.title.data()); }

	SetWindowLongPtr(this->handle_window_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
	SetWindowLongPtr(this->handle_window_, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProc));

	// align window
	this->align_window_();

	// show window
	ShowWindow(this->handle_window_, SW_SHOW);

	// HACK: To not show loading hourglass,
	//       Change the cursor itself back to arrow
	SetCursor(LoadCursor(nullptr, IDC_ARROW));
}

RF::win32_window::~win32_window()
{ this->close(); }

void RF::win32_window::close()
{
	DestroyWindow(this->handle_window_);
}

// vulkan
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>
vk::ResultValue<vk::SurfaceKHR> RF::win32_window::create_surface(vk::Instance instance, const vk::AllocationCallbacks *allocator)
{
	HINSTANCE handle_instance { GetModuleHandle(nullptr) };
	if (!handle_instance)
	{ return vk::ResultValue<vk::SurfaceKHR>(vk::Result::eErrorInitializationFailed, nullptr); }

	// C type
	VkWin32SurfaceCreateInfoKHR sci {};
	sci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	sci.hinstance = handle_instance;
	sci.hwnd = this->handle_window_;

	auto vk_create_win32_surface_khr = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(instance.getProcAddr("vkCreateWin32SurfaceKHR"));

	if (!vk_create_win32_surface_khr)
	{ return vk::ResultValue<vk::SurfaceKHR>(vk::Result::eErrorExtensionNotPresent, nullptr); }

	VkSurfaceKHR raw_surface;
	vk::Result result = static_cast<vk::Result>(vk_create_win32_surface_khr(
		static_cast<VkInstance>(instance),
		&sci,
		reinterpret_cast<const VkAllocationCallbacks*>(allocator),
		&raw_surface
	));

	if (result != vk::Result::eSuccess)
	{ return vk::ResultValue<vk::SurfaceKHR>(result, nullptr); }

	return vk::ResultValue<vk::SurfaceKHR>(result, static_cast<vk::SurfaceKHR>(raw_surface));
}

void RF::win32_window::handle_window_fullscreen_()
{
	LONG style = GetWindowLong(this->handle_window_, GWL_STYLE);

	SetWindowLong(this->handle_window_, GWL_STYLE, style & ~RF_WINDOWSTYLE);

	DEVMODE native = static_cast<RF::win32_delegate *>(this->delegate_.ptr_get())->to_native_video_mode_win32(this->fullscreen_mode_);
        LONG result = ChangeDisplaySettings(&native, CDS_FULLSCREEN);

	SetWindowPos(this->handle_window_, NULL, 0, 0, this->info_.size.x, this->info_.size.y, SWP_NOZORDER | SWP_NOACTIVATE);

	if (result != DISP_CHANGE_SUCCESSFUL)
        {
		throw RF::engine_error("Failed to change display settings.");
        }
}

void RF::win32_window::handle_window_restore_()
{
	LONG style = GetWindowLong(this->handle_window_, GWL_STYLE);
	
	SetWindowLong(this->handle_window_, GWL_STYLE, style | RF_WINDOWSTYLE);

	DEVMODE native = static_cast<RF::win32_delegate *>(this->delegate_.ptr_get())->to_native_video_mode_win32(this->delegate_->video_mode());
        LONG result = ChangeDisplaySettings(&native, CDS_FULLSCREEN);

	if (result != DISP_CHANGE_SUCCESSFUL)
        {
		throw RF::engine_error("Failed to change display settings.");
        }
}

void RF::win32_window::handle_virtual_key_down(RF::virtual_key_t key)
{
	auto call_key_event_callback = [this](RF::virtual_key_t key, RF::key_state_t state) -> void
	{
		if (this->virtual_key_event_callback_)
		{
			this->virtual_key_event_callback_(this, key, state);
		}
	};

	RF::key_state_t &state = this->virtual_key_states_.at(key);

	if (state == RF::key_state_t::Inactive || state == RF::key_state_t::Suppressed)
	{
		state = RF::key_state_t::Triggered;
		call_key_event_callback(key, state);
	}
	if (state == RF::key_state_t::Triggered)
	{
		state = RF::key_state_t::Ongoing;
		call_key_event_callback(key, state);
	}
	else
	{ call_key_event_callback(key, state); }
}

void RF::win32_window::handle_virtual_key_up(RF::virtual_key_t key)
{
	auto call_key_event_callback = [this](RF::virtual_key_t key, RF::key_state_t state) -> void
	{
		if (this->virtual_key_event_callback_)
		{
			this->virtual_key_event_callback_(this, key, state);
		}
	};

	RF::key_state_t &state = this->virtual_key_states_.at(key);

	if (state == RF::key_state_t::Ongoing || state == RF::key_state_t::Triggered)
	{
		state = RF::key_state_t::Suppressed;
		call_key_event_callback(key, state);
		this->handle_virtual_key_up(key);
	}
	else if (state == RF::key_state_t::Suppressed)
	{
		state = RF::key_state_t::Inactive;
		call_key_event_callback(key, state);
	}
}

void RF::win32_window::handle_mouse_key_down(RF::mouse_key_t key)
{
	auto call_mouse_event_callback = [this](RF::mouse_key_t key, RF::key_state_t state) -> void
	{
		if (this->mouse_key_event_callback_)
		{ this->mouse_key_event_callback_(this, key, state); }
	};

	RF::key_state_t &state = this->mouse_key_states_.at(key);

	if (state == RF::key_state_t::Inactive || state == RF::key_state_t::Suppressed)
	{
		state = RF::key_state_t::Triggered;
		call_mouse_event_callback(key, state);
	}
	if (state == RF::key_state_t::Triggered)
	{
		state = RF::key_state_t::Ongoing;
		call_mouse_event_callback(key, state);
	}
	else
	{ call_mouse_event_callback(key, state); }
}

void RF::win32_window::handle_mouse_key_up(RF::mouse_key_t key)
{
	auto call_mouse_event_callback = [this](RF::mouse_key_t key, RF::key_state_t state) -> void
	{
		if (this->mouse_key_event_callback_)
		{ this->mouse_key_event_callback_(this, key, state); }
	};

	RF::key_state_t &state = this->mouse_key_states_.at(key);

	if (state == RF::key_state_t::Ongoing || state == RF::key_state_t::Triggered)
	{
		state = RF::key_state_t::Suppressed;
		call_mouse_event_callback(key, state);
		this->handle_mouse_key_up(key);
	}
	else if (state == RF::key_state_t::Suppressed)
	{
		state = RF::key_state_t::Inactive;
		call_mouse_event_callback(key, state);
	}
}

void RF::win32_window::handle_mouse_update(RF::uivec2 position)
{
	RF::ivec2 diff = RF::ivec2(position.x, position.y) - this->mouse_position_;
	
	if (!this->get_flag(RF::window_flag_bit_t::CursorLocked))
	{
		this->mouse_position_ = position;
	}

	if (this->mouse_move_callback_)
	{ this->mouse_move_callback_(this, this->mouse_position_, diff); }
}

void RF::win32_window::focus()
{
	SetForegroundWindow(this->handle_window_);
}

void RF::win32_window::minimise()
{
	ShowWindow(this->handle_window_, SW_MINIMIZE);
}

void RF::win32_window::lock_cursor_(const RF::uivec2 point)
{
	RECT rect;
	if (GetClientRect(this->handle_window_, &rect))
	{
		POINT top_left { rect.left, rect.top };
		ClientToScreen(this->handle_window_, &top_left);

		RECT clipRect
		{
			.left = static_cast<LONG>(point.x),
			.top = static_cast<LONG>(point.y),
			.right = static_cast<LONG>(point.x),
			.bottom = static_cast<LONG>(point.y)
		};

		ClipCursor(&clipRect);
	}
}

void RF::win32_window::align_window_()
{
	int x = (GetSystemMetrics(SM_CXSCREEN) - this->info_.size.x) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - this->info_.size.y) / 2;
	SetWindowPos(this->handle_window_, nullptr, x, y, this->info_.size.x, this->info_.size.y, SWP_NOZORDER | SWP_NOACTIVATE);
}

void RF::win32_window::handle_set_cursor_position_(const RF::uivec2 point)
{
	this->mouse_position_ = point;

	RECT rect;
	GetWindowRect(this->handle_window_, &rect);

	SetCursorPos(rect.left + point.x, rect.top + point.y);
}

void RF::win32_window::handle_flag_update_(RF::window_flag_bit_t flags, bool enabled)
{
	#define RF_case_flag_bit(flag) if ((flags & flag) != RF::window_flag_bit_t::None) \

	RF_case_flag_bit(RF::window_flag_bit_t::CursorLocked)
	{
		if (enabled)
		{
			this->lock_cursor_(this->mouse_position_);
		}
		else
		{
			ClipCursor(NULL);
		}
	}

	RF_case_flag_bit(RF::window_flag_bit_t::CursorHidden)
	{
		if (enabled)
		{
			ShowCursor(FALSE);
		}
		else
		{
			ShowCursor(TRUE);
		}
	}

	// TODO: implement proper fullscreen by changing video mode
	RF_case_flag_bit(RF::window_flag_bit_t::Fullscreen)
	{
		if (enabled)
		{
			this->fullscreen_mode_ = this->find_fitting_video_mode_(this->info_.size);

			this->handle_window_fullscreen_();
			ShowWindow(this->handle_window_, SW_SHOW);
		}
		else
		{
			this->handle_window_restore_();
			ShowWindow(this->handle_window_, SW_SHOW);
		}
	}
	
	RF_case_flag_bit(RF::window_flag_bit_t::Borderless)
	{
		LONG style = GetWindowLong(this->handle_window_, GWL_STYLE);

		if (enabled)
		{
			SetWindowLong(this->handle_window_, GWL_STYLE, style & ~RF_WINDOWSTYLE);
		}
		else
		{
			SetWindowLong(this->handle_window_, GWL_STYLE, style | RF_WINDOWSTYLE);
		}
	}

	#undef RF_case_flag_bit
}

void RF::win32_window::set_size(RF::uivec2 size)
{
	RECT rect;
	GetWindowRect(this->handle_window_, &rect);
	SetWindowPos(this->handle_window_, NULL, rect.left, rect.top, size.x, size.y, SWP_NOZORDER | SWP_NOACTIVATE);
	this->info_.size = size;
}
