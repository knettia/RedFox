#pragma once

#include <sstream> // std::ostringstream
#include <string>
#include <string_view>

namespace RF
{
	bool is_str_number(const std::string_view str);

	std::string to_utf8(char32_t ch);
	std::string double_to_string(double d, std::uint8_t p);

	std::vector<std::string> split_lines(std::string_view input);

	template <typename T>
	std::string to_oss_str(T &&value);

	template <typename ...Args>
	std::string format_view(std::string_view fmt, Args &&...args);

	std::string format_view(std::string_view fmt, std::vector<std::string>);
} // namespace RF

#include "./string.ipp"
