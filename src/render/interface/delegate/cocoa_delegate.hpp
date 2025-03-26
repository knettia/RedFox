#if defined (__APPLE__)
#pragma once
// RedFox
#include "RF/interface/delegate.hpp"

#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
@class cocoa_delegate_m;
#else
class NSApplication;
class cocoa_delegate_m;
#endif

namespace RF
{
	class cocoa_delegate : public RF::delegate
	{
	private:
		NSApplication *ns_application_;
		cocoa_delegate_m *delegate_m_; // internal obj-c++ class
	public:
		~cocoa_delegate();
		cocoa_delegate(RF::delegate_info info);

		void terminate() override;
		void poll_events() override;
		char32_t to_keysym(RF::virtual_key_t) override;
		RF::window *create_window(RF::window_info) override;
	};
} // namespace RF

#if defined (__OBJC__)
@interface cocoa_delegate_m : NSObject <NSApplicationDelegate>
@property RF::cocoa_delegate *delegate_;
@property std::function<void()> *terminate_callback_;
- (void)set_delegate:(RF::cocoa_delegate *)delegate;
- (void)set_terminate_callback:(std::function<void()> *)callback;
@end
#endif // __OBJC__
#endif // __APPLE__