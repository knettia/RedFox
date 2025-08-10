#pragma once

#include "RF/library.hpp"

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

#include <memory>
#include <functional>

namespace RF
{
	struct x11_dyfuncs
	{
		std::shared_ptr<RF::lib> x11_lib;
		std::shared_ptr<RF::lib> xrandr_lib;

		std::function<int(void *)> x_free;

		std::function<Display *(const char *)> x_open_display;
		std::function<int(Display *)> x_close_display;

		std::function<Window(Display *, Window, int, int, unsigned int, unsigned int, unsigned int, unsigned long, unsigned long)> x_create_simple_window;
		std::function<int(Display *, Window, const char *, const char *, Pixmap, char **, int, XSizeHints *)> x_set_standard_properties;
		std::function<XSizeHints *()> x_alloc_size_hints;

		std::function<int(Display *, Window, Atom *, int)> x_set_wm_protocols;
		std::function<void(Display *, Window, XSizeHints *)> x_set_wm_normal_hints;

		std::function<Atom(Display *, const char *, int)> x_intern_atom;
		std::function<int(Display *, Window, long)> x_select_input;
		std::function<int(Display *, Window, int, Time)> x_set_input_focus;
		std::function<int(Display *, Window, int)> x_iconify_window;
		std::function<int(Display *, Window)> x_map_window;
		std::function<int(Display *, Window)> x_destroy_window;
		std::function<int(Display *)> x_flush;
		
		std::function<int(Display *, Window, Window, int, int, unsigned int, unsigned int, int, int)> x_warp_pointer;
		std::function<int(Display *, Time)> x_ungrab_pointer;
		
		std::function<int(Display *)> x_pending;
		std::function<int(Display *, XEvent *)> x_next_event;
		std::function<int(Display *, Window, int, long, XEvent *)> x_send_event;

		std::function<RROutput(Display *, Window)> xrandr_get_output_primary;

		std::function<XRRScreenResources *(Display *, Window)> xrandr_get_screen_resources;
		std::function<void(XRRScreenResources *)> xrandr_free_screen_resources;

		std::function<XRROutputInfo *(Display *, XRRScreenResources *, RROutput)> xrandr_get_output_info;
		std::function<void(XRROutputInfo *)> xrandr_free_output_info;
		
		std::function<XRRCrtcInfo *(Display *, XRRScreenResources *, RRCrtc)> xrandr_get_crct_info;
		std::function<void(XRRCrtcInfo *)> xrandr_free_crct_info;
		std::function<int(Display *, XRRScreenResources *, RRCrtc, Time, int, int, RRMode, Rotation, RROutput *, int)> xrandr_set_crct_config;
	};

	RF::x11_dyfuncs get_x11_dyfuncs();
} // namescape RF
