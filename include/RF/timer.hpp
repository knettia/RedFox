#pragma once

#include <chrono>
#include <memory>

namespace RF
{

enum class duration_t
{
	microsec,
	millisec,
	sec
};

class timer
{
public:
	timer();

	void reset();
	void tick();

	template<RF::duration_t time_unit = RF::duration_t::microsec>
	double get_duration() const;

private:
	std::chrono::steady_clock::time_point _last_time;
	std::chrono::steady_clock::duration _delta;
};

} // namespace RF

#include "timer.ipp"
