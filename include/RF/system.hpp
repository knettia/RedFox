#pragma once

// std
#include <string>

#include "RF/memory.hpp"
#include "RF/mass.hpp"

namespace RF::sys
{
	struct memory_data_t
	{
		RF::gibibytes virtual_size;
		RF::mebibytes physical_size;
	};

	struct stack_entry_t
	{
		std::int8_t   frame;
		std::uint64_t address;
		std::string   process;
		std::string   callname;
	};

	using stack_trace_t = RF::mass<stack_entry_t, UINT8_MAX>;

	RF::sys::memory_data_t get_process_memory();
	RF::sys::memory_data_t get_system_memory();

	RF::sys::stack_trace_t get_stack_trace();

	#if defined (__linux__)
	std::string get_distro_name();
	#endif
	std::string get_process_name();

	struct utc_time_t
	{
		std::uint16_t year;
		std::uint16_t month;
		std::uint16_t day;
		std::uint16_t hour;
		std::uint16_t minute;
		std::uint16_t second;
		std::uint16_t millisecond;
	};

	RF::sys::utc_time_t get_current_time();
}