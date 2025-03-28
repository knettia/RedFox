#import "RF/interface/video_mode.hpp"
#import "RF/exception.hpp"

CGDisplayMode *RF::video_mode_t::to_native()
{
	@autoreleasepool
	{

	// FIXME: For now, this supports only the root display
	//        But in the future, we want to implement a struct
	//        Called RF::display_t to representent different displays
	CFArrayRef modes = CGDisplayCopyAllDisplayModes(CGMainDisplayID(), NULL);
	CGDisplayMode *native = nullptr;

	for (CFIndex i = 0; i < CFArrayGetCount(modes); i++)
	{
		CGDisplayMode *mode = (CGDisplayMode *)CFArrayGetValueAtIndex(modes, i);

		bool matches_resolution = (this->extent.x == CGDisplayModeGetWidth(mode)
		                          && this->extent.y == CGDisplayModeGetHeight(mode));
		
		bool matches_refresh = (this->refresh_rate == CGDisplayModeGetRefreshRate(mode));

		if (matches_resolution && matches_refresh)
		{
			native = mode;
			break;
		}
	}

	CFRelease(modes);

	if (!native)
	{
		throw RF::engine_error("Internal inconsistency: no CGDisplayMode found matching RF::video_mode_t with extent '<0>x<1>' and refresh rate '<2>'", this->extent.x, this->extent.y, this->refresh_rate);
	}

	CFRetain(native);
	return native;

	}
}

RF::video_mode_t RF::get_current_video_mode()
{
	@autoreleasepool
	{

	// FIXME: Same issue as RF::video_mode_t::to_native()
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

std::vector<RF::video_mode_t> RF::enumerate_video_modes()
{
	@autoreleasepool
	{

	// FIXME: Same issue as RF::video_mode_t::to_native()
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

		modes.emplace_back(mode);
	}

	return modes;

	}
}
