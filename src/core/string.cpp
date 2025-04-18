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

std::string RF::int_to_string(std::uint64_t i, std::uint8_t d)
{
	std::ostringstream oss;
	oss << std::setw(d) << std::setfill('0') << i;
	return oss.str();
}

std::string RF::int_to_hex(std::uint64_t i, std::uint8_t s)
{
	std::ostringstream oss;
	oss << "0x" << std::setw(16) << std::setfill('0') << std::hex << i;
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

std::string RF::trim(std::string_view str)
{
	// std::size_t first = str.find_first_not_of(' ');
	std::size_t first = str.find_first_not_of(" \t\n\r\f\v");

	if (first == std::string_view::npos)
	{ return std::string(); }

	// std::size_t last = str.find_last_not_of(' ');
	std::size_t last = str.find_last_not_of(" \t\n\r\f\v");

	return std::string(str.substr(first, (last - first + 1)));
}

#include "RF/definitions.hpp"

#if defined (__UNIX_LIKE__)
#include <libgen.h> // basename
#if defined (__LINUX__)
#undef basename
#endif
#endif // __linux__, __APPLE__

std::string RF::basename(std::string_view str)
{
#if defined (__UNIX_LIKE__)
#if defined (__LINUX__)
	return std::string(::__xpg_basename(const_cast<char *>(str.data())));
#else
	return std::string(::basename(const_cast<char *>(str.data())));
#endif
#elif defined (_WIN32)
	std::string_view::size_type pos = str.find_last_of("\\/");
	if (pos != std::string_view::npos)
	{
		return std::string(str.substr(pos + 1));
	}

	return std::string(str);
#endif
}

std::string RF::format_view(std::string_view fmt, std::vector<std::string> list)
{
	std::ostringstream result;
	std::size_t i = 0;

	while (i < fmt.size())
	{
		if (fmt[i] == '\\')
		{
			if (i + 1 < fmt.size() && (fmt[i + 1] == '<' || fmt[i + 1] == '>'))
			{
				result << fmt[i + 1];
				i += 2;
			}
			else
			{
				result << fmt[i++];
			}
		}
		else if (fmt[i] == '<')
		{
			std::size_t end = fmt.find('>', i);

			if (end == std::string_view::npos)
			{
				throw std::runtime_error("mismatched tag, no closing bracket '>' found");
			}

			std::string index_str = std::string(fmt.substr(i + 1, end - i - 1));

			if (index_str.empty() || !RF::is_str_number(index_str))
			{
				throw std::runtime_error("invalid tag specifier <" + index_str + ">, have you meant to index a number?");
			}

			std::size_t index = static_cast<std::size_t>(std::stoi(index_str));

			if (index >= list.size())
			{
				throw std::runtime_error("argument index out of range for tag specifier <" + index_str + ">");
			}

			result << list[index];
			i = end + 1;
		}
		else
		{
			result << fmt[i++];
		}
	}

	return std::move(result).str();
}
