#pragma once

#include "RF/system.hpp"

#include <stdexcept>

#include <string>
#include <string_view>

namespace RF
{
	class runtime_error : public std::runtime_error
	{
	private:
		RF::sys::stack_entry_t entry_;
	public:
		explicit runtime_error(const std::string_view message);

		RF::sys::stack_entry_t entry() const;
	};

	class engine_error : public RF::runtime_error
	{
	public:
		explicit engine_error(const std::string_view message);
	};
}