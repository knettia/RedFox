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
		RF::bytes(vm_size * 1024),
		RF::bytes(vm_rss * 1024)
	};
	#elif defined (__APPLE__)
	struct task_basic_info info;
	mach_msg_type_number_t info_count = TASK_BASIC_INFO_COUNT;
	
	if (task_info(mach_task_self(), TASK_BASIC_INFO, reinterpret_cast<task_info_t>(&info), &info_count) != KERN_SUCCESS)
	{
		return
		{
			RF::bytes(0),
			RF::bytes(0)
		};
	}
	
	return
	{
		RF::bytes(info.virtual_size),
		RF::bytes(info.resident_size)
	};
	#elif defined(_WIN32)
	// read: https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process/
	// Not tested:
	PROCESS_MEMORY_COUNTERS_EX pmc;
	if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc), sizeof(pmc)))
	{
		return
		{
			RF::bytes(pmc.PrivateUsage),
			RF::bytes(pmc.WorkingSetSize)
		};
	}
	return
	{
		RF::bytes(0),
		RF::bytes(0)
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
	char **symbols = backtrace_symbols(callstack, traceframes);

	dlopen(nullptr, RTLD_NOW | RTLD_GLOBAL);

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
			.address = std::uint64_t(reinterpret_cast<std::uint64_t *>(callstack[i])),
			.process = std::string(info.dli_fname ? basename(const_cast<char *>(info.dli_fname)) : "???"),
			.callname = std::string(info.dli_sname ? symbol_name : "???")
		};
		
		stack_entries.push_back(entry);
	}
#elif defined(_WIN32)

#endif

	return stack_entries;
}

#include <fstream>
#include <regex>

#if defined (__linux__)
std::string RF::sys::get_distro_name()
{
	// read at: https://lindevs.com/get-linux-distribution-name-using-cpp
	std::ifstream stream("/etc/os-release");
	std::string line;
	std::regex name_regex("^NAME=\"(.*?)\"$");
	std::smatch match;

	std::string name;
	while (std::getline(stream, line))
	{
		if (std::regex_search(line, match, name_regex))
		{
			name = match[1].str();
			break;
		}
	}

	return name;
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

#if defined (__APPLE__)
#include <sys/sysctl.h>
#endif

RF::sys::cpu_info_t RF::sys::get_cpu_info()
{
	RF::sys::cpu_info_t info;
#if defined (__linux__)
	// read: https://www.geeksforgeeks.org/check-linux-cpu-information/?utm_source=chatgpt.com
	// Not tested:
	std::ifstream cpuinfo_file("/proc/cpuinfo");
	std::string line;
	std::unordered_map<std::string, std::string> cpuinfo_map;

	while (std::getline(cpuinfo_file, line))
	{
		std::istringstream iss(line);
		std::string key;
		if (std::getline(iss, key, ':'))
		{
			std::string value;
			if (std::getline(iss, value))
			{ cpuinfo_map[key] = value; }
		}
	}

	info.model_name = cpuinfo_map["model name"];
	info.clock_speed = std::stod(cpuinfo_map["cpu MHz"]);
	info.architecture = cpuinfo_map["architecture"];
	info.logical_cores = std::stoi(cpuinfo_map["siblings"]);
	info.physical_cores = std::stoi(cpuinfo_map["cpu cores"]);
#elif defined (__APPLE__)
	char buffer[128];
	size_t buffer_size = sizeof(buffer);

	sysctlbyname("machdep.cpu.brand_string", &buffer, &buffer_size, nullptr, 0);
	info.model = buffer;

	int physical_cores;
	buffer_size = sizeof(physical_cores);
	sysctlbyname("hw.physicalcpu", &physical_cores, &buffer_size, nullptr, 0);
	info.physical_cores = physical_cores;

	int logical_cores;
	buffer_size = sizeof(logical_cores);
	sysctlbyname("hw.logicalcpu", &logical_cores, &buffer_size, nullptr, 0);
	info.logical_cores = logical_cores;

	uint64_t frequency;
	buffer_size = sizeof(frequency);
	sysctlbyname("hw.cpufrequency", &frequency, &buffer_size, nullptr, 0);
	info.clock_speed = frequency / 1'000'000; // Convert Hz to MHz

	buffer_size = sizeof(buffer);
	sysctlbyname("hw.machine", &buffer, &buffer_size, nullptr, 0);
	info.architecture = buffer;
#elif defined (_WIN32)
	// read: https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getlogicalprocessorinformation?utm_source=chatgpt.com
	// Not tested:
	SYSTEM_INFO sys_info;
	GetSystemInfo(&sys_info);
	info.logical_cores = sys_info.dwNumberOfProcessors;

	switch (sys_info.wProcessorArchitecture)
	{
		case (PROCESSOR_ARCHITECTURE_AMD64):
		{
			info.architecture = "x64";
			break;
		}
		case (PROCESSOR_ARCHITECTURE_INTEL):
		{
			info.architecture = "x86";
			break;
		}
		case (PROCESSOR_ARCHITECTURE_ARM64):
		{
			info.architecture = "ARM64";
			break;
		}
		default:
		{
			info.architecture = "Unknown";
			break;
		}
	}

	HKEY hKeyProcessor;
	LONG lError = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
		0,
		KEY_READ,
		&hKeyProcessor
	);

	if (lError == ERROR_SUCCESS)
	{
		char buffer[128];
		DWORD buffer_size = sizeof(buffer);
		RegQueryValueEx(hKeyProcessor, "ProcessorNameString", nullptr, nullptr, (LPBYTE)buffer, &buffer_size);
		info.model_name = buffer;

		DWORD mhz;
		buffer_size = sizeof(mhz);
		RegQueryValueEx(hKeyProcessor, "~MHz", nullptr, nullptr, (LPBYTE)&mhz, &buffer_size);
		info.clock_speed = static_cast<double>(mhz);

		RegCloseKey(hKeyProcessor);
	}

	DWORD length = 0;
	GetLogicalProcessorInformation(nullptr, &length);
	std::vector<SYSTEM_LOGICAL_PROCESSOR_INFORMATION> buffer(length / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));
	GetLogicalProcessorInformation(buffer.data(), &length);

	int physical_cores = 0;
	for (const auto& info : buffer)
	{
		if (info.Relationship == RelationProcessorCore)
		{ physical_cores++; }
	}
	info.physical_cores = physical_cores;
#endif

	return info;
}