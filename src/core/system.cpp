#if defined(__linux__)
#include <fstream>
#include <string>
#elif defined(__APPLE__)
#include <mach/mach.h>
#elif defined(_WIN32)
#include <windows.h>
#include <psapi.h>
#endif

#include "RF/system.hpp"

RF::sys::memory_data_t RF::sys::get_process_memory()
{
	#if defined (__linux__)
	// read: https://stackoverflow.com/questions/1558402/memory-usage-of-current-process-in-c
	// Not tested:
	struct sysinfo info;
	
	std::ifstream status_file("/proc/self/status");
	std::string line;
	std::size_t vm_size = 0;
	std::size_t vm_rss = 0;
	
	while (std::getline(status_file, line))
	{
		if (line.compare(0, 7, "VmSize:") == 0)
		{
			sscanf(line.c_str(), "VmSize: %zu kB", &vm_size);
		}
		else if (line.compare(0, 6, "VmRSS:") == 0)
		{
			sscanf(line.c_str(), "VmRSS: %zu kB", &vm_rss);
		}
	}
	
	return
	{
		RF::memory_cast<RF::gibibyte_scale>(RF::bytes(vm_size * 1024)),
		RF::memory_cast<RF::mebibyte_scale>(RF::bytes(vm_rss * 1024))
	};
	#elif defined (__APPLE__)
	struct task_basic_info info;
	mach_msg_type_number_t info_count = TASK_BASIC_INFO_COUNT;
	
	if (task_info(mach_task_self(), TASK_BASIC_INFO, reinterpret_cast<task_info_t>(&info), &info_count) != KERN_SUCCESS)
	{
		return
		{
			RF::gibibytes(0),
			RF::mebibytes(0)
		};
	}
	
	return
	{
		RF::memory_cast<gibibyte_scale>(RF::bytes(info.virtual_size)),
		RF::memory_cast<mebibyte_scale>(RF::bytes(info.resident_size))
	};
	#elif defined(_WIN32)
	// read: https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process/
	// Not tested:
	PROCESS_MEMORY_COUNTERS_EX pmc;
	if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc)))
	{
		return
		{
			RF::memory_cast<gibibyte_scale>(RF::bytes(pmc.PrivateUsage)),
			RF::memory_cast<mebibyte_scale>(RF::bytes(pmc.WorkingSetSize))
		};
	}
	return
	{
		RF::gibibytes(0),
		RF::mebibytes(0)
	};
	#endif
}

#if defined (__linux__) || defined (__APPLE__)
#include <execinfo.h> // backtrace
#include <dlfcn.h> // dladdr
#include <libgen.h> // basename
#include <cxxabi.h>
#endif

RF::sys::stack_trace_t RF::sys::get_stack_trace()
{
	RF::sys::stack_trace_t stack_entries;

#if defined (__linux__) || defined (__APPLE__)
	void* callstack[UINT8_MAX];
	int traceframes = backtrace(callstack, UINT8_MAX);

	for (int i = 0; i < traceframes; i++)
	{
		Dl_info info;
		dladdr(callstack[i], &info);

		std::string symbol_name;
		if (info.dli_sname)
		{
			int status;
			char *buffer = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
			if (status == 0 && buffer)
			{ symbol_name = buffer; }
			else
			{ symbol_name = info.dli_sname; }
			
			free(buffer);
		}

		RF::sys::stack_entry_t entry
		{
			.frame = std::int8_t(i + 2),
			.process = std::string(info.dli_fname ? basename(const_cast<char *>(info.dli_fname)) : "???"),
			.address = std::uint64_t(reinterpret_cast<std::uint64_t *>(callstack[i])),
			.callname = std::string(info.dli_sname ? symbol_name : "???")
		};
		
		stack_entries.push_back(entry);
	}
#elif defined(_WIN32)

#endif

	return stack_entries;
}

#if defined (__linux__)
std::string RF::sys::get_distro_name()
{
	// read at: https://lindevs.com/get-linux-distribution-name-using-cpp
	std::ifstream stream("/etc/os-release");
	std::string line;
	std::regex nameRegex("^NAME=\"(.*?)\"$");
	std::smatch match;

	std::string name;
	while (std::getline(stream, line))
	{
		if (std::regex_search(line, match, nameRegex))
		{
			name = match[1].str();
			break;
		}
	}
}
#endif

std::string RF::sys::get_process_name()
{
	// read: https://stackoverflow.com/questions/9097201/how-to-get-current-process-name-in-linux
	#if defined(__APPLE__) || defined(__FreeBSD__)
	return getprogname();
	#elif defined(_GNU_SOURCE)
	return program_invocation_name;
	#elif defined(_WIN32)
	return __argv[0];
	#else
	return "???";
	#endif
}

#include <chrono>
#include <ctime>

RF::sys::utc_time_t RF::sys::get_current_time()
{
	auto now = std::chrono::system_clock::now();

	std::time_t now_c = std::chrono::system_clock::to_time_t(now);

	std::tm utc_tm = *std::gmtime(&now_c);

	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

	utc_time_t current_time;
	current_time.year = std::uint16_t(utc_tm.tm_year + 1900);
	current_time.month = std::uint16_t(utc_tm.tm_mon + 1);
	current_time.day = std::uint16_t(utc_tm.tm_mday);
	current_time.hour = std::uint16_t(utc_tm.tm_hour);
	current_time.minute = std::uint16_t(utc_tm.tm_min);
	current_time.second = std::uint16_t(utc_tm.tm_sec);
	current_time.millisecond = std::uint16_t(milliseconds.count());
    
	return current_time;
}