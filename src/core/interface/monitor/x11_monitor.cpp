// redfox
#include "redfox/core/types/library.hpp"

#include "redfox/core/interface/delegate.hpp"
#include "redfox/core/interface/monitor.hpp"

// X11
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xatom.h>

// library and functions dictionary
/*
 * TODO: probably wrap these up inside RF::delegate?
 * By wrapping them into an anonymous struct if delegate is initialiyed
 * with the windowing API as RF::window_API::eX11 we can init them only
 * if needed.
 *
 * Problem to solve when we handle Wayland in wl_monitor.cpp
 * TODO: smarter RF::delegate on Linux builds, design when implementing Wayland and RF::window_API::eWayland
 */
auto x11_lib = RF::library_m::self().load_library("libX11.so");

auto func_XOpenDisplay = x11_lib->get_function<Display *(const char *)>("XOpenDisplay");
auto func_XDefaultRootWindow = x11_lib->get_function<Window(Display *)>("XDefaultRootWindow");
auto func_XInternAtom = x11_lib->get_function<Atom(Display *, const char *, int)>("XInternAtom");
auto func_XCloseDisplay = x11_lib->get_function<int(Display *)>("XCloseDisplay");
auto func_XFree = x11_lib->get_function<int(void *)>("XFree");

auto xrandr_lib = RF::library_m::self().load_library("libXrandr.so");

auto func_XRRGetOutputProperty = xrandr_lib->get_function<int(Display *, RROutput, Atom, long, long, int, int, Atom, Atom *, int *, unsigned long *, unsigned long *, unsigned char **)>("XRRGetOutputProperty");
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

std::vector<uint8_t> get_edid(Display* display, RROutput output)
{
	Atom edid_atom = func_XInternAtom(display, "EDID", True);
	if (edid_atom == None)
	{ throw std::runtime_error("EDID property not found"); }
 
	Atom actual_type;
	int actual_format;
	unsigned long nitems, bytes_after;
	unsigned char* prop_data = nullptr;
 
	func_XRRGetOutputProperty(display, output, edid_atom, 0, 128, False, False,
						 AnyPropertyType, &actual_type, &actual_format,
						 &nitems, &bytes_after, &prop_data);
 
	if (!prop_data || nitems == 0)
	{ throw std::runtime_error("Failed to retrieve EDID data"); }
 
	std::vector<uint8_t> edid(prop_data, prop_data + nitems);
	func_XFree(prop_data);
	return edid;
}

#include <cstring>

std::string parse_edid_name(const std::vector<uint8_t>& edid)
{
	if (edid.size() < 128)
	{ throw std::runtime_error("Invalid EDID size"); }
 
	for (int i = 54; i <= 108; i += 18) // Descriptor blocks at 0x36, 0x48, 0x5A, 0x6C
	{
		if (edid[i] == 0x00 && edid[i + 1] == 0x00 && edid[i + 2] == 0x00 &&
			edid[i + 3] == 0xFC) // 0xFC = Display Name Descriptor
		{
			char name[14] = {};
			std::memcpy(name, &edid[i + 5], 13);
			return std::string(name);
		}
	}
 
	throw std::runtime_error("Monitor name not found in EDID");
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
	{ 
		func_XCloseDisplay(display);
		throw std::runtime_error("Failed to get screen resources"); 
	}
 
	XRROutputInfo* output_info = func_XRRGetOutputInfo(display, screen_res, screen_res->outputs[index]);
	if (!output_info)
	{
		func_XRRFreeScreenResources(screen_res);
		func_XCloseDisplay(display);
		throw std::runtime_error("Failed to get output info");
	}
 
	XRRCrtcInfo* crtc_info = func_XRRGetCrtcInfo(display, screen_res, output_info->crtc);
	if (!crtc_info)
	{
		func_XRRFreeOutputInfo(output_info);
		func_XRRFreeScreenResources(screen_res);
		func_XCloseDisplay(display);
		throw std::runtime_error("Failed to get CRTC info");
	}
 
	// Get EDID and extract monitor name
	try
	{
		auto edid = get_edid(display, screen_res->outputs[index]);
		result.name = parse_edid_name(edid);
	}
	catch (...)
	{
		result.name = std::string(output_info->name); // Fallback if EDID fails
	}
 
	// Retrieve refresh rate properly
	XRRModeInfo* mode_info = nullptr;
	for (int i = 0; i < screen_res->nmode; i++)
	{
		if (screen_res->modes[i].id == crtc_info->mode)
		{
			mode_info = &screen_res->modes[i];
			break;
		}
	}
 
	if (!mode_info)
	{
		func_XRRFreeOutputInfo(output_info);
		func_XRRFreeCrtcInfo(crtc_info);
		func_XRRFreeScreenResources(screen_res);
		func_XCloseDisplay(display);
		throw std::runtime_error("Failed to get mode info");
	}
 
	result.refresh_rate = mode_info->dotClock / (mode_info->hTotal * mode_info->vTotal);
	result.resolution = std::move(RF::uivec2(crtc_info->width, crtc_info->height));
 
	func_XRRFreeOutputInfo(output_info);
	func_XRRFreeCrtcInfo(crtc_info);
	func_XRRFreeScreenResources(screen_res);
	func_XCloseDisplay(display);
 
	return result;
}