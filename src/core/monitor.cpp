#include "RF/file.hpp"
#define NOMINMAX
#include "RF/monitor.hpp"
#include "RF/system.hpp"
#include "RF/string.hpp"

#include "RF/definitions.hpp"

#if defined (__linux__) || defined (__APPLE__)
#include <csignal>
#include <cstdlib>
#elif defined (_WIN32)
#include <windows.h>
#endif

std::string RF::monitor_m::save_path_;

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

void RF::monitor_m::set_save_path(std::string_view sw)
{
	RF::monitor_m::save_path_ = std::string(sw);
}

void RF::monitor_m::open_save_path_()
{
	if (RF::file_m::exists(RF::monitor_m::save_path_))
	{ RF::file_m::open_file(RF::monitor_m::save_path_); }
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

#include "RF/exception.hpp"

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
#if defined (__UNIX_LIKE__)
void RF::monitor_m::handle_crash_(int signal)
#elif defined (_WIN32)
LONG __cdecl RF::monitor_m::handle_crash_(EXCEPTION_POINTERS *exception_info)
#endif
{
	// disable ABORT signal
	#if defined (__UNIX_LIKE__)
	std::signal(SIGABRT, nullptr);
	#else // for non-UNIX
	int signal = 0;
	#endif

	if (RF::monitor_m::handle_exception_())
	{
		// exit programme
		#if defined (__UNIX_LIKE__)
		std::abort();
		#elif defined (_WIN32)
		return EXCEPTION_EXECUTE_HANDLER; 
		#endif
	}

	// generate crash log and save it
	std::ostringstream log = generate_crash_log_(signal);
	RF::file_m::write_file(RF::monitor_m::save_path_, log.str(), true);

	// show crash dialogue
	RF::monitor_m::crash_dialogue_();

	// exit programme
	#if defined (__UNIX_LIKE__)
	std::abort();
	#elif defined (_WIN32)
	return EXCEPTION_EXECUTE_HANDLER; 
	#endif
}

bool RF::monitor_m::handle_exception_()
{
	// TODO: fix rethrowing on Windows (for some reason it does not work)
	try
	{
		if (std::exception_ptr eptr = std::current_exception())
		{
			std::rethrow_exception(eptr);
		}
	}
	catch (const RF::engine_error &e)
	{
		RF::sys::stack_entry_t entry = e.entry();
		RF::monitor_m::exception_dialogue_(
			"RedFox Engine Error",
			RF::format_view("<0>\n\nAt: <1> <2>", e.what(), RF::int_to_hex(entry.address, 16), entry.callname)
		);

		return true;
	}
	catch (const RF::runtime_error &e)
	{
		RF::sys::stack_entry_t entry = e.entry();
		RF::monitor_m::exception_dialogue_(
			"RedFox Runtime Error",
			RF::format_view("<0>\n\nAt: <1> <2>", e.what(), RF::int_to_hex(entry.address, 16), entry.callname)
		);

		return true;
	}
	catch (const std::runtime_error &e)
	{
		RF::monitor_m::exception_dialogue_("STD Runtime Error", e.what());
		return true;
	}
	catch (...)
	{ /* do nothing */ }

	return false;
}

std::ostringstream RF::monitor_m::generate_crash_log_(int signal)
{
	std::ostringstream log_stream;

	// general details
	log_stream << RF::format_view("Process: <0>", RF::sys::get_process_name()) << '\n';
	log_stream << RF::format_view("System: <0>", RF::monitor_m::get_platform_name_()) << '\n';
	log_stream << RF::format_view("Distribution: <0> <1>", RF::sys::distribution_name(), RF::sys::distribution_version()) << '\n';
	log_stream << RF::format_view("Time: <0>", RF::monitor_m::get_timestamp_()) << '\n';

	log_stream << '\n';

	// CPU information
	RF::sys::cpu_info_t cpu_info = RF::sys::get_cpu_info();
	log_stream << RF::format_view("CPU : | Model: <0>", cpu_info.model) << '\n'
	           << RF::format_view("      | Clock Speed: <0>", cpu_info.clock_speed) << '\n'
	           << RF::format_view("      | Physical Cores: <0>", static_cast<std::uint16_t>(cpu_info.physical_cores)) << '\n'
	           << RF::format_view("      | Logical Cores: <0>", static_cast<std::uint16_t>(cpu_info.logical_cores)) << '\n'
	           << RF::format_view("      | Architecture: <0>", cpu_info.architecture) << '\n';

	log_stream << '\n';

	// memory usage
	RF::sys::memory_data_t process_memory = RF::sys::get_process_memory();

	#if defined (__DARWIN_ALIAS) // DARWIN virtual memory is larger, measure in GiB
	log_stream << RF::format_view("Virtual Memory Usage: <0> GiB",
	              RF::double_to_string(RF::memory_cast<RF::gibibyte_scale>(process_memory.virtual_size).count(), 2)) << '\n';
	#else
	log_stream << RF::format_view("Virtual Memory Usage: <0> MiB",
	              RF::double_to_string(RF::memory_cast<RF::mebibyte_scale>(process_memory.virtual_size).count(), 2)) << '\n';
	#endif
	log_stream << RF::format_view("Physical Memory Usage: <0> MiB",
	              RF::double_to_string(RF::memory_cast<RF::mebibyte_scale>(process_memory.physical_size).count(), 2)) << '\n';

	log_stream << '\n';

	// UNIX terminate signal
	#if defined (__UNIX_LIKE__)
	log_stream << RF::format_view("UNIX Terminate Signal: <0>", unix_signal_map.at(signal)) << '\n';
	log_stream << '\n';
	#endif

	// stack trace
	log_stream << RF::monitor_m::generate_stack_trace_().str();

	return log_stream;
}

std::ostringstream RF::monitor_m::generate_stack_trace_()
{
	std::ostringstream log_stream;

	log_stream << "Stack Trace:" << '\n';

	RF::sys::stack_trace_t stack_trace = RF::sys::get_stack_trace();

	// find column widths for formatting
	std::size_t max_frame_width = 0;
	std::size_t max_process_width = 0;
	for (const RF::sys::stack_entry_t &entry : stack_trace)
	{
		max_frame_width = std::max(max_frame_width, std::to_string(entry.frame).size());
		max_process_width = std::max(max_process_width, entry.process.size());
	}

	// append stack trace entries
	for (const RF::sys::stack_entry_t &entry : stack_trace)
	{
		log_stream << "    | "
		           << std::right << std::setw(max_frame_width) << static_cast<std::uint64_t>(entry.frame) << "  "
		           << std::left << std::setw(max_process_width) << entry.process
		           << RF::format_view(" :    <0> <1>", RF::int_to_hex(entry.address, 16), entry.callname)
		           << "\n";
	}

	return log_stream;
}

std::string RF::monitor_m::get_platform_name_()
{
#if defined (__UNIX_LIKE__)
#if defined (__LINUX__)
	return "UNIX-like (GNU/Linux)";
#elif defined (__DARWIN__)
	return "UNIX-like (BSD/Mach-O)";
#elif defined (__BSD__)
	return "UNIX-like (BSD)";
#endif
#elif defined (__WINDOWS__)	
	return "Windows";
#endif
}

std::string RF::monitor_m::get_timestamp_()
{
	RF::sys::utc_time_t time = RF::sys::get_current_time();
	return RF::format_view(
		"<0>-<1>-<2>T<3>:<4>:<5>.<6>Z",
		RF::int_to_string(time.year, 4),
		RF::int_to_string(time.month, 2),
		RF::int_to_string(time.day, 2),
		RF::int_to_string(time.hour, 2),
		RF::int_to_string(time.minute, 2),
		RF::int_to_string(time.second, 2),
		RF::int_to_string(time.millisecond, 3)
	);
}