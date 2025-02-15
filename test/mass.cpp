// RedFox
#include <RedFox/core/base.hpp>

int main()
{
	RF::mass<int, 10> m;
	m.emplace_back(10);
	m.emplace_back(20);
	m.emplace_back(30);

	for (const int i : m)
	{
		RF::ignoramus::logf(RF::ignoramus_t::info, "Index: <0>", i);
	}


	return 0;
}