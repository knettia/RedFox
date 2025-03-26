#if defined (__APPLE__)
#pragma once
// RedFox
#include "RF/interface/window.hpp"

#if defined (__OBJC__)
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
@class cocoa_window_m;
@class cocoa_view_m;
@class cocoa_responder_m;
#else
// forward declare:
class NSWindow;
class cocoa_window_m;
class cocoa_view_m;
class cocoa_responder_m;
class CAMetalLayer;
#endif

namespace RF
{
	class cocoa_window : public RF::window
	{
	private:
		NSWindow *ns_window_;
		
		cocoa_window_m *window_m_;
		cocoa_responder_m *responder_m_;
		cocoa_view_m *view_m_;

		CAMetalLayer *metal_layer_;
	public:
		~cocoa_window() override;
		cocoa_window(RF::window_info info);

		void cocoa_call_close_callback(); // HACK: find better way to fix
		void update_window_state(RF::window_state_t state);
		
		void handle_virtual_key_down(RF::virtual_key_t key);
		void handle_virtual_key_up(RF::virtual_key_t key);
		
		void handle_mouse_key_down(RF::mouse_key_t key);
		void handle_mouse_key_up(RF::mouse_key_t key);

		vk::ResultValue<vk::SurfaceKHR> create_surface(vk::Instance instance, const vk::AllocationCallbacks *allocator) override;

		void close() override;
		void focus() override;
		void minimise() override;

		void set_size(RF::uivec2 size) override;
	};
} // namespace RF

#if defined (__OBJC__)
@interface cocoa_window_m : NSObject <NSWindowDelegate>
@property RF::cocoa_window *window_;
- (void) set_window:(RF::cocoa_window *) window;
@end

@interface cocoa_view_m : NSView
@property RF::cocoa_window *window_;
- (void) set_window:(RF::cocoa_window *) window;
@end

@interface cocoa_responder_m : NSResponder
@property  RF::cocoa_window *window_;
- (void) set_window:(RF::cocoa_window *) window;
@end
#endif // __OBJC__
#endif // __APPLE__