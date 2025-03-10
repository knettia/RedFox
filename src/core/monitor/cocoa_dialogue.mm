#if defined (__APPLE__)
#import "Cocoa/Cocoa.h"
#import "RF/monitor.hpp"

void RF::monitor_m::crash_dialogue_()
{
	NSAlert* alert = [[[NSAlert alloc] init] autorelease];

	[alert setMessageText:@"RedFox Engine: Process Crashed"];

	[alert setInformativeText:@"Oh no! A crash has occurred. We are sorry for the inconvenience, for more information, check the crash log."];

	[alert setAlertStyle:NSAlertStyleCritical];

	[alert addButtonWithTitle:@"Yes"];
	[alert addButtonWithTitle:@"No"];

	[[alert buttons][1] setKeyEquivalent:@"\r"];

	NSWindow* alert_window = [alert window];
	[alert_window setLevel:NSModalPanelWindowLevel];
	[alert_window makeKeyAndOrderFront:nil];
	[NSApp activateIgnoringOtherApps:YES];

	NSScreen* main_screen = [NSScreen mainScreen];
	NSRect screen_frame = [main_screen frame];
	NSRect window_frame = [alert_window frame];

	CGFloat x = NSMidX(screen_frame) - (window_frame.size.width / 2);
	CGFloat y = NSMidY(screen_frame) - (window_frame.size.height / 2);
	[alert_window setFrameOrigin:NSMakePoint(x, y)];

	NSModalResponse response = [alert runModal];

	if (response == NSAlertFirstButtonReturn)
	{ RF::monitor_m::open_save_path_(); }
}

void RF::monitor_m::exception_dialogue_(std::string_view title, std::string_view description)
{
	NSAlert *alert = [[[NSAlert alloc] init] autorelease];

	[alert setMessageText:[NSString stringWithUTF8String:title.data()]];

	[alert setInformativeText:[NSString stringWithUTF8String:description.data()]];

	[alert setAlertStyle:NSAlertStyleCritical];

	[alert addButtonWithTitle:@"OK"];

	NSWindow *alert_window = [alert window];
	[alert_window setLevel:NSModalPanelWindowLevel];
	[alert_window makeKeyAndOrderFront:nil];
	[NSApp activateIgnoringOtherApps:YES];

	NSScreen *main_screen = [NSScreen mainScreen];
	NSRect screen_frame = [main_screen frame];
	NSRect window_frame = [alert_window frame];

	CGFloat x = NSMidX(screen_frame) - (window_frame.size.width / 2);
	CGFloat y = NSMidY(screen_frame) - (window_frame.size.height / 2);
	[alert_window setFrameOrigin:NSMakePoint(x, y)];

	NSModalResponse response = [alert runModal];
}
#endif // __APPLE__