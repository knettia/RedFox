#if defined (__APPLE__)
#pragma once
// RedFox
#include "RedFox/render/interface/window.hpp"

#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#else
// forward declare:
class NSObject;
class NSWindow;
class CAMetalLayer;
#endif

namespace RF
{
	class cocoa_window : public RF::window
	{
	private:
		NSObject *cocoa_interface; // internal obj-c++ class
		NSWindow *ns_window;

		CAMetalLayer *metal_layer;
	public:
		~cocoa_window() override;
		cocoa_window(RF::window_info info);

		void poll_events() override;

		void close() override;
		void focus() override;
		void minimize() override;
	};
} // namespace RF
#endif