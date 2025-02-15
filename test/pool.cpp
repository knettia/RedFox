// RedFox
#include <RedFox/core/base.hpp>

int main()
{
	RF::megabytes pool_size(1); // 1 MB
	RF::pool<int> int_pool(5, pool_size);

	auto index1 = int_pool.alloc(42);
	auto index2 = int_pool.alloc(100);

	std::cout << "Index1: " << index1 << " Value: " << int_pool.get(index1) << "\n";
	std::cout << "Index2: " << index2 << " Value: " << int_pool.get(index2) << "\n";

	std::cout << "Allocated Memory: " << int_pool.memory() << " bytes\n";

	int_pool.free(index1);
	int_pool.free(index2);

	std::cout << "Allocated Memory After Dealloc: " << int_pool.memory() << " bytes\n";

	return 0;
}