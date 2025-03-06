// RedFox
#include <RF/mass.hpp>
#include <RF/log.hpp>

int main()
{
	RF::mass<int, 10> m;
	m.emplace_back(10);
	m.emplace_back(20);
	m.emplace_back(30);

	for (const int i : m)
	{
		RF::logf::info("Index: <0>", i);
	}

	return 0;
}