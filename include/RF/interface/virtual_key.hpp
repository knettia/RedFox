#pragma once

#include "RF/definitions.hpp"
#include "RF/map.hpp"

#include <string>
#if defined (__APPLE__)
#include <Carbon/Carbon.h> // for kVK macros
#elif defined (_WIN32)
#include <windows.h> // for kVK macros
#endif

namespace RF
{
	enum class virtual_key_t
	{
		Unknown = 0,
		A, B, C, D, E, F, G, H, I, J, K, L, M,
		N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

		Key0, Key1, Key2, Key3, Key4, Key5, Key6, Key7, Key8, Key9,
		Pad0, Pad1, Pad2, Pad3, Pad4, Pad5, Pad6, Pad7, Pad8, Pad9,
		
		Equal, Plus,
		Minus, PadMinus,

		Escape,
		Return, PadReturn,
		Backspace,
		Tab,
		Space,

		LShift, RShift,
		LControl, RControl,
		LSuper, RSuper,
		Alt, AltGr,

		F1, F2, F3, F4, F5,
		F6, F7, F8, F9, F10,
		F11, F12, F13, F14, F15,
		F16, F17, F18, F19, F20,

		Home, End,
		Insert, Delete,
		PageUp, PageDown,

		ArrowUp, ArrowDown,
		ArrowLeft, ArrowRight,

		CapsLock,
		_Count
	};

	enum class mouse_key_t
	{
		LMB, RMB, MMB,
		X1, X2, X3, X4, X5,
		_Count
	};

	enum class key_state_t
	{
		Inactive,
		Triggered,
		Ongoing,
		Suppressed
	};

	inline std::string to_string(RF::key_state_t t)
	{
		switch (t)
		{
			case RF::key_state_t::Inactive:
			{ return std::string("Inactive"); }

			case RF::key_state_t::Triggered:
			{ return std::string("Triggered"); }
			
			case RF::key_state_t::Ongoing:
			{ return std::string("Ongoing"); }

			case RF::key_state_t::Suppressed:
			{ return std::string("Suppressed"); }
		}
	}

#if defined (__LINUX__)
	inline const RF::map<uint8_t, RF::virtual_key_t> x11_key_map
	{
		{ 38, RF::virtual_key_t::A },
		{ 56, RF::virtual_key_t::B },
		{ 54, RF::virtual_key_t::C },
		{ 54, RF::virtual_key_t::C },
		{ 40, RF::virtual_key_t::D },
		{ 26, RF::virtual_key_t::E },
		{ 41, RF::virtual_key_t::F },
		{ 42, RF::virtual_key_t::G },
		{ 43, RF::virtual_key_t::H },
		{ 44, RF::virtual_key_t::J },
		{ 45, RF::virtual_key_t::K },
		{ 46, RF::virtual_key_t::L },
		{ 58, RF::virtual_key_t::M },
		{ 57, RF::virtual_key_t::N },
		{ 32, RF::virtual_key_t::O },
		{ 33, RF::virtual_key_t::P },
		{ 24, RF::virtual_key_t::Q },
		{ 27, RF::virtual_key_t::R },
		{ 39, RF::virtual_key_t::S },
		{ 28, RF::virtual_key_t::T },
		{ 30, RF::virtual_key_t::U },
		{ 55, RF::virtual_key_t::V },
		{ 25, RF::virtual_key_t::W },
		{ 53, RF::virtual_key_t::X },
		{ 29, RF::virtual_key_t::Y },
		{ 52, RF::virtual_key_t::Z },
		{ 19, RF::virtual_key_t::Key0 },
		{ 10, RF::virtual_key_t::Key1 },
		{ 11, RF::virtual_key_t::Key2 },
		{ 12, RF::virtual_key_t::Key3 },
		{ 13, RF::virtual_key_t::Key4 },
		{ 14, RF::virtual_key_t::Key5 },
		{ 15, RF::virtual_key_t::Key6 },
		{ 16, RF::virtual_key_t::Key7 },
		{ 17, RF::virtual_key_t::Key8 },
		{ 18, RF::virtual_key_t::Key9 },
		{ 90, RF::virtual_key_t::Pad0 },
		{ 87, RF::virtual_key_t::Pad1 },
		{ 88, RF::virtual_key_t::Pad2 },
		{ 89, RF::virtual_key_t::Pad3 },
		{ 83, RF::virtual_key_t::Pad4 },
		{ 84, RF::virtual_key_t::Pad5 },
		{ 85, RF::virtual_key_t::Pad6 },
		{ 79, RF::virtual_key_t::Pad7 },
		{ 80, RF::virtual_key_t::Pad8 },
		{ 81, RF::virtual_key_t::Pad9 },
		{ 9, RF::virtual_key_t::Escape },
		{ 36, RF::virtual_key_t::Return },
		{ 104, RF::virtual_key_t::PadReturn },
		{ 22, RF::virtual_key_t::Backspace },
		{ 23, RF::virtual_key_t::Tab },
		{ 65, RF::virtual_key_t::Space },
		{ 50, RF::virtual_key_t::LShift },
		{ 62, RF::virtual_key_t::RShift },
		{ 37, RF::virtual_key_t::LControl },
		{ 105, RF::virtual_key_t::RControl },
		{ 133, RF::virtual_key_t::LSuper },
		{ 134, RF::virtual_key_t::RSuper },
		{ 64, RF::virtual_key_t::Alt },
		{ 108, RF::virtual_key_t::AltGr },
		{ 67, RF::virtual_key_t::F1 },
		{ 68, RF::virtual_key_t::F2 },
		{ 69, RF::virtual_key_t::F3 },
		{ 70, RF::virtual_key_t::F4 },
		{ 71, RF::virtual_key_t::F5 },
		{ 72, RF::virtual_key_t::F6 },
		{ 73, RF::virtual_key_t::F7 },
		{ 74, RF::virtual_key_t::F8 },
		{ 75, RF::virtual_key_t::F9 },
		{ 76, RF::virtual_key_t::F10 },
		{ 95, RF::virtual_key_t::F11 },
		{ 96, RF::virtual_key_t::F12 },
		// TODO: support up to F20
		{ 110, RF::virtual_key_t::Home },
		{ 115, RF::virtual_key_t::End },
		{ 118, RF::virtual_key_t::Insert },
		{ 119, RF::virtual_key_t::Delete },
		{ 112, RF::virtual_key_t::PageUp },
		{ 117, RF::virtual_key_t::PageDown },
		{ 111, RF::virtual_key_t::ArrowUp },
		{ 116, RF::virtual_key_t::ArrowDown },
		{ 113, RF::virtual_key_t::ArrowLeft },
		{ 114, RF::virtual_key_t::ArrowRight },
	};
#elif defined (__APPLE__)
	inline const RF::map<CGKeyCode, RF::virtual_key_t> cocoa_key_map
	{
		{kVK_ANSI_A, RF::virtual_key_t::A},
		{kVK_ANSI_B, RF::virtual_key_t::B},
		{kVK_ANSI_C, RF::virtual_key_t::C},
		{kVK_ANSI_D, RF::virtual_key_t::D},
		{kVK_ANSI_E, RF::virtual_key_t::E},
		{kVK_ANSI_F, RF::virtual_key_t::F},
		{kVK_ANSI_G, RF::virtual_key_t::G},
		{kVK_ANSI_H, RF::virtual_key_t::H},
		{kVK_ANSI_I, RF::virtual_key_t::I},
		{kVK_ANSI_J, RF::virtual_key_t::J},
		{kVK_ANSI_K, RF::virtual_key_t::K},
		{kVK_ANSI_L, RF::virtual_key_t::L},
		{kVK_ANSI_M, RF::virtual_key_t::M},
		{kVK_ANSI_N, RF::virtual_key_t::N},
		{kVK_ANSI_O, RF::virtual_key_t::O},
		{kVK_ANSI_P, RF::virtual_key_t::P},
		{kVK_ANSI_Q, RF::virtual_key_t::Q},
		{kVK_ANSI_R, RF::virtual_key_t::R},
		{kVK_ANSI_S, RF::virtual_key_t::S},
		{kVK_ANSI_T, RF::virtual_key_t::T},
		{kVK_ANSI_U, RF::virtual_key_t::U},
		{kVK_ANSI_V, RF::virtual_key_t::V},
		{kVK_ANSI_W, RF::virtual_key_t::W},
		{kVK_ANSI_X, RF::virtual_key_t::X},
		{kVK_ANSI_Y, RF::virtual_key_t::Y},
		{kVK_ANSI_Z, RF::virtual_key_t::Z},
		{kVK_ANSI_0, RF::virtual_key_t::Key0},
		{kVK_ANSI_1, RF::virtual_key_t::Key1},
		{kVK_ANSI_2, RF::virtual_key_t::Key2},
		{kVK_ANSI_3, RF::virtual_key_t::Key3},
		{kVK_ANSI_4, RF::virtual_key_t::Key4},
		{kVK_ANSI_5, RF::virtual_key_t::Key5},
		{kVK_ANSI_6, RF::virtual_key_t::Key6},
		{kVK_ANSI_7, RF::virtual_key_t::Key7},
		{kVK_ANSI_8, RF::virtual_key_t::Key8},
		{kVK_ANSI_9, RF::virtual_key_t::Key9},
		{kVK_ANSI_Keypad0, RF::virtual_key_t::Pad0},
		{kVK_ANSI_Keypad1, RF::virtual_key_t::Pad1},
		{kVK_ANSI_Keypad2, RF::virtual_key_t::Pad2},
		{kVK_ANSI_Keypad3, RF::virtual_key_t::Pad3},
		{kVK_ANSI_Keypad4, RF::virtual_key_t::Pad4},
		{kVK_ANSI_Keypad5, RF::virtual_key_t::Pad5},
		{kVK_ANSI_Keypad6, RF::virtual_key_t::Pad6},
		{kVK_ANSI_Keypad7, RF::virtual_key_t::Pad7},
		{kVK_ANSI_Keypad8, RF::virtual_key_t::Pad8},
		{kVK_ANSI_Keypad9, RF::virtual_key_t::Pad9},
		{kVK_Escape, RF::virtual_key_t::Escape},
		{kVK_Return, RF::virtual_key_t::Return},
		{kVK_ANSI_KeypadEnter, RF::virtual_key_t::PadReturn},
		{kVK_Delete, RF::virtual_key_t::Backspace},
		{kVK_Tab, RF::virtual_key_t::Tab},
		{kVK_Space, RF::virtual_key_t::Space},
		{kVK_Shift, RF::virtual_key_t::LShift},
		{kVK_RightShift, RF::virtual_key_t::LShift},
		{kVK_Control, RF::virtual_key_t::LControl},
		{kVK_RightControl, RF::virtual_key_t::RControl},
		{kVK_Command, RF::virtual_key_t::LSuper},
		{kVK_RightCommand, RF::virtual_key_t::RSuper},
		{kVK_Option, RF::virtual_key_t::Alt},
		{kVK_RightOption, RF::virtual_key_t::AltGr},
		{kVK_F1, RF::virtual_key_t::F1},
		{kVK_F2, RF::virtual_key_t::F2},
		{kVK_F3, RF::virtual_key_t::F3},
		{kVK_F4, RF::virtual_key_t::F4},
		{kVK_F5, RF::virtual_key_t::F5},
		{kVK_F6, RF::virtual_key_t::F6},
		{kVK_F7, RF::virtual_key_t::F7},
		{kVK_F8, RF::virtual_key_t::F8},
		{kVK_F9, RF::virtual_key_t::F9},
		{kVK_F10, RF::virtual_key_t::F10},
		{kVK_F11, RF::virtual_key_t::F11},
		{kVK_F12, RF::virtual_key_t::F12},
		{kVK_F13, RF::virtual_key_t::F13},
		{kVK_F14, RF::virtual_key_t::F14},
		{kVK_F15, RF::virtual_key_t::F15},
		{kVK_F16, RF::virtual_key_t::F16},
		{kVK_F17, RF::virtual_key_t::F17},
		{kVK_F18, RF::virtual_key_t::F18},
		{kVK_F19, RF::virtual_key_t::F19},
		{kVK_F20, RF::virtual_key_t::F20},
		{kVK_Home, RF::virtual_key_t::Home},
		{kVK_End, RF::virtual_key_t::End},
		{kVK_Help, RF::virtual_key_t::Insert},
		{kVK_ForwardDelete, RF::virtual_key_t::Delete},
		{kVK_PageUp, RF::virtual_key_t::PageUp},
		{kVK_PageDown, RF::virtual_key_t::PageDown},
		{kVK_UpArrow, RF::virtual_key_t::ArrowUp},
		{kVK_DownArrow, RF::virtual_key_t::ArrowDown},
		{kVK_LeftArrow, RF::virtual_key_t::ArrowLeft},
		{kVK_RightArrow, RF::virtual_key_t::ArrowRight},
	};
#elif defined (_WIN32)
	// https://docs.google.com/spreadsheets/d/1GSj0gKDxyWAecB3SIyEZ2ssPETZkkxn67gdIwL1zFUs/edit?gid=0#gid=0
	inline const RF::map<UINT, RF::virtual_key_t> win32_key_map
	{
		{0x01E, RF::virtual_key_t::A},
		{0x030, RF::virtual_key_t::B},
		{0x02E, RF::virtual_key_t::C},
		{0x020, RF::virtual_key_t::D},
		{0x012, RF::virtual_key_t::E},
		{0x021, RF::virtual_key_t::F},
		{0x022, RF::virtual_key_t::G},
		{0x023, RF::virtual_key_t::H},
		{0x017, RF::virtual_key_t::I},
		{0x024, RF::virtual_key_t::J},
		{0x025, RF::virtual_key_t::K},
		{0x026, RF::virtual_key_t::L},
		{0x032, RF::virtual_key_t::M},
		{0x031, RF::virtual_key_t::N},
		{0x018, RF::virtual_key_t::O},
		{0x019, RF::virtual_key_t::P},
		{0x010, RF::virtual_key_t::Q},
		{0x013, RF::virtual_key_t::R},
		{0x01F, RF::virtual_key_t::S},
		{0x014, RF::virtual_key_t::T},
		{0x016, RF::virtual_key_t::U},
		{0x02F, RF::virtual_key_t::V},
		{0x011, RF::virtual_key_t::W},
		{0x02D, RF::virtual_key_t::X},
		{0x015, RF::virtual_key_t::Y},
		{0x02C, RF::virtual_key_t::Z},
		{0x00B, RF::virtual_key_t::Key0},
		{0x002, RF::virtual_key_t::Key1},
		{0x003, RF::virtual_key_t::Key2},
		{0x004, RF::virtual_key_t::Key3},
		{0x005, RF::virtual_key_t::Key4},
		{0x006, RF::virtual_key_t::Key5},
		{0x007, RF::virtual_key_t::Key6},
		{0x008, RF::virtual_key_t::Key7},
		{0x009, RF::virtual_key_t::Key8},
		{0x00A, RF::virtual_key_t::Key9},
		{0x001, RF::virtual_key_t::Escape},
		{0x01C, RF::virtual_key_t::Return}
	};
#endif
} // namespace RF
