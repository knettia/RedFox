#include "RF/string.hpp" // RF::format_view

template<typename T>
inline T RF::lib::get_function_raw(const std::string_view symbol)
{
	#if defined (__linux__) || defined (__APPLE__)
	void *func = dlsym(handle_, symbol.data());
	if (!func) {
		throw std::runtime_error(RF::format_view("could not find/load library symbol '<0>'", symbol));
	}
	return reinterpret_cast<T>(func);
	#elif defined (_WIN32)
	FARPROC func = GetProcAddress(static_cast<HMODULE>(handle_), symbol.data());
	if (!func) {
		throw std::runtime_error(RF::format_view("could not find/load library symbol '<0>'", symbol));
	}
	return reinterpret_cast<T>(func);
	#endif
}

template<typename T>
inline std::function<T> RF::lib::get_function(const std::string_view symbol)
{
	#if defined (__linux__) || defined (__APPLE__)
	void *func = dlsym(handle_, symbol.data());
	if (!func)
	{ throw std::runtime_error(RF::format_view("could not find/load library symbol \'<0>\'", symbol)); }

	return reinterpret_cast<T *>(func);
	#elif defined (_WIN32)
	FARPROC func = GetProcAddress(static_cast<HMODULE>(handle_), symbol.data());
	if (!func)
	{ throw std::runtime_error(RF::format_view("could not find/load library symbol \'<0>\'", symbol)); }

	return reinterpret_cast<T *>(func);
	#endif
}