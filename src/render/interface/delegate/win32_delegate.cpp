#include "RedFox/render/interface/delegate/win32_delegate.hpp"
#include "RedFox/render/interface/types/virtual_key.hpp"
#include <minwindef.h>

// RF::win32_delegate implementation:
RF::win32_delegate::~win32_delegate()
{ this->terminate(); }

void RF::win32_delegate::terminate()
{
	UnregisterClass(this->window_class_.lpszClassName, this->window_class_.hInstance);
}

RF::win32_delegate::win32_delegate(RF::delegate_info info) : RF::delegate(info)
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
	std::optional<WPARAM> codeopt = RF::win32_key_map[key];
	if (!codeopt.has_value())
	{ return char32_t { U'\uFFFD' }; }

	WPARAM code = codeopt.value();

	UINT scanCode = MapVirtualKey(code, MAPVK_VK_TO_VSC);

	WCHAR unicodeChar[2] {0};

	BYTE keyboardState[256];
	if (!GetKeyboardState(keyboardState))
	{
		return U'\uFFFD';
	}

	int result = ToUnicode(code, scanCode, keyboardState, unicodeChar, 2, 0);

	if (result == 1)
	{
		return static_cast<char32_t>(unicodeChar[0]);
	}
	else
	{
		return U'\uFFFD';
	}
}

#include "RedFox/render/interface/window/win32_window.hpp"
RF::window *RF::win32_delegate::create_window(RF::window_info info)
{ return new RF::win32_window(info); }
