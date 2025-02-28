#pragma once

#include <sstream> // std::ostringstream
#include <string>
#include <string_view>
#include <locale>
#include <codecvt>
#include <array>
#include <stdexcept>

namespace RF
{
	template <typename T>
	std::string to_oss_str(T &&value);

	bool is_str_number(const std::string_view str);

	std::string to_utf8(char32_t ch);

	template <typename ...Args>
	std::string format_view(std::string_view fmt, Args &&...args);
} // namespace RF

#include "string_utilities.ipp" // Include the inline implementation
