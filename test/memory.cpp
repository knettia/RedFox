// redfox
#include <redfox/core/base.hpp>

int main()
{
	RF::kilobytes k1(1);
	RF::bytes b1 = RF::memory_cast<RF::byte_scale>(k1);
	RF::ignoramus::logf(RF::ignoramus_t::info, "1KB = <0>bytes", b1.count()); 
	
	RF::gigabytes g1(1);
	RF::megabytes m1 = RF::memory_cast<RF::megabyte_scale>(g1);
	RF::ignoramus::logf(RF::ignoramus_t::info, "1GB = <0>MB", m1.count()); 
	
	RF::kilobytes k2(1500);
	RF::megabytes m2 = RF::memory_cast<RF::megabyte_scale>(k2);
	RF::ignoramus::logf(RF::ignoramus_t::info, "1500KB = <0>MB", m2.count());

	return 0;
}