// RedFox
#include <RedFox/core/base.hpp>

int main()
{
	#if defined (__linux__)
	auto lib = RF::library_m::self().load_library("bin/libsquare.so");
	#elif defined (__APPLE__)
	auto lib = RF::library_m::self().load_library("bin/libsquare.dylib");
	#elif defined (_WIN32)
	auto lib = RF::library_m::self().load_library("bin\\Debug\\square.dll");
	#endif

	auto func = lib->get_function<int(int)>("square");

	if (func)
	{
		std::initializer_list<int> to_square = { 10, 25, 18, 19, 82, 2, 74, 101};

		for (const int i : to_square)
		{
			int result = func(i);
			RF::ignoramus::logf(RF::ignoramus_t::info, "Squared <0> to <1>", i, result);
		}
	}

	RF::library_m::self().unload_library("bin/libsquare.dylib");

	return 0;
}