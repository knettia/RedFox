// redfox
#include "redfox/core/types/library.hpp"

#include "redfox/core/interface/delegate.hpp"
#include "redfox/core/interface/monitor.hpp"

// X11
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

auto x11_lib = RF::library_m::self().load_library("libX11.so");
auto xrandr_lib = RF::library_m::self().load_library("libXrandr.so");

auto func_XOpenDisplay = x11_lib->get_function<Display *(const char *)>("XOpenDisplay");
auto func_XDefaultRootWindow = x11_lib->get_function<Window(Display *)>("XDefaultRootWindow");
auto func_XCloseDisplay = x11_lib->get_function<int(Display *)>("XCloseDisplay");

auto func_XRRGetScreenResourcesCurrent = xrandr_lib->get_function<XRRScreenResources *(Display *, Window)>("XRRGetScreenResourcesCurrent");
auto func_XRRGetOutputInfo = xrandr_lib->get_function<XRROutputInfo *(Display *, XRRScreenResources *, RROutput)>("XRRGetOutputInfo");
auto func_XRRGetCrtcInfo = xrandr_lib->get_function<XRRCrtcInfo *(Display *, XRRScreenResources *, RRCrtc)>("XRRGetCrtcInfo");
auto func_XRRFreeCrtcInfo = xrandr_lib->get_function<void(XRRCrtcInfo *)>("XRRFreeCrtcInfo");
auto func_XRRFreeOutputInfo = xrandr_lib->get_function<void(XRROutputInfo *)>("XRRFreeOutputInfo");
auto func_XRRFreeScreenResources = xrandr_lib->get_function<void(XRRScreenResources *)>("XRRFreeScreenResources");

int RF::delegate::monitor_count()
{
	Display* display = func_XOpenDisplay(NULL);
	if (!display)
	{ throw std::runtime_error("Failed to open X display"); }
	
	Window root = func_XDefaultRootWindow(display);
	
	XRRScreenResources* screen_res = func_XRRGetScreenResourcesCurrent(display, root);
	if (!screen_res)
	{ throw std::runtime_error("Failed to get screen resources"); }

	return screen_res->noutput;

	func_XRRFreeScreenResources(screen_res);
	func_XCloseDisplay(display);
}

RF::monitor_data RF::delegate::monitor_data(int index)
{
	RF::monitor_data result; 

	Display* display = func_XOpenDisplay(NULL);
	if (!display)
	{ throw std::runtime_error("Failed to open X display"); }
	
	Window root = func_XDefaultRootWindow(display);

	XRRScreenResources* screen_res = func_XRRGetScreenResourcesCurrent(display, root);
	if (!screen_res)
	{ throw std::runtime_error("Failed to get screen resources"); }

	XRROutputInfo* output_info = func_XRRGetOutputInfo(display, screen_res, screen_res->outputs[index]);
	XRRCrtcInfo *crtc_info = func_XRRGetCrtcInfo(display, screen_res, output_info->crtc);

	XRRModeInfo mode_info = screen_res->modes[index];

	result.name = std::move(std::string(output_info->name)); // FIXME: display actual name of monitor instead of "HDMI-1" or "eDP-1"
	result.refresh_rate = static_cast<double>(mode_info.dotClock) / (static_cast<double>(mode_info.hTotal) * mode_info.vTotal); // TODO: properly return refresh rate instead of calculating active rate
	result.resolution = std::move(RF::uivec2(crtc_info->width, crtc_info->height));

	func_XRRFreeOutputInfo(output_info);
	func_XRRFreeCrtcInfo(crtc_info);
	func_XRRFreeScreenResources(screen_res);
	func_XCloseDisplay(display);

	return result;
}