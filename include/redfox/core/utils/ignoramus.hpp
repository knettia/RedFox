// ignoramus.hpp
#pragma once

#include <string_view>

#include "redfox/core/utils/string_utilities.hpp" // RF::format_view
#include <iostream> // std::cout

namespace RF
{
	enum class ignoramus_t : int
	{
		info,
		warning,
		error
	};

	class ignoramus
	{
	public:

		// log
		template <typename ...Args>
		static inline void logf(RF::ignoramus_t type, std::string_view fmt, Args &&...args)
		{ std::cout << RF::ignoramus::type_to_hue(type) << RF::ignoramus::type_to_str(type) << RF::ignoramus::reset_hue << " :: " << RF::format_view(fmt, args...) << '\n'; }

		// log debug
		template <typename ...Args>
		static inline void logf_debug(RF::ignoramus_t type, std::string_view fmt, Args &&...args)
		{
			#ifdef NDEBUG
			return; // no-op
			#endif

			RF::ignoramus::logf(type, fmt, args...);
		}
	private:
		static constexpr std::string_view reset_hue = "\033[0m";
		static std::string type_to_hue(RF::ignoramus_t t);
		static std::string type_to_str(RF::ignoramus_t t); // ignoramus.cpp
	};
} // namespace RF