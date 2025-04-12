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
#include "RF/string.hpp"

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
#elif defined (_WIN32)
#include <dbghelp.h>
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

	free(symbols);
#elif defined(_WIN32)
	void* callstack[UINT8_MAX];
	HANDLE process = GetCurrentProcess();
	SymInitialize(process, nullptr, TRUE);

	CONTEXT context;
	RtlCaptureContext(&context);

	STACKFRAME64 stackframe;
	memset(&stackframe, 0, sizeof(STACKFRAME64));

#ifdef _M_IX86
	stackframe.AddrPC.Offset = context.Eip;
	stackframe.AddrPC.Mode = AddrModeFlat;
	stackframe.AddrFrame.Offset = context.Ebp;
	stackframe.AddrFrame.Mode = AddrModeFlat;
	stackframe.AddrStack.Offset = context.Esp;
	stackframe.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
	stackframe.AddrPC.Offset = context.Rip;
	stackframe.AddrPC.Mode = AddrModeFlat;
	stackframe.AddrFrame.Offset = context.Rsp;
	stackframe.AddrFrame.Mode = AddrModeFlat;
	stackframe.AddrStack.Offset = context.Rsp;
	stackframe.AddrStack.Mode = AddrModeFlat;
#endif

	for (int i = 0; i < UINT8_MAX; i++)
	{
		if (!StackWalk64(
			// TODO: change on system
			IMAGE_FILE_MACHINE_AMD64,
			process,
			GetCurrentThread(),
			&stackframe,
			&context,
			nullptr,
			SymFunctionTableAccess64,
			SymGetModuleBase64,
			nullptr
		))
		{ break; }

		DWORD64 address = stackframe.AddrPC.Offset;
		char symbol_buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
		PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(symbol_buffer);
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		symbol->MaxNameLen = MAX_SYM_NAME;

		if (SymFromAddr(process, address, nullptr, symbol))
		{
			MODULEINFO moduleInfo;
			HMODULE hModule = (HMODULE)SymGetModuleBase64(process, address);
			if (hModule && GetModuleInformation(process, hModule, &moduleInfo, sizeof(moduleInfo)))\
			{
				char module_name[MAX_PATH];
				GetModuleFileNameA(hModule, module_name, sizeof(module_name));
				std::string process_name = RF::basename(std::string_view(module_name));
		
				stack_entry_t entry
				{
					.frame = std::int8_t(i + 1),
					.address = address,
					.process = process_name,
					.callname = std::string(symbol->Name)
				};

				stack_entries.push_back(entry);
			}
			else
			{
				stack_entry_t entry
				{
					.frame = std::int8_t(i + 1),
					.address = address,
					.process = "???",
					.callname = "???"
				};
				
				stack_entries.push_back(entry);
			}
		}
		else 
		{
			stack_entry_t entry
			{
				.frame = std::int8_t(i + 1),
				.address = address,
				.process = "???",
				.callname = "???"
			};

			stack_entries.push_back(entry);
		}
	}

	SymCleanup(process);
#endif

	return stack_entries;
}

#if defined (__linux__)
#include <fstream>
#include <regex>

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
#endif // __linux__

#include "RF/definitions.hpp"

#if defined(__UNIX_LIKE__)
#include <sys/utsname.h>
#elif defined(__WINDOWS__)
#include <versionhelpers.h>
#endif

std::string RF::sys::distribution_name()
{
#if defined(__LINUX__)
	std::ifstream stream("/etc/os-release");
	std::string line;
	std::regex name_regex("^NAME=\"(.*?)\"$");
	std::smatch match;

	std::string name = "???";
	while (std::getline(stream, line))
	{
		if (std::regex_search(line, match, name_regex))
		{
			name = match[1].str();
			break;
		}
	}

	return name;
#elif defined(__BSD__)
	struct utsname uts;

	std::string name = "???";
	if (uname(&uts) == 0)
	{
		name = uts.sysname; // Darwin, FreeBSD, OpenBSD, NetBSD
	}

	return name;
#elif defined(__WINDOWS__)
	// TODO: determine Windows distribution in a better way
	OSVERSIONINFOEX osvi = { sizeof(OSVERSIONINFOEX) };
	GetVersionEx((OSVERSIONINFO*)&osvi);

	std::string name = "???";
	if (IsWindows10OrGreater())
	{
		name = (osvi.dwBuildNumber >= 22000) ? "Windows 11" : "Windows 10";
	}
	else if (IsWindows8Point1OrGreater()) { name = "Windows 8.1"; }
	else if (IsWindows8OrGreater()) { name = "Windows 8"; }
	else if (IsWindows7OrGreater()) { name = "Windows 7"; }
	else if (IsWindowsVistaOrGreater()) { name = "Windows Vista"; }
	else if (IsWindowsXPOrGreater()) { name = "Windows XP"; }
	else { name = "Windows (Unknown)"; }

	return name;
#endif
}

std::string RF::sys::distribution_version()
{
	#if defined(__LINUX__)
	std::ifstream stream("/etc/os-release");
	std::string line;
	std::regex version_regex("^VERSION=\"(.*?)\"$");
	std::smatch match;

	std::string version = "???";
	while (std::getline(stream, line))
	{
		if (std::regex_search(line, match, version_regex))
		{
			version = match[1].str();
			break;
		}
	}

	return version;
#elif defined(__BSD__)
	struct utsname uts;

	std::string version = "???";
	if (uname(&uts) == 0)
	{
		version = uts.release;
	}

	return version;
#elif defined(__WINDOWS__)
	// TODO: determine Windows distribution in a better way
	using RtlGetVersion_t = LONG(WINAPI*)(PRTL_OSVERSIONINFOW);
	HMODULE hMod = GetModuleHandleW(L"ntdll.dll");
	if (hMod)
	{
		auto rtlGetVersion = (RtlGetVersion_t)GetProcAddress(hMod, "RtlGetVersion");
		if (rtlGetVersion)
		{
			RTL_OSVERSIONINFOW info = { 0 };
			info.dwOSVersionInfoSize = sizeof(info);
			if (rtlGetVersion(&info) == 0)
			{
				return std::to_string(info.dwMajorVersion) + "." +
				       std::to_string(info.dwMinorVersion) + "." +
				       std::to_string(info.dwBuildNumber);
			}
		}
	}

	OSVERSIONINFO osvi { sizeof(OSVERSIONINFO) };
	if (GetVersionEx(&osvi))
	{
		return std::to_string(osvi.dwMajorVersion) + "." +
		       std::to_string(osvi.dwMinorVersion);
	}

	return "???";
#endif
}

std::string RF::sys::get_process_name()
{
	// read: https://stackoverflow.com/questions/9097201/how-to-get-current-process-name-in-linux
	#if defined(__APPLE__) || defined(__FreeBSD__)
	return getprogname();
	#elif defined(_GNU_SOURCE)
	return basename(program_invocation_name);
	#elif defined(_WIN32)
	return RF::basename(__argv[0]);
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

#if defined (__linux__)
#include <unordered_map>
#endif // __linux__

#if defined (__APPLE__)
#include <sys/sysctl.h>
#endif // __APPLE__

RF::sys::cpu_info_t RF::sys::get_cpu_info()
{
	RF::sys::cpu_info_t info;
#if defined (__linux__)
	// read: https://www.geeksforgeeks.org/check-linux-cpu-information
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
			{ cpuinfo_map[RF::trim(key)] = RF::trim(value); }
		}
	}

	info.model = cpuinfo_map["model name"];
	info.clock_speed = std::stod(cpuinfo_map["cpu MHz"]);
	info.architecture = cpuinfo_map["cpu family"];
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
	info.clock_speed = static_cast<long double>(frequency) / 1'000'000; 

	buffer_size = sizeof(buffer);
	sysctlbyname("hw.machine", &buffer, &buffer_size, nullptr, 0);
	info.architecture = buffer;
#elif defined (_WIN32)
	// read: https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getlogicalprocessorinformation
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
		info.model = buffer;

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

#if defined (__LINUX__) || defined (__BSD_KERNEL_)
std::optional<std::string> RF::sys::find_core_library(std::string name)
{
	// TODO: implement proper BSD as well
	for (const auto &dir : RF::sys::linux_lib_dirs)
	{
		try
		{
			for (const auto& entry : fs::directory_iterator(dir))
			{
				if (entry.is_regular_file() && entry.path().filename().string().find(name) != std::string::npos)
				{
					return entry.path().string();
				}
			}
		}
		catch (const std::exception& e)
		{
			continue;
		}
	}

	return std::nullopt;
}
#endif
