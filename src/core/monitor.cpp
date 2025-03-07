#define NOMINMAX
#include "RF/monitor.hpp"
#include "RF/system.hpp"
#include "RF/string.hpp"
#include "RF/log.hpp"

#if defined (__linux__) || defined (__APPLE__)
#include <csignal>
#include <cstdlib>
#elif defined (_WIN32)
#include <windows.h>
#endif

void RF::monitor_m::activate(bool b)
{
#if defined (__linux__) || defined (__APPLE__) || defined (__FreeBSD__)  || defined (__OpenBSD__)
	typedef void (*unix_func)(int);
	unix_func func = b ? RF::monitor_m::handle_crash_ : nullptr;
	
	std::signal(SIGABRT, func);
	std::signal(SIGSEGV, func);
	std::signal(SIGQUIT, func);
	std::signal(SIGTERM, func);
	std::signal(SIGTSTP, func);
	std::signal(SIGPIPE, func);
	std::signal(SIGUSR1, func);
	std::signal(SIGUSR2, func);
	std::signal(SIGFPE, func);
	std::signal(SIGILL, func);
	std::signal(SIGBUS, func);
	std::signal(SIGINT, func);
	std::signal(SIGHUP, func);
#elif defined (_WIN32)
	typedef LONG (WINAPI *win32_func)(EXCEPTION_POINTERS*);
	win32_func func = b ? RF::monitor_m::handle_crash_ : nullptr;

	SetUnhandledExceptionFilter(func);
#endif
}

#include <iomanip>
#include <sstream>

#if defined (__linux__) || defined (__APPLE__) || defined (__FreeBSD__)  || defined (__OpenBSD__)
#include <unordered_map>

std::unordered_map<int, std::string> unix_signal_map
{
	{SIGABRT, "SIGABRT"},
	{SIGSEGV, "SIGSEGV"},
	{SIGQUIT, "SIGQUIT"},
	{SIGTERM, "SIGTERM"},
	{SIGTSTP, "SIGTSTP"},
	{SIGPIPE, "SIGPIPE"},
	{SIGUSR1, "SIGUSR1"},
	{SIGUSR2, "SIGUSR2"},
	{SIGFPE, "SIGFPE"},
	{SIGILL, "SIGILL"},
	{SIGBUS, "SIGBUS"},
	{SIGINT, "SIGINT"},
	{SIGHUP, "SIGHUP"}
};
#endif

// format:
/*
 * Process: process
 * Platform: platform
 * Time: YYYY-MM-DDTHH:MM:SS.MILLZ
 *
 * CPU: | Model: model
 *      | Clock Speed: num
 *      | Physical Cores: num
 *      | Logical Cores: num
 *      | Architecture: architecture
 *
 * Virtual Memory Usage: num MiB
 * Physical Memory Usage: num MiB
 * 
 * UNIX Terminate Signal: signal
 * 
 * Stack Trace:
 *     | 2  process :    0x0000000000000000 symbol
 *     | 3  process :    0x0000000000000000 symbol
 *     | 4  process :    0x0000000000000000 symbol
 */
#if defined (__linux__) || defined (__APPLE__) || defined (__FreeBSD__)  || defined (__OpenBSD__)
void RF::monitor_m::handle_crash_(int signal)
#elif defined (_WIN32)
LONG __cdecl RF::monitor_m::handle_crash_(EXCEPTION_POINTERS *exception_info)
#endif
{
	#if defined (__linux__) || defined (__APPLE__)
	std::signal(SIGABRT, nullptr);
	#endif
	
	std::ostringstream log_stream;
	
	RF::sys::memory_data_t process_memory = RF::sys::get_process_memory();
	
	log_stream << RF::format_view("Process: <0>", RF::sys::get_process_name()) << '\n';
	
	std::string platform;
	#if defined (__linux__)
	platform = RF::format_view("Linux <0>", RF::sys::get_distro_name()) ;
	#elif defined (__APPLE__)
	platform = "macOS (OS X)";
	#elif defined (_WIN32)
	platform = "Windows";
	#endif
	
	log_stream << RF::format_view("Platform: <0>", platform) << '\n';
	
	RF::sys::utc_time_t time = RF::sys::get_current_time();
	log_stream << RF::format_view("Time: <0>-<1>-<2>T<3>:<4>:<5>.<6>Z", time.year, time.month, time.day, time.hour, time.minute, time.second, time.millisecond) << '\n';
	
	log_stream << '\n';
	
	RF::sys::cpu_info_t cpu_info = RF::sys::get_cpu_info();
	
	log_stream << RF::format_view("CPU : | Model: <0>", cpu_info.model) << '\n';
	log_stream << RF::format_view("      | Clock Speed: <0>", cpu_info.clock_speed) << '\n';
	log_stream << RF::format_view("      | Physical Cores: <0>", static_cast<std::uint16_t>(cpu_info.physical_cores)) << '\n';
	log_stream << RF::format_view("      | Logical Cores: <0>", static_cast<std::uint16_t>(cpu_info.logical_cores)) << '\n';
	log_stream << RF::format_view("      | Architecture: <0>", cpu_info.architecture) << '\n';
	
	log_stream << '\n';
	
	#if defined (__APPLE__)
	log_stream << RF::format_view("Virtual Memory Usage: <0> GiB", RF::double_to_string(RF::memory_cast<RF::gibibyte_scale>(process_memory.virtual_size).count(), 2)) << '\n';
	#else
	log_stream << RF::format_view("Virtual Memory Usage: <0> MiB", RF::double_to_string(RF::memory_cast<RF::mebibyte_scale>(process_memory.virtual_size).count(), 2)) << '\n';
	#endif
	log_stream << RF::format_view("Physical Memory Usage: <0> MiB", RF::double_to_string(RF::memory_cast<RF::mebibyte_scale>(process_memory.physical_size).count(), 2)) << '\n';

	log_stream << '\n';
	
	#if defined (__linux__) || defined (__APPLE__)
	log_stream << RF::format_view("UNIX Terminate Signal: <0>", unix_signal_map.at(signal)) << '\n';

	log_stream << '\n';
	#endif

	RF::sys::stack_trace_t stack_trace = RF::sys::get_stack_trace();

	log_stream << "Stack Trace:" << '\n';

	std::size_t max_frame_width = 0;
	std::size_t max_process_width = 0;

	for (RF::sys::stack_entry_t entry : stack_trace)
	{
		max_frame_width = std::max(max_frame_width, static_cast<std::size_t>(std::to_string(entry.frame).size()));
		max_process_width = std::max(max_process_width, entry.process.size());
	}

	for (RF::sys::stack_entry_t entry : stack_trace)
	{
		log_stream
		<< "    | "
		<< std::right
		<< std::setw(max_frame_width) << static_cast<int>(entry.frame)
		<< "  "
		<< std::left
		<< std::setw(max_process_width) << entry.process
		<< std::right
		<< " :    "
		<< "0x" << std::setw(16) << std::setfill('0') << std::hex << entry.address
		<< std::dec << std::setfill(' ')
		<< " "
		<< entry.callname
		<< "\n";
	}

	// for now just print the log
	// TODO: implement saving crashlog in file
	RF::logf::error(log_stream.str());

	#if defined (__linux__) || defined (__APPLE__) || defined (__FreeBSD__)  || defined (__OpenBSD__)
	std::abort();
	#elif defined (_WIN32)
	int result = MessageBoxA(
		NULL,
		"Oh no! A crash has occurred. We are sorry for the inconvenience, for more information, check the crash log.",
		"RedFox Engine: Process Crashed",
		MB_ICONERROR | MB_YESNO | MB_DEFBUTTON2
	);
	
	if (result == IDYES)
	{
		// TODO: implement opuning the crashlog file just saved
	}
	else if (result == IDNO)
	{
		PostQuitMessage(0);
	}

	return EXCEPTION_EXECUTE_HANDLER; 
	#endif
}