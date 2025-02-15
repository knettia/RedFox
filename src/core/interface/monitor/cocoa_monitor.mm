#if defined (__APPLE__)
// RedFox
#include "RedFox/core/interface/delegate.hpp"
#include "RedFox/core/interface/monitor.hpp"

// Cocoa
#import <Cocoa/Cocoa.h>
#import <CoreGraphics/CoreGraphics.h>
#import <IOKit/graphics/IOGraphicsLib.h>

int RF::delegate::cocoa_monitor_count()
{
	unsigned int online_displays = 0;
	CGError err = CGGetOnlineDisplayList(0, NULL, &online_displays);
	if (err != kCGErrorSuccess)
	{ throw std::runtime_error("fatal error fetching online display list"); }
	return online_displays;
}

// internal
NSString *_Nullable RF_cocoa_screen_to_display_name(CGDirectDisplayID displayID)
{
	NSString *screenName = nil;
	
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wdeprecated-declarations"
	NSDictionary *deviceInfo = CFBridgingRelease(IODisplayCreateInfoDictionary(CGDisplayIOServicePort(displayID), kIODisplayOnlyPreferredName));
	#pragma clang diagnostic pop
	
	NSDictionary *localizedNames = deviceInfo[@(kDisplayProductName)];
	
	if (localizedNames.count > 0)
	{ return localizedNames[localizedNames.allKeys[0]]; }
	
	return nil;
}

RF::monitor_data RF::delegate::cocoa_monitor_data(int index)
{
	@autoreleasepool
	{
		RF::monitor_data result;

		uint32_t online_displays = 0;
		CGDirectDisplayID display_list[32] = { 0 };

		CGError err = CGGetOnlineDisplayList(32, display_list, &online_displays);
		if (err != kCGErrorSuccess || index < 0 || static_cast<uint32_t>(index) >= online_displays)
		{ throw std::runtime_error("fatal error fetching online display list"); }

		CGDirectDisplayID display_id = display_list[index];
		NSString *display_name = RF_cocoa_screen_to_display_name(display_id);

		if (nullptr != display_name)
		{ result.name = [display_name UTF8String]; }
		else
		{ result.name = "Unknown"; }

		result.resolution = RF::uivec2(CGDisplayPixelsWide(display_id), CGDisplayPixelsHigh(display_id));

		CGDisplayModeRef mode = CGDisplayCopyDisplayMode(display_id);
		if (NULL != mode)
		{
			double refresh_rate = CGDisplayModeGetRefreshRate(mode);

			if (refresh_rate < 1.0)
			{ refresh_rate = 60.0; }
			result.refresh_rate = static_cast<uint16_t>(refresh_rate);
			CFRelease(mode);
		}
		else
		{ result.refresh_rate = 0; }

		return result;
	}
}
#else
#error "fatal error: MacOS source in non-MacOS build"
#endif