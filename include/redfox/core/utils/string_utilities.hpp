// string_utilities.hpp
#pragma once

#include <sstream> // std::ostringstream
#include <string>
#include <string_view>
// #include <charconv> // std::from_chars

#include <vector>
#include <stdexcept>

namespace RF
{
	template <typename T>
	inline std::string to_oss_str(T &&value)
	{
		std::ostringstream oss;
		oss << std::forward<T>(value);
		return std::move(oss).str();
	}

	inline bool is_str_number(const std::string &str)
	{
		if (str.empty())
		{ return false; }

		size_t start = (str[0] == '-') ? 1 : 0;
		for (size_t i = start; i < str.size(); ++i)
		{
			if (!std::isdigit(str[i]))
			{ return false; }
		}

		return true;
	}


	template <typename ...Args>
	inline std::string format_view(std::string_view fmt, Args &&...args)
	{
		const std::array<std::string, sizeof...(Args)> list = { RF::to_oss_str(std::forward<Args>(args))... };

		std::ostringstream result;
		size_t i = 0;
		while (i < fmt.size())
		{
			if (fmt[i] == '<')
			{
				size_t end = fmt.find('>', i);
				if (end == std::string_view::npos)
				{ throw std::runtime_error("mismatched tag, no closing bracket '>' found"); }

				std::string index_str = std::string(fmt.substr(i + 1, end - i - 1));
				if (index_str.empty() || !RF::is_str_number(index_str))
				{ throw std::runtime_error("invalid tag specifier <" + index_str + ">, have you meant to index a number?"); }

				size_t index = static_cast<size_t>(std::stoi(index_str));
				if (index >= list.size())
				{ throw std::runtime_error("argument index out of range for tag specifier <" + index_str + ">"); }

				result << list[index]; // not so safe, but faster than list.at(index)
				i = end + 1;
			}
			else
			{ result << fmt[i++]; }
		}

		return std::move(result).str();
	}

}