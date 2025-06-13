#include "./x11_window.hpp" // header
#include "../delegate/x11_delegate.hpp"

#include "RF/log.hpp"
#include <X11/extensions/Xrandr.h>

RF::x11_window::x11_window(RF::reference_ptr<RF::delegate> delegate, RF::window_info info) : RF::window(delegate, info), x11_delegate_(static_cast<RF::x11_delegate *>(delegate.ptr_get()))
{
	Display *display = this->x11_delegate_->get_x11_display();

	int screen = DefaultScreen(display);

	this->window_ = XCreateSimpleWindow(
		display,
		RootWindow(display, screen),
		0,
		0,
		info.size.x,
		info.size.y,
		1,
		BlackPixel(display, screen),
		WhitePixel(display, screen)
	);

	XSetStandardProperties(
		display,
		this->window_,
		info.title.c_str(),
		info.title.c_str(),
		None,
		nullptr,
		0,
		nullptr
	);

	// Remove ability of the user user resizing the window
	XSizeHints *size_hints = XAllocSizeHints();
	if (size_hints != nullptr)
	{
		size_hints->flags = PMinSize | PMaxSize;
		size_hints->min_width = info.size.x;
		size_hints->min_height = info.size.y;
		size_hints->max_width = info.size.x;
		size_hints->max_height = info.size.y;

		XSetWMNormalHints(display, this->window_, size_hints);
		XFree(size_hints);
	}

	// Remove ability of the user closing the window automatically
	this->wm_delete_window_ = XInternAtom(display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(display, this->window_, &wm_delete_window_, 1);

	// Set up inputs and map window
	XSelectInput(
		display,
		this->window_,
		ExposureMask
		| PointerMotionMask
		| KeyPressMask
		| KeyReleaseMask
		| StructureNotifyMask
		| FocusChangeMask
	);

	XMapWindow(display, this->window_);
	XFlush(display);

	this->x11_delegate_->register_x11_window(this->window_, this);
}

void RF::x11_window::update_window_state(RF::window_state_t new_state)
{
	if (this->state_ != new_state)
	{
		this->state_ = new_state;
		if (this->state_changed_callback_)
		{
			this->state_changed_callback_(this, new_state);
		}

		if (this->get_flag(RF::window_flag_bit_t::Fullscreen))
		{
			if (new_state == RF::window_state_t::Focused)
			{
				this->handle_window_fullscreen_();
				this->focus();
			}
			else
			{
				this->handle_window_restore_();
				this->minimise();
			}
		}
	}

}

void RF::x11_window::handle_x11_event(const XEvent &event)
{
	switch (event.type)
	{
		case (ClientMessage):
		{
			if ((Atom)event.xclient.data.l[0] == this->wm_delete_window_)
			{
				if (this->close_callback_)
				{ this->close_callback_(this); }
			}
			break;
		}

		case (MotionNotify):
		{
			// Cursor locking logic
			// TODO: Find a better way to handle this, similar to Win32,
			//       But currently, overriding the cursor position was
			//       The only reliable solution I found.
			if (this->get_flag(RF::window_flag_bit_t::CursorLocked))
			{
				this->handle_set_cursor_position_(this->mouse_position_);
			}

			std::uint32_t x = event.xmotion.x;
			std::uint32_t y = event.xmotion.y;

			this->handle_mouse_update(RF::uivec2(x, y));
			break;
		}

		case (FocusIn):
		{
			this->update_window_state(RF::window_state_t::Focused);
			break;
		}

		case (FocusOut):
		{
			// If not hidden, consider it visible
			if (this->get_state() != RF::window_state_t::Hidden)
			{
				this->update_window_state(RF::window_state_t::Visible);
			}
			break;
		}

		case (UnmapNotify):
		{
			this->update_window_state(RF::window_state_t::Hidden);
			break;
		}

		case (MapNotify):
		{
			// After being mapped again, assume visible unless focused
			if (this->get_state() != RF::window_state_t::Focused)
			{
				this->update_window_state(RF::window_state_t::Visible);
			}
			break;
		}

		case (KeyPress):
		{
			uint8_t keycode = event.xkey.keycode;

			std::optional<RF::virtual_key_t> keyopt = RF::x11_key_map[keycode];

			if (keyopt.has_value())
			{
				this->handle_virtual_key_down(keyopt.value());
			}
			break;
		}

		case (KeyRelease):
		{
			uint8_t keycode = event.xkey.keycode;

			std::optional<RF::virtual_key_t> keyopt = RF::x11_key_map[keycode];

			if (keyopt.has_value())
			{
				this->handle_virtual_key_up(keyopt.value());
			}
			break;
		}

		default: break;
	}
}

void RF::x11_window::handle_mouse_update(RF::uivec2 position)
{
	RF::ivec2 diff = RF::ivec2(position.x, position.y) - this->mouse_position_;

	if (!this->get_flag(RF::window_flag_bit_t::CursorLocked))
	{
		this->mouse_position_ = position;
	}

	if (this->mouse_move_callback_)
	{
		this->mouse_move_callback_(this, this->mouse_position_, diff);
	}
}

RF::x11_window::~x11_window()
{
	this->close();
}

void RF::x11_window::close()
{
	Display *display = this->x11_delegate_->get_x11_display();

	if (this->window_)
	{
		// if ()

		this->x11_delegate_->deregister_x11_window(this->window_);

		XDestroyWindow(display, this->window_);
		XFlush(display);

		this->window_ = 0;
	}
}

void RF::x11_window::focus()
{
	Display *display = this->x11_delegate_->get_x11_display();

	if (this->window_)
	{
		XSetInputFocus(display, this->window_, RevertToParent, CurrentTime);
		XFlush(display);
	}
}

void RF::x11_window::minimise()
{
	Display *display = this->x11_delegate_->get_x11_display();

	if (this->window_)
	{
		XIconifyWindow(display, this->window_, DefaultScreen(display));
		XFlush(display);
	}
}

// vulkan
#define VK_USE_PLATFORM_XLIB_KHR
#include <vulkan/vulkan.hpp>

vk::ResultValue<vk::SurfaceKHR> RF::x11_window::create_surface(vk::Instance instance, const vk::AllocationCallbacks *allocator)
{
	Display *display = this->x11_delegate_->get_x11_display();
	Window window = this->window_;

	if (!display || !window)
	{
		return vk::ResultValue<vk::SurfaceKHR>(vk::Result::eErrorInitializationFailed, nullptr);
	}

	// C type
	VkXlibSurfaceCreateInfoKHR sci {};
	sci.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
	sci.dpy = display;
	sci.window = window;

	auto vk_create_xlib_surface_khr = reinterpret_cast<PFN_vkCreateXlibSurfaceKHR>(instance.getProcAddr("vkCreateXlibSurfaceKHR"));

	if (!vk_create_xlib_surface_khr)
	{
		return vk::ResultValue<vk::SurfaceKHR>(vk::Result::eErrorExtensionNotPresent, nullptr);
	}

	VkSurfaceKHR raw_surface;
	vk::Result result = static_cast<vk::Result>(vk_create_xlib_surface_khr(
		static_cast<VkInstance>(instance),
		&sci,
		reinterpret_cast<const VkAllocationCallbacks *>(allocator),
		&raw_surface
	));

	if (result != vk::Result::eSuccess)
	{
		return vk::ResultValue<vk::SurfaceKHR>(result, nullptr);
	}

	return vk::ResultValue<vk::SurfaceKHR>(result, static_cast<vk::SurfaceKHR>(raw_surface));
}

void RF::x11_window::handle_window_fullscreen_()
{
	Display *display = this->x11_delegate_->get_x11_display();
	Window root = DefaultRootWindow(display);

	RF::video_mode_t mode = this->fullscreen_mode_;

	Atom wm_state = XInternAtom(display, "_NET_WM_STATE", False);
	Atom fullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

	XEvent xev {};
	xev.xclient.type = ClientMessage;
	xev.xclient.serial = 0;
	xev.xclient.send_event = True;
	xev.xclient.message_type = wm_state;
	xev.xclient.window = this->window_;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 1; // _NET_WM_STATE_ADD
	xev.xclient.data.l[1] = fullscreen;
	xev.xclient.data.l[2] = 0;
	xev.xclient.data.l[3] = 0;
	xev.xclient.data.l[4] = 0;

	XSendEvent
	(
		display,
		root,
		False,
		SubstructureNotifyMask | SubstructureRedirectMask,
		&xev
	);

	// --- Find and apply matching XRandR mode ---
	XRRScreenResources *resources = XRRGetScreenResources(display, root);
	if (resources == nullptr)
	{
		throw RF::engine_error("X11: Failed to get screen resources for fullscreen.");
	}

	RROutput primary_output = XRRGetOutputPrimary(display, root);
	XRROutputInfo *output_info = XRRGetOutputInfo(display, resources, primary_output);
	if (output_info == nullptr || output_info->crtc == None)
	{
		XRRFreeScreenResources(resources);
		if (output_info) { XRRFreeOutputInfo(output_info); }
		throw RF::engine_error("X11: No output or CRTC found for fullscreen.");
	}

	RRMode target_mode = None;

	for (int i = 0; i < output_info->nmode; ++i)
	{
		RRMode candidate_id = output_info->modes[i];

		for (int j = 0; j < resources->nmode; ++j)
		{
			XRRModeInfo &mode_info = resources->modes[j];

			if (mode_info.id == candidate_id &&
			static_cast<std::uint32_t>(mode_info.width) == mode.extent.x &&
			static_cast<std::uint32_t>(mode_info.height) == mode.extent.y)
			{
				target_mode = candidate_id;
				break;
			}
		}

		if (target_mode != None)
		{
			break;
		}
	}

	if (target_mode == None)
	{
		XRRFreeOutputInfo(output_info);
		XRRFreeScreenResources(resources);
		throw RF::engine_error("X11: No matching XRandR mode for requested fullscreen size, mode requested: <0>x<1>, desktop: <2>x<3>", mode.extent.x, mode.extent.y, this->x11_delegate_->video_mode().extent.x, this->x11_delegate_->video_mode().extent.y);
	}

	XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(display, resources, output_info->crtc);
	if (crtc_info == nullptr)
	{
		XRRFreeOutputInfo(output_info);
		XRRFreeScreenResources(resources);
		throw RF::engine_error("X11: Failed to get CRTC info.");
	}

	Status result = XRRSetCrtcConfig
	(
		display,
		resources,
		output_info->crtc,
		CurrentTime,
		crtc_info->x,
		crtc_info->y,
		target_mode,
		crtc_info->rotation,
		&primary_output,
		1
	);

	XRRFreeCrtcInfo(crtc_info);
	XRRFreeOutputInfo(output_info);
	XRRFreeScreenResources(resources);

	if (result != 0)
	{
		throw RF::engine_error("X11: Failed to apply fullscreen mode using XRandR.");
	}

	XFlush(display);
}

void RF::x11_window::handle_window_restore_()
{
	Display *display = this->x11_delegate_->get_x11_display();
	Window root = DefaultRootWindow(display);

	Atom wm_state = XInternAtom(display, "_NET_WM_STATE", False);
	Atom fullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

	XEvent xev {};
	xev.xclient.type = ClientMessage;
	xev.xclient.serial = 0;
	xev.xclient.send_event = True;
	xev.xclient.message_type = wm_state;
	xev.xclient.window = this->window_;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 0; // _NET_WM_STATE_REMOVE
	xev.xclient.data.l[1] = fullscreen;
	xev.xclient.data.l[2] = 0;
	xev.xclient.data.l[3] = 0;
	xev.xclient.data.l[4] = 0;

	XSendEvent
	(
		display,
		root,
		False,
		SubstructureNotifyMask | SubstructureRedirectMask,
		&xev
	);

	RF::video_mode_t desktop_mode = this->x11_delegate_->video_mode();

	XRRScreenResources *resources = XRRGetScreenResources(display, root);
	if (resources == nullptr)
	{
		throw RF::engine_error("X11: Failed to get screen resources for fullscreen.");
	}

	RROutput primary_output = XRRGetOutputPrimary(display, root);
	XRROutputInfo *output_info = XRRGetOutputInfo(display, resources, primary_output);
	if (output_info == nullptr || output_info->crtc == None)
	{
		XRRFreeScreenResources(resources);
		if (output_info) { XRRFreeOutputInfo(output_info); }
		throw RF::engine_error("X11: No output or CRTC found for fullscreen.");
	}

	RRMode target_mode = None;

	for (int i = 0; i < output_info->nmode; ++i)
	{
		RRMode candidate_id = output_info->modes[i];

		for (int j = 0; j < resources->nmode; ++j)
		{
			XRRModeInfo &mode_info = resources->modes[j];

			if (mode_info.id == candidate_id &&
			static_cast<std::uint32_t>(mode_info.width) == desktop_mode.extent.x &&
			static_cast<std::uint32_t>(mode_info.height) == desktop_mode.extent.y)
			{
				target_mode = candidate_id;
				break;
			}
		}

		if (target_mode != None)
		{
			break;
		}
	}

	if (target_mode == None)
	{
		XRRFreeOutputInfo(output_info);
		XRRFreeScreenResources(resources);
		throw RF::engine_error("X11: No matching XRandR mode for requested fullscreen size");
	}

	XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(display, resources, output_info->crtc);
	if (crtc_info == nullptr)
	{
		XRRFreeOutputInfo(output_info);
		XRRFreeScreenResources(resources);
		throw RF::engine_error("X11: Failed to get CRTC info.");
	}

	Status result = XRRSetCrtcConfig
	(
		display,
		resources,
		output_info->crtc,
		CurrentTime,
		crtc_info->x,
		crtc_info->y,
		target_mode,
		crtc_info->rotation,
		&primary_output,
		1
	);

	XRRFreeCrtcInfo(crtc_info);
	XRRFreeOutputInfo(output_info);
	XRRFreeScreenResources(resources);

	if (result != 0)
	{
		throw RF::engine_error("X11: Failed to apply fullscreen mode using XRandR.");
	}

	XFlush(display);
}

void RF::x11_window::handle_virtual_key_down(RF::virtual_key_t key)
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

void RF::x11_window::handle_virtual_key_up(RF::virtual_key_t key)
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

void RF::x11_window::handle_set_cursor_position_(const RF::uivec2 point)
{
	Display *display = this->x11_delegate_->get_x11_display();

	XWarpPointer(display, None, this->window_, 0, 0, 0, 0, point.x, point.y);
	XFlush(display);

	this->mouse_position_ = point;
}

void RF::x11_window::handle_flag_update_(RF::window_flag_bit_t flags, bool enabled)
{
	#if defined (None) && defined (X_H)
	#undef None
	#endif

	#define RF_case_flag_bit(flag) if ((flags & flag) != RF::window_flag_bit_t::None)

	RF_case_flag_bit(RF::window_flag_bit_t::CursorLocked)
	{
		if (!enabled)
		{
			Display *display = this->x11_delegate_->get_x11_display();

			XUngrabPointer(display, CurrentTime);
			XFlush(display);
		}
	}

	RF_case_flag_bit(RF::window_flag_bit_t::CursorHidden)
	{
		RF::logf::warn("Could not toggle flag 'CursorHidden' on X11: flag not supported");
	}

	RF_case_flag_bit(RF::window_flag_bit_t::Fullscreen)
	{
		if (enabled)
		{
			this->fullscreen_mode_ = this->find_fitting_video_mode_(this->info_.size);

			this->handle_window_fullscreen_();
		}
		else
		{
			this->handle_window_restore_();
		}
	}

	RF_case_flag_bit(RF::window_flag_bit_t::Borderless)
	{
		RF::logf::warn("Could not toggle flag 'Borderless' on X11: flag not supported");
	}

	#if !defined (None) && defined (X_H)
	#define None 0L
	#endif

	#undef RF_case_flag_bit
}

void RF::x11_window::set_size(RF::uivec2 size)
{
	// this->info_.size = size;
}

