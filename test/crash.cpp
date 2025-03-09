// RedFox
#include <RF/monitor.hpp>

int main()
{
	RF::monitor_m::activate(true);
	RF::monitor_m::set_save_path("crash.log");

	new int[100000000000000000]; // simulate crash

	return 0;
}