inline RF::timer::timer()
{
	reset();
}

inline void RF::timer::reset()
{
	_last_time = std::chrono::steady_clock::now();
}

template<RF::duration_t time_unit>
double RF::timer::get_duration() const
{
	auto const now = std::chrono::steady_clock::now();
	auto const delta = now - _last_time;
	auto chrono_dur = std::chrono::duration_cast<std::chrono::microseconds>(delta);
	auto dur = static_cast<double>(chrono_dur.count());

	if (time_unit == RF::duration_t::millisec) return dur / 1'000.0;
	else if (time_unit == RF::duration_t::sec) return dur / 1'000'000.0;
	else return dur;
}
