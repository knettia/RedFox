#include "./win32_delegate.hpp"
#include "RF/exception.hpp"
#include "RF/interface/virtual_key.hpp"

// RF::win32_delegate implementation:
RF::win32_delegate::~win32_delegate()
{ this->terminate(); }

void RF::win32_delegate::terminate()
{
	UnregisterClass(this->window_class_.lpszClassName, this->window_class_.hInstance);
}

// ---- Video Mode ----
RF::video_mode_t RF::win32_delegate::current_video_mode()
{
	DEVMODE native;

	if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &native))
	{
		RF::video_mode_t mode
		{
			{native.dmPelsWidth, native.dmPelsHeight},
			static_cast<double>(native.dmDisplayFrequency)
		};

		return mode;
	}

	throw RF::engine_error("Could not retrieve current video mode.");
}

std::vector<RF::video_mode_t> RF::win32_delegate::enumerate_video_modes()
{
	std::vector<RF::video_mode_t> modes;
	DEVMODE native;
	native.dmSize = sizeof(DEVMODE);

	int i = 0;
	while (EnumDisplaySettings(NULL, i++, &native))
	{
		RF::video_mode_t mode
		{
			{native.dmPelsWidth, native.dmPelsHeight},
			static_cast<double>(native.dmDisplayFrequency)
		};

		modes.push_back(mode);
	}

	return modes;
}

DEVMODE RF::win32_delegate::to_native_video_mode_win32(RF::video_mode_t video_mode)
{
	DEVMODE native;
	native.dmSize = sizeof(DEVMODE);

	int i = 0;
	while (EnumDisplaySettings(NULL, i++, &native))
	{
		RF::video_mode_t mode
		{
			{native.dmPelsWidth, native.dmPelsHeight},
			static_cast<double>(native.dmDisplayFrequency)
		};

		
		bool matches_resolution = (video_mode.extent.x == native.dmPelsWidth
		                          && video_mode.extent.y == native.dmPelsHeight);
		
		bool matches_refresh = (video_mode.refresh_rate == native.dmDisplayFrequency);

		if (matches_resolution && matches_refresh)
		{
			return native;
			break;
		}
	}

	throw RF::engine_error
	(
		"Internal inconsistency: no DEVMODE found matching RF::video_mode_t with extent '<0>x<1>' and refresh rate '<2>'",
		video_mode.extent.x, video_mode.extent.y,
		video_mode.refresh_rate
	);
}

RF::win32_delegate::win32_delegate(RF::delegate_info info) : RF::delegate(info, this->current_video_mode())
{
	this->window_class_ = {
		.lpfnWndProc = DefWindowProc,
		.hInstance = GetModuleHandle(nullptr),
		.lpszClassName = "RedFox Win32 Delegate"
	};
	RegisterClass(&this->window_class_);
}

void RF::win32_delegate::poll_events()
{
	MSG msg;

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

char32_t RF::win32_delegate::to_keysym(RF::virtual_key_t key)
{
	std::optional<UINT> codeopt = RF::win32_key_map[key];
	if (!codeopt.has_value())
	{ return char32_t { U'\uFFFD' }; }

	UINT code = codeopt.value();
	UINT vk_code = MapVirtualKey(code, MAPVK_VSC_TO_VK);

	WCHAR unicode_char[2] {0};

	BYTE keyboard_state[256];
	if (!GetKeyboardState(keyboard_state))
	{ return U'\uFFFD'; }

	int result = ToUnicode(vk_code, code, keyboard_state, unicode_char, 2, 0);

	if (result == 1)
	{ return static_cast<char32_t>(unicode_char[0]); }
	else
	{ return U'\uFFFD'; }
}

#include "../window/win32_window.hpp"
RF::window *RF::win32_delegate::create_window(RF::window_info info)
{ return new RF::win32_window(RF::reference_ptr<RF::delegate>(this), info); }
