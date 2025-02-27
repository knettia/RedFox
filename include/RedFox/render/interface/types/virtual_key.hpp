#pragma once

#include "RedFox/core/types/map.hpp"

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
		Backspace, PadBackspace,
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

		CapsLock, PrintScreen, ScrollLock, PauseBreak,
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

#if defined (__APPLE__)
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
		{kVK_Escape, RF::virtual_key_t::Escape},
		{kVK_Return, RF::virtual_key_t::Return},
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
		{kVK_F20, RF::virtual_key_t::F20}
	};
#elif defined (_WIN32)
	inline const RF::map<WPARAM, RF::virtual_key_t> win32_key_map
	{
		{'A', RF::virtual_key_t::A},
		{'B', RF::virtual_key_t::B},
		{'C', RF::virtual_key_t::C},
		{'D', RF::virtual_key_t::D},
		{'E', RF::virtual_key_t::E},
		{'F', RF::virtual_key_t::F},
		{'G', RF::virtual_key_t::G},
		{'H', RF::virtual_key_t::H},
		{'I', RF::virtual_key_t::I},
		{'J', RF::virtual_key_t::J},
		{'K', RF::virtual_key_t::K},
		{'L', RF::virtual_key_t::L},
		{'M', RF::virtual_key_t::M},
		{'N', RF::virtual_key_t::N},
		{'O', RF::virtual_key_t::O},
		{'P', RF::virtual_key_t::P},
		{'Q', RF::virtual_key_t::Q},
		{'R', RF::virtual_key_t::R},
		{'S', RF::virtual_key_t::S},
		{'T', RF::virtual_key_t::T},
		{'U', RF::virtual_key_t::U},
		{'V', RF::virtual_key_t::V},
		{'W', RF::virtual_key_t::W},
		{'X', RF::virtual_key_t::X},
		{'Y', RF::virtual_key_t::Y},
		{'Z', RF::virtual_key_t::Z},
		{'0', RF::virtual_key_t::Key0},
		{'1', RF::virtual_key_t::Key1},
		{'2', RF::virtual_key_t::Key2},
		{'3', RF::virtual_key_t::Key3},
		{'4', RF::virtual_key_t::Key4},
		{'5', RF::virtual_key_t::Key5},
		{'6', RF::virtual_key_t::Key6},
		{'7', RF::virtual_key_t::Key7},
		{'8', RF::virtual_key_t::Key8},
		{'9', RF::virtual_key_t::Key9},
		{VK_ESCAPE, RF::virtual_key_t::Escape},
		{VK_RETURN, RF::virtual_key_t::Return},
		{VK_F1, RF::virtual_key_t::F1},
		{VK_F2, RF::virtual_key_t::F2},
		{VK_F3, RF::virtual_key_t::F3},
		{VK_F4, RF::virtual_key_t::F4},
		{VK_F5, RF::virtual_key_t::F5},
		{VK_F6, RF::virtual_key_t::F6},
		{VK_F7, RF::virtual_key_t::F7},
		{VK_F8, RF::virtual_key_t::F8},
		{VK_F9, RF::virtual_key_t::F9},
		{VK_F10, RF::virtual_key_t::F10},
		{VK_F11, RF::virtual_key_t::F11},
		{VK_F12, RF::virtual_key_t::F12},
		{VK_F13, RF::virtual_key_t::F13},
		{VK_F14, RF::virtual_key_t::F14},
		{VK_F15, RF::virtual_key_t::F15},
		{VK_F16, RF::virtual_key_t::F16},
		{VK_F17, RF::virtual_key_t::F17},
		{VK_F18, RF::virtual_key_t::F18},
		{VK_F19, RF::virtual_key_t::F19},
		{VK_F20, RF::virtual_key_t::F20}
	};
#endif
} // namespace RF