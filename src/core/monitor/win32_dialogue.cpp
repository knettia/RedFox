#if defined (_WIN32)
#include "RF/monitor.hpp"

void RF::monitor_m::crash_dialogue_()
{
	int result = MessageBoxA(
		NULL,
		"Oh no! A crash has occurred. We are sorry for the inconvenience, for more information, check the crash log.",
		"RedFox Engine: Process Crashed",
		MB_ICONERROR | MB_YESNO | MB_DEFBUTTON2
	);
	
	if (result == IDYES)
	{ RF::monitor_m::open_save_path_(); }
	else if (result == IDNO)
	{ PostQuitMessage(0); }
}

void RF::monitor_m::exception_dialogue_(std::string_view title, std::string_view description)
{
	int result = MessageBoxA(
		NULL,
		description.data(),
		title.data(),
		MB_ICONERROR | MB_YESNO | MB_DEFBUTTON2
	);
	
	if (result == IDYES)
	{ RF::monitor_m::open_save_path_(); }
	else if (result == IDNO)
	{ PostQuitMessage(0); }
}
#endif // _WIN32