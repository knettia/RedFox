#include "RF/string.hpp"

#include <stdexcept>
#include <locale>
#include <codecvt>
#include <iomanip>

bool RF::is_str_number(const std::string_view str)
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

std::string RF::to_utf8(char32_t ch)
{
	#ifdef __clang__
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wdeprecated-declarations"
	#endif
	std::u32string utf32_str(1, ch);
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
	#ifdef __clang__
	#pragma clang diagnostic pop
	#endif
   	return converter.to_bytes(utf32_str);
}

std::string RF::double_to_string(double d, std::uint8_t p)
{
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(p) << d;
	return oss.str();
}

std::vector<std::string> RF::split_lines(std::string_view input)
{
	std::vector<std::string> lines;
	std::istringstream stream(input.data());
	std::string line;

	while (std::getline(stream, line))
	{
		lines.push_back(line);
	}

	return lines;
}

std::string RF::format_view(std::string_view fmt, std::vector<std::string> list)
{
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
