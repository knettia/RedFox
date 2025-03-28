#pragma once

#include <vector>

#include "RF/definitions.hpp"
#include "RF/vec2.hpp"

#if defined (__DARWIN__)
#if defined (__OBJC__)
#import "CoreGraphics/CoreGraphics.h"
#else
struct CGDisplayMode;
#endif
#endif

namespace RF
{
	struct video_mode_t
	{
		RF::uivec2 extent;
		double refresh_rate;

	#if defined (__DARWIN__)
		CGDisplayMode *to_native();
	#endif
	};

	RF::video_mode_t get_current_video_mode();
	std::vector<RF::video_mode_t> enumerate_video_modes();
} // namespace RF
