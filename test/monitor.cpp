// redfox
#include <redfox/core/base.hpp>

int main()
{
	RF::DelegateInfo delegate_info = {
		.name = "RedFox Delegate",
		.api = RF::WindowAPI::Cocoa
	};

	RF::Delegate delegate = RF::Delegate(delegate_info);

	int monitor_count;
	delegate.query_monitor_count(&monitor_count);

	RF::ignoramus::logf(RF::ignoramus_t::info, "Monitor count is <0>", monitor_count);

	for (int i = 0; i < monitor_count; ++i)
	{
		RF::MonitorData data;
		delegate.query_monitor_data(i, &data);
		
		// print data
		RF::ignoramus::logf(RF::ignoramus_t::info, "Name: <0>", data.name);
		RF::ignoramus::logf(RF::ignoramus_t::info, "Resolution: <0>x<1>", data.resolution.x, data.resolution.y);
		RF::ignoramus::logf(RF::ignoramus_t::info, "Refresh Rate: <0>", data.refresh_rate);

		// splitter
		if (i != monitor_count - 1)
		{ RF::ignoramus::logf(RF::ignoramus_t::info, " ----"); }
	}

	return 0;
}