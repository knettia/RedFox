#import "./cocoa_delegate.hpp"
#include "RF/exception.hpp"

// internal cocoa manager
@implementation cocoa_delegate_m
- (void) set_delegate:(RF::cocoa_delegate *) delegate
{
	if (self)
	{ self.delegate_ = delegate; }
}
- (void) set_terminate_callback:(std::function<void()> *) callback
{
	if (self)
	{ self.terminate_callback_ = callback; }
}
- (NSApplicationTerminateReply) applicationShouldTerminate:(NSApplication *) sender
{
	if (self.delegate_ && self.terminate_callback_)
	{ (*self.terminate_callback_)(); }
	return NSTerminateCancel; // handle it ourselves
}
@end
// cocoa_delegate_m

// ---- Video Mode ----
RF::video_mode_t RF::cocoa_delegate::current_video_mode()
{
	@autoreleasepool
	{

	// FIXME: For now, this supports only the root display
	//        But in the future, we want to implement a struct
	//        Called RF::display_t to representent different displays
	CGDisplayMode *native = CGDisplayCopyDisplayMode(CGMainDisplayID());

	RF::video_mode_t mode
	{
		{ CGDisplayModeGetWidth(native), CGDisplayModeGetHeight(native) },
		CGDisplayModeGetRefreshRate(native)
	};

	CFRelease(native);

	return mode;

	}
}

std::vector<RF::video_mode_t> RF::cocoa_delegate::enumerate_video_modes()
{
	@autoreleasepool
	{

	// FIXME: Same issue as RF::cocoa_delegate::current_video_mode()
	CFArrayRef cf_modes = CGDisplayCopyAllDisplayModes(CGMainDisplayID(), NULL);

	std::vector<RF::video_mode_t> modes(CFArrayGetCount(cf_modes));

	for (CFIndex i = 0; i < CFArrayGetCount(cf_modes); i++)
	{
		CGDisplayMode *cf_mode = (CGDisplayMode *)CFArrayGetValueAtIndex(cf_modes, i);

		RF::video_mode_t mode
		{
			{ CGDisplayModeGetWidth(cf_mode), CGDisplayModeGetHeight(cf_mode) },
			CGDisplayModeGetRefreshRate(cf_mode)
		};

		modes[i] = mode;
	}

	return modes;

	}
}

CGDisplayMode *RF::cocoa_delegate::to_native_video_mode_cocoa(RF::video_mode_t video_mode)
{
	@autoreleasepool
	{

	// FIXME: Same issue as RF::cocoa_delegate::current_video_mode()
	CFArrayRef modes = CGDisplayCopyAllDisplayModes(CGMainDisplayID(), NULL);
	CGDisplayMode *native = nullptr;

	for (CFIndex i = 0; i < CFArrayGetCount(modes); i++)
	{
		CGDisplayMode *mode = (CGDisplayMode *)CFArrayGetValueAtIndex(modes, i);

		bool matches_resolution = (video_mode.extent.x == CGDisplayModeGetWidth(mode)
		                          && video_mode.extent.y == CGDisplayModeGetHeight(mode));
		
		bool matches_refresh = (video_mode.refresh_rate == CGDisplayModeGetRefreshRate(mode));

		if (matches_resolution && matches_refresh)
		{
			native = mode;
			break;
		}
	}

	CFRelease(modes);

	if (!native)
	{
		throw RF::engine_error
		(
			"Internal inconsistency: no CGDisplayMode found matching RF::video_mode_t with extent '<0>x<1>' and refresh rate '<2>'",
			video_mode.extent.x, video_mode.extent.y,
			video_mode.refresh_rate\
		);
	}

	CFRetain(native);
	return native;

	}
}

void RF_placeholder_create_cocoa_menu(NSApplication *ns_application, std::string_view delegate_name)
{
	NSMenu *menu_bar = [[NSMenu alloc] init];
	[ns_application setMainMenu:menu_bar];

	NSMenuItem *app_menu_item = [menu_bar addItemWithTitle:@"" action:NULL keyEquivalent:@""];
	NSMenu *app_menu = [[NSMenu alloc] init];
	[app_menu_item setSubmenu:app_menu];

	[app_menu addItem:[NSMenuItem separatorItem]];
	[app_menu addItemWithTitle:[NSString stringWithFormat:@"Hide %s", delegate_name.data()] action:@selector(hide:) keyEquivalent:@"h"];
	[[app_menu addItemWithTitle:@"Hide Others" action:@selector(hideOtherApplications:) keyEquivalent:@"h"] setKeyEquivalentModifierMask:NSEventModifierFlagOption | NSEventModifierFlagCommand];

	[app_menu addItemWithTitle:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];

	[app_menu addItem:[NSMenuItem separatorItem]];

	[app_menu addItemWithTitle:[NSString stringWithFormat:@"Quit %s", delegate_name.data()] action:@selector(terminate:) keyEquivalent:@"q"];

	NSMenuItem *window_menu_item = [menu_bar addItemWithTitle:@"" action:NULL keyEquivalent:@""];
	[menu_bar release];

	NSMenu *window_menu = [[NSMenu alloc] initWithTitle:@"Window"];
	[ns_application setWindowsMenu:window_menu];
	[window_menu_item setSubmenu:window_menu];

	[window_menu addItemWithTitle:@"Miniaturise" action:@selector(performMiniaturize:) keyEquivalent:@"m"];

	SEL setAppleMenuSelector = NSSelectorFromString(@"setAppleMenu:");
	[ns_application performSelector:setAppleMenuSelector withObject:app_menu];
}

// RF::cocoa_delegate implementation:
RF::cocoa_delegate::~cocoa_delegate()
{ this->terminate(); }

void RF::cocoa_delegate::terminate()
{
	@autoreleasepool
	{
		if (this->ns_application_)
		{
			[this->ns_application_ activateIgnoringOtherApps:NO];
			[this->ns_application_ release];
			this->ns_application_ = nil;
		}
		
		if (this->delegate_m_)
		{
			[this->delegate_m_ release];
			this->delegate_m_ = nil;
		}
	}
}

RF::cocoa_delegate::cocoa_delegate(RF::delegate_info info) : RF::delegate(info)
{
	@autoreleasepool
	{
		this->delegate_m_ = [cocoa_delegate_m alloc];
		[this->delegate_m_ set_delegate:this];
		[this->delegate_m_ set_terminate_callback:&this->terminate_callback_];

		this->ns_application_ = [NSApplication sharedApplication];
		[this->ns_application_ setActivationPolicy:NSApplicationActivationPolicyRegular];

		[this->ns_application_ setDelegate:static_cast<cocoa_delegate_m *>(this->delegate_m_)];

		RF_placeholder_create_cocoa_menu(this->ns_application_, this->info_.name);

		[this->ns_application_ finishLaunching];

		[this->ns_application_ activateIgnoringOtherApps:YES];
	}
}

void RF::cocoa_delegate::poll_events()
{
	@autoreleasepool
	{
		for (;;)
		{
			NSEvent *event = [
				this->ns_application_
				nextEventMatchingMask:NSEventMaskAny
				untilDate:[NSDate distantPast]
				inMode:NSDefaultRunLoopMode
				dequeue:YES
			];

			if (event == nil)
			{ break; }

			[this->ns_application_ sendEvent:event];
			[this->ns_application_ updateWindows];
		}
	}
}

#import <Carbon/Carbon.h>
#import "RF/interface/virtual_key.hpp"

char32_t RF::cocoa_delegate::to_keysym(RF::virtual_key_t key)
{
	std::optional<CGKeyCode> codeopt = RF::cocoa_key_map[key];
	if (!codeopt.has_value())
	{ return char32_t { U'\uFFFD' }; }

	CGKeyCode code = codeopt.value();
	
	TISInputSourceRef current_keyboard = TISCopyCurrentKeyboardLayoutInputSource();
	CFDataRef layout_data = (CFDataRef)TISGetInputSourceProperty(current_keyboard, kTISPropertyUnicodeKeyLayoutData);
	
	if (!layout_data) {
		CFRelease(current_keyboard);
		throw std::runtime_error("fatal error: layout data for current keyboard not found");
	}

	const UCKeyboardLayout *keyboard_layout = (const UCKeyboardLayout *)CFDataGetBytePtr(layout_data);
	if (!keyboard_layout) {
		CFRelease(current_keyboard);
		throw std::runtime_error("fatal error: keyboard layout for current keyboard not found");
	}

	std::uint32_t dead_key_state = 0;
	uint16_t str;
	unsigned long real_length = 0;

	OSStatus status = UCKeyTranslate(
		keyboard_layout,
		code,
		kUCKeyActionDown,
		((GetCurrentKeyModifiers()) >> 8) & 0xFF,
		LMGetKbdType(),
		kUCKeyTranslateNoDeadKeysBit,
		&dead_key_state,
		sizeof(str),
		&real_length,
		&str
	);

	CFRelease(current_keyboard);
	
	if (status != noErr || real_length == 0)
	{ return char32_t { U'\uFFFD' }; }

	return static_cast<char32_t>(str);
}

#import "../window/cocoa_window.hpp"

RF::window *RF::cocoa_delegate::create_window(RF::window_info info)
{
	return new RF::cocoa_window(RF::reference_ptr<RF::delegate>(this), info);
	[this->ns_application_ run];
}
