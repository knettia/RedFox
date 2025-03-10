// RedFox
#include <RF/monitor.hpp>
#include <RF/exception.hpp>
#include <RF/log.hpp>

int main()
{
	RF::monitor_m::activate(true);

	RF::monitor_m::set_save_path("crash.log");

	throw std::invalid_argument("failed init"); // should generate grosh
}