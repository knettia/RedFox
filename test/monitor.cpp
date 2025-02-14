// redfox
#include <redfox/core/base.hpp>

int main()
{
	RF::delegate_info delegate_info = {
		.name = "RedFox Delegate",
		.api = RF::window_API::Cocoa
	};

	RF::delegate delegate = RF::delegate(delegate_info);

	RF::ignoramus::logf(RF::ignoramus_t::info, "Monitor count is <0>", delegate.monitor_count());
	RF::ignoramus::logf(RF::ignoramus_t::info, " ----");

	for (int i = 0; i < delegate.monitor_count(); ++i)
	{
		RF::monitor_data data = delegate.monitor_data(i);
		
		// print data
		RF::ignoramus::logf(RF::ignoramus_t::info, "Name: <0>", data.name);
		RF::ignoramus::logf(RF::ignoramus_t::info, "Resolution: <0>x<1>", data.resolution.x, data.resolution.y);
		RF::ignoramus::logf(RF::ignoramus_t::info, "Refresh Rate: <0>", data.refresh_rate);

		// splitter
		if (i != delegate.monitor_count() - 1)
		{ RF::ignoramus::logf(RF::ignoramus_t::info, " ----"); }
	}

	return 0;
}