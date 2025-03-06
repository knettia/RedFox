#pragma once

#include <iostream>
#include "RF/string.hpp"

namespace RF::logf
{
	enum class level_t
	{
		error,
		info,
		warn,
	};
	
	template <typename ...Args>
	void out(level_t level, std::string_view fmt, Args &&...args);

	template <typename ...Args>
	void error(std::string_view fmt, Args &&...args);

	template <typename ...Args>
	void info(std::string_view fmt, Args &&...args);

	template <typename ...Args>
	void warn(std::string_view fmt, Args &&...args);
}

namespace RF::logf::detail
{
	constexpr std::string_view reset   = "\033[0m";
	constexpr std::string_view error   = "\033[31m"; 
	constexpr std::string_view info    = "\033[32m";  
	constexpr std::string_view warn    = "\033[33m";
}

#include "./log.ipp"