#pragma once

#include "RedFox/core/types/map.hpp"

#include <string>
#if defined (__APPLE__)
#include <Carbon/Carbon.h> // for kVK macros
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
		{kVK_ANSI_0, RF::virtual_key_t::Pad0},
		{kVK_ANSI_1, RF::virtual_key_t::Pad1},
		{kVK_ANSI_2, RF::virtual_key_t::Pad2},
		{kVK_ANSI_3, RF::virtual_key_t::Pad3},
		{kVK_ANSI_4, RF::virtual_key_t::Pad4},
		{kVK_ANSI_5, RF::virtual_key_t::Pad5},
		{kVK_ANSI_6, RF::virtual_key_t::Pad6},
		{kVK_ANSI_7, RF::virtual_key_t::Pad7},
		{kVK_ANSI_8, RF::virtual_key_t::Pad8},
		{kVK_ANSI_9, RF::virtual_key_t::Pad9},
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
		{kVK_F12, RF::virtual_key_t::F12}
	};
#endif
} // namespace RF