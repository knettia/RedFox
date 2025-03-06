// RedFox
#include <RF/monitor.hpp>

int main()
{
	RF::monitor_m::activate(true);

	new int[100000000000000000]; // simulate crash

	return 0;
}