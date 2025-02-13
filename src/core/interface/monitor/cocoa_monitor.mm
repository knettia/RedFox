#import <Cocoa/Cocoa.h>
#import <CoreGraphics/CoreGraphics.h>
#import <IOKit/graphics/IOGraphicsLib.h>

#include "redfox/core/base.hpp"

void RF::Delegate::query_monitor_count(int *c)
{
	if (c == NULL)
	{ return; }

	uint32_t online_displays = 0;
	CGError err = CGGetOnlineDisplayList(0, NULL, &online_displays);
	if (err != kCGErrorSuccess)
	{
		*c = 0;
		return;
	}
	*c = static_cast<int>(online_displays);
}

void RF::Delegate::query_master_monitor(int *i)
{
	if (i == NULL)
	{ return; }

	CGDirectDisplayID main_display = CGMainDisplayID();
	uint32_t display_count = 0;
	CGDirectDisplayID online_displays[32] = { 0 };

	CGError err = CGGetOnlineDisplayList(32, online_displays, &display_count);
	if (err != kCGErrorSuccess)
	{
		*i = -1;
		return;
	}

	*i = -1;
	for (uint32_t index = 0; index < display_count; index++)
	{
		if (online_displays[index] == main_display)
		{
			*i = static_cast<int>(index);
			break;
		}
	}
}

// internal
NSString* _Nullable screen_to_display_name(CGDirectDisplayID displayID)
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

void RF::Delegate::query_monitor_data(int index, RF::MonitorData *dst)
{
	@autoreleasepool
	{
		if (dst == NULL)
		{ return; }

		uint32_t online_displays = 0;
		CGDirectDisplayID display_list[32] = { 0 };

		CGError err = CGGetOnlineDisplayList(32, display_list, &online_displays);
		if (err != kCGErrorSuccess || index < 0 || static_cast<uint32_t>(index) >= online_displays)
		{ return; }

		CGDirectDisplayID display_id = display_list[index];
		NSString *display_name = screen_to_display_name(display_id);

		if (NULL != display_name)
		{ dst->name = [display_name UTF8String]; }
		else
		{ dst->name = "Unknown"; }

		dst->resolution = RF::uivec2(CGDisplayPixelsWide(display_id), CGDisplayPixelsHigh(display_id));

		CGDisplayModeRef mode = CGDisplayCopyDisplayMode(display_id);
		if (mode != NULL)
		{
			double refresh_rate = CGDisplayModeGetRefreshRate(mode);

			if (refresh_rate < 1.0)
			{ refresh_rate = 60.0; }
			dst->refresh_rate = static_cast<uint16_t>(refresh_rate);
			CFRelease(mode);
		}
		else
		{ dst->refresh_rate = 0; }
	}
}