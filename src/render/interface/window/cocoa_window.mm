#import "RedFox/render/interface/window/cocoa_window.hpp" // header

// internal cocoa manager
@interface cocoa_window_m : NSObject <NSWindowDelegate>
{
	RF::cocoa_window *RF_window;
}
- (instancetype)from_window:(RF::cocoa_window *)window;
@end

@implementation cocoa_window_m
- (instancetype)from_window:(RF::cocoa_window *)window
{
	self = [super init];
	if (self)
	{ self->RF_window = window; }
	return self;
}
@end
// cocoa_window_m

// RF::cocoa_window implementation:
RF::cocoa_window::cocoa_window(RF::window_info info) : RF::window(info)
{
	@autoreleasepool
	{
		this->cocoa_interface = [[cocoa_window_m alloc] from_window:this];

		NSRect rect = NSMakeRect(0, 0, info.size.x, info.size.y);
		this->ns_window = [
			[NSWindow alloc]
			initWithContentRect:rect
			styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskFullSizeContentView )
			backing:NSBackingStoreType::NSBackingStoreBuffered
			defer:NO
		];

		[this->ns_window setTitle:[NSString stringWithUTF8String:info.title.data()]];

		[this->ns_window setCollectionBehavior:NSWindowCollectionBehaviorDefault];

		if ([this->ns_window respondsToSelector:@selector(setTitleVisibility:)])
		{ [this->ns_window setTitleVisibility:NSWindowTitleVisible]; }

		if ([this->ns_window respondsToSelector:@selector(setTitlebarAppearsTransparent:)])
		{ [this->ns_window setTitlebarAppearsTransparent:YES]; }

		[this->ns_window setDelegate:static_cast<cocoa_window_m *>(this->cocoa_interface)];

		[this->ns_window center];
		[this->ns_window makeKeyAndOrderFront:static_cast<cocoa_window_m *>(this->cocoa_interface)];
		[this->ns_window makeFirstResponder:[this->ns_window contentView]];
	}
}

RF::cocoa_window::~cocoa_window()
{
	[this->ns_window release];

	if (this->metal_layer)
	{ [this->metal_layer release]; }
}

void RF::cocoa_window::poll_events()
{
	@autoreleasepool
	{
		for (;;)
		{
			NSEvent *event = [
				NSApp
				nextEventMatchingMask:NSEventMaskAny
				untilDate:[NSDate distantPast]
				inMode:NSDefaultRunLoopMode
				dequeue:YES
			];

			if (event == nil)
			{ break; }

			[NSApp sendEvent:event];
		}
	} 
}

void RF::cocoa_window::close()
{
	
}

void RF::cocoa_window::focus()
{

}

void RF::cocoa_window::minimize()
{

}