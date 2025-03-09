#pragma once

#include <string>
#include <string_view>

#if defined (_WIN32)
#include <windows.h>
#endif // _WIN32

namespace RF
{
	class monitor_m
	{
	public:
		monitor_m() = delete;
		~monitor_m() = delete;

		static void activate(bool);
		static void set_save_path(std::string_view sw);
	private:
		static std::string save_path_;
		static void open_save_path_();
		static void crash_dialogue_();
		
		#if defined (__linux__) || defined (__APPLE__)
		static void handle_crash_(int signal);
		#elif defined (_WIN32)
		static LONG __cdecl handle_crash_(EXCEPTION_POINTERS *exception_info);
		#endif
	};
} // namespace RF