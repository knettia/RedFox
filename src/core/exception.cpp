#include "RF/exception.hpp" // header

RF::runtime_error::runtime_error(const std::string_view message)
:
	std::runtime_error(std::string(message))
{
	RF::sys::stack_trace_t stack_trace = RF::sys::get_stack_trace();

	this->entry_ = stack_trace[stack_trace.size() - 1];
}

RF::sys::stack_entry_t RF::runtime_error::entry() const
{
	return this->entry_;
}

RF::engine_error::engine_error(const std::string_view message)
:
	RF::runtime_error(message)
{ }