#include "x11_dyfuncs.hpp"

#include "RF/exception.hpp"
#include "RF/system.hpp"
#include "RF/library.hpp"

#include "RF/log.hpp"

RF::x11_dyfuncs RF::get_x11_dyfuncs()
{
	RF::x11_dyfuncs dyfuncs;

	auto x11_lib_opt = RF::sys::find_core_library("X11");
	auto xrandr_lib_opt = RF::sys::find_core_library("Xrandr");

	if (!x11_lib_opt.has_value())
	{
		throw RF::engine_error("Could not find X11 core library. Make sure your machine has a standard Linux system which has X11");
	}

	if (!xrandr_lib_opt.has_value())
	{
		throw RF::engine_error("Could not find Xrandr core library. Make sure your machine has a standard Linux system which has Xrandr");
	}

	dyfuncs.x11_lib = RF::library_m::load_library(x11_lib_opt.value());
	dyfuncs.xrandr_lib = RF::library_m::load_library(xrandr_lib_opt.value());

	dyfuncs.x_free = dyfuncs.x11_lib->get_function<int(void *)>("XFree");

	dyfuncs.x_open_display = dyfuncs.x11_lib->get_function<Display *(const char *)>("XOpenDisplay");
	dyfuncs.x_close_display = dyfuncs.x11_lib->get_function<int(Display *)>("XCloseDisplay");

	dyfuncs.x_create_simple_window = dyfuncs.x11_lib->get_function<Window(Display *, Window, int, int, unsigned int, unsigned int, unsigned int, unsigned long, unsigned long)>("XCreateSimpleWindow");
	dyfuncs.x_set_standard_properties = dyfuncs.x11_lib->get_function<int(Display *, Window, const char *, const char *, Pixmap, char **, int, XSizeHints *)>("XSetStandardProperties");
	dyfuncs.x_alloc_size_hints = dyfuncs.x11_lib->get_function<XSizeHints *()>("XAllocSizeHints");
	dyfuncs.x_set_wm_normal_hints = dyfuncs.x11_lib->get_function<void(Display *, Window, XSizeHints *)>("XSetWMNormalHints");
	dyfuncs.x_set_wm_protocols = dyfuncs.x11_lib->get_function<int(Display *, Window, Atom *, int)>("XSetWMProtocols");
	dyfuncs.x_intern_atom = dyfuncs.x11_lib->get_function<Atom(Display *, const char *, int)>("XInternAtom");
	dyfuncs.x_select_input = dyfuncs.x11_lib->get_function<int(Display *, Window, long)>("XSelectInput");
	dyfuncs.x_set_input_focus = dyfuncs.x11_lib->get_function<int(Display *, Window, int, Time)>("XSetInputFocus");
	dyfuncs.x_iconify_window = dyfuncs.x11_lib->get_function<int(Display *, Window, int)>("XIconifyWindow");
	dyfuncs.x_map_window = dyfuncs.x11_lib->get_function<int(Display *, Window)>("XMapWindow");
	dyfuncs.x_destroy_window = dyfuncs.x11_lib->get_function<int(Display *, Window)>("XDestroyWindow");
	dyfuncs.x_flush = dyfuncs.x11_lib->get_function<int(Display *)>("XFlush");

	dyfuncs.x_warp_pointer = dyfuncs.x11_lib->get_function<int(Display *, Window, Window, int, int, unsigned int, unsigned int, int, int)>("XWarpPointer");
	dyfuncs.x_ungrab_pointer = dyfuncs.x11_lib->get_function<int(Display *, Time)>("XUngrabPointer");

	dyfuncs.x_pending = dyfuncs.x11_lib->get_function<int(Display *)>("XPending");
	dyfuncs.x_next_event = dyfuncs.x11_lib->get_function<int(Display *, XEvent *)>("XNextEvent");
	dyfuncs.x_send_event = dyfuncs.x11_lib->get_function<int(Display *, Window, int, long, XEvent *)>("XSendEvent");

	dyfuncs.xrandr_get_output_primary = dyfuncs.xrandr_lib->get_function<RROutput(Display *, Window)>("XRRGetOutputPrimary");

	dyfuncs.xrandr_get_screen_resources = dyfuncs.xrandr_lib->get_function<XRRScreenResources *(Display *, Window)>("XRRGetScreenResources");
	dyfuncs.xrandr_free_screen_resources = dyfuncs.xrandr_lib->get_function<void(XRRScreenResources *)>("XRRFreeScreenResources");

	dyfuncs.xrandr_get_output_info = dyfuncs.xrandr_lib->get_function<XRROutputInfo *(Display *, XRRScreenResources *, RROutput)>("XRRGetOutputInfo");
	dyfuncs.xrandr_free_output_info = dyfuncs.xrandr_lib->get_function<void(XRROutputInfo *)>("XRRFreeOutputInfo");

	dyfuncs.xrandr_get_crct_info = dyfuncs.xrandr_lib->get_function<XRRCrtcInfo *(Display *, XRRScreenResources *, RRCrtc)>("XRRGetCrtcInfo");
	dyfuncs.xrandr_free_crct_info = dyfuncs.xrandr_lib->get_function<void(XRRCrtcInfo *)>("XRRFreeCrtcInfo");
	dyfuncs.xrandr_set_crct_config = dyfuncs.xrandr_lib->get_function<int(Display *, XRRScreenResources *, RRCrtc, Time, int, int, RRMode, Rotation, RROutput *, int)>("XRRSetCrtcConfig");

	return dyfuncs;
}
