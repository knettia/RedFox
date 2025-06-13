#include "./x11_delegate.hpp"
#include "../window/x11_window.hpp"

#include <X11/extensions/Xrandr.h>

RF::x11_delegate::~x11_delegate()
{
	this->terminate();
}

void RF::x11_delegate::terminate()
{
	XCloseDisplay(display_);
}

// ---- Video Mode ----
RF::video_mode_t RF::x11_delegate::current_video_mode()
{
	Display *display = XOpenDisplay(nullptr);
	Window root = DefaultRootWindow(display);

	XRRScreenResources *screen_resources = XRRGetScreenResources(display, root);
	if (screen_resources == nullptr)
	{
		throw RF::engine_error("X11: Failed to retrieve screen resources for current video mode.");
	}

	RROutput primary_output = XRRGetOutputPrimary(display, root);
	if (primary_output == None)
	{
		XRRFreeScreenResources(screen_resources);
		throw RF::engine_error("X11: No primary output found.");
	}

	XRROutputInfo *output_info = XRRGetOutputInfo(display, screen_resources, primary_output);
	if (output_info == nullptr || output_info->crtc == None)
	{
		XRRFreeScreenResources(screen_resources);
		if (output_info) { XRRFreeOutputInfo(output_info); }
		throw RF::engine_error("X11: No active CRTC on primary output.");
	}

	XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(display, screen_resources, output_info->crtc);
	if (crtc_info == nullptr)
	{
		XRRFreeOutputInfo(output_info);
		XRRFreeScreenResources(screen_resources);
		throw RF::engine_error("X11: Failed to retrieve CRTC info.");
	}

	int refresh_rate = 0;
	for (int i = 0; i < screen_resources->nmode; ++i)
	{
		if (screen_resources->modes[i].id == crtc_info->mode)
		{
			XRRModeInfo &mode = screen_resources->modes[i];
			if (mode.hTotal > 0 && mode.vTotal > 0)
			{
				refresh_rate = static_cast<int>((mode.dotClock) / (mode.hTotal * mode.vTotal));
			}
			break;
		}
	}

	RF::video_mode_t result
	{
		{ static_cast<std::uint32_t>(crtc_info->width), static_cast<std::uint32_t>(crtc_info->height) },
		static_cast<std::double_t>(refresh_rate)
	};

	XRRFreeCrtcInfo(crtc_info);
	XRRFreeOutputInfo(output_info);
	XRRFreeScreenResources(screen_resources);

	return result;
}

std::vector<RF::video_mode_t> RF::x11_delegate::enumerate_video_modes()
{
	Display *display = this->get_x11_display();
	Window root = DefaultRootWindow(display);

	XRRScreenResources *screen_resources = XRRGetScreenResources(display, root);
	if (screen_resources == nullptr)
	{
		return {};
	}

	std::vector<RF::video_mode_t> modes;
	modes.reserve(static_cast<std::size_t>(screen_resources->nmode));

	for (int i = 0; i < screen_resources->nmode; ++i)
	{
		XRRModeInfo &mode = screen_resources->modes[i];

		if (mode.width == 0 || mode.height == 0 || mode.hTotal == 0 || mode.vTotal == 0)
		{
			continue;
		}

		int refresh_rate = static_cast<int>((mode.dotClock) / (mode.hTotal * mode.vTotal));

		RF::video_mode_t vm
		{
			{ static_cast<std::uint32_t>(mode.width), static_cast<std::uint32_t>(mode.height) },
			static_cast<std::double_t>(refresh_rate)
		};

		modes.push_back(vm);
	}

	XRRFreeScreenResources(screen_resources);
	return modes;
}


Display *RF::x11_delegate::get_x11_display()
{
	if (!this->display_)
	{
		throw RF::engine_error("Unable to access X display.");
	}

	return this->display_;
}

RF::x11_delegate::x11_delegate(RF::delegate_info info) : display_(XOpenDisplay(nullptr)), RF::delegate(info, this->current_video_mode())
{ }

void RF::x11_delegate::register_x11_window(Window x_window, RF::x11_window *window)
{
	this->window_map_[x_window] = window;
}

void RF::x11_delegate::deregister_x11_window(Window x_window)
{
	this->window_map_.erase(x_window);
}

void RF::x11_delegate::poll_events()
{
	while (XPending(this->display_) > 0)
	{
		XEvent event;
		XNextEvent(this->display_, &event);

		this->dispatch_x11_event(event);
	}
}

void RF::x11_delegate::dispatch_x11_event(XEvent &event)
{
	auto iter = this->window_map_.find(event.xany.window);

	if (iter != this->window_map_.end())
	{
		RF::x11_window *window = iter->second;
		window->handle_x11_event(event);
	}
	else
	{
		// Optional: Log unknown window event
	}
}

char32_t RF::x11_delegate::to_keysym(RF::virtual_key_t key)
{
	return {};
}

#include "../window/x11_window.hpp"

RF::window *RF::x11_delegate::create_window(RF::window_info info)
{
	return new RF::x11_window(RF::reference_ptr<RF::delegate>(this), info);
}
