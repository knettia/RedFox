#pragma once

#include "RF/string.hpp"
#include "RF/system.hpp"

#include <stdexcept>
#include <string_view> 

namespace RF
{
	class engine_error : public std::runtime_error
	{
	private:
		RF::sys::stack_entry_t entry_;
	public:
		engine_error(const std::string_view message);
		
		template <typename ...Args>
		engine_error(const std::string_view message, Args &&...args);

		RF::sys::stack_entry_t entry() const;
	};

	class runtime_error : public std::runtime_error
	{
	private:
		RF::sys::stack_entry_t entry_;
	public:
		runtime_error(const std::string_view message);

		template <typename ...Args>
		runtime_error(const std::string_view message, Args &&...args);

		RF::sys::stack_entry_t entry() const;
	};
} // namespace RF

#include "RF/exception.ipp"
