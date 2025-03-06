#pragma once

namespace RF
{
	class monitor_m
	{
	public:
		monitor_m() = delete;
		~monitor_m() = delete;

		static void activate(bool);
	private:
		static void evacuate_();
		
		#if defined (__linux__) || defined (__APPLE__)
		static void handle_crash_(int signal);
		#endif
	};
} // namespace RF