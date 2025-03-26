#import "cocoa_window.hpp" // header
#import "RF/interface/virtual_key.hpp"
#import "RF/log.hpp" // RF::logf::

@implementation cocoa_view_m
- (void) set_window:(RF::cocoa_window *) window
{
	if (self)
	{ self.window_ = window; }
}

// tagging in events of left mouse button 
- (void) mouseDown:(NSEvent *) event
{
	if (self.window_)
	{ self.window_->handle_mouse_key_down(RF::mouse_key_t::LMB); }
}

- (void) mouseUp:(NSEvent *) event
{
	if (self.window_)
	{ self.window_->handle_mouse_key_up(RF::mouse_key_t::LMB); }
}

// tagging in events of right mouse button 
- (void) rightMouseDown:(NSEvent *) event
{
	if (self.window_)
	{ self.window_->handle_mouse_key_down(RF::mouse_key_t::RMB); }
}

- (void) rightMouseUp:(NSEvent *) event
{
	if (self.window_)
	{ self.window_->handle_mouse_key_up(RF::mouse_key_t::RMB); }
}

const std::unordered_map<int, RF::mouse_key_t> other_mouse_key_map
{
	{2, RF::mouse_key_t::MMB},
	{3, RF::mouse_key_t::X1},
	{4, RF::mouse_key_t::X2},
	{5, RF::mouse_key_t::X3},
	{6, RF::mouse_key_t::X4},
	{7, RF::mouse_key_t::X5},
};

// tagging in events of other mouse buttons
- (void) otherMouseDown:(NSEvent *) event
{
	if (self.window_)
	{
		try
		{
			const RF::mouse_key_t &equivalent_key = other_mouse_key_map.at(event.buttonNumber);
			self.window_->handle_mouse_key_down(equivalent_key);
		}
		catch (...)
		{ RF::logf::warn("from cocoa: not supported mouse button down, index <0>", event.buttonNumber); }
	}
}

- (void) otherMouseUp:(NSEvent *) event
{
	if (self.window_)
	{
		try
		{
			const RF::mouse_key_t &equivalent_key = other_mouse_key_map.at(event.buttonNumber);
			self.window_->handle_mouse_key_up(equivalent_key);
		}
		catch (...)
		{ RF::logf::warn("from cocoa: not supported mouse button up, index <0>", event.buttonNumber); }
	}
}
@end

// internal cocoa_responder
@implementation cocoa_responder_m
- (void) set_window:(RF::cocoa_window *) window
{
	if (self)
	{ self.window_ = window; }
}

- (void) keyDown:(NSEvent *) event
{
	if (self.window_)
	{
		std::optional<RF::virtual_key_t> keyopt = RF::cocoa_key_map[event.keyCode];
		if (keyopt.has_value())
		{ self.window_->handle_virtual_key_down(keyopt.value()); }
	}
}

- (void) keyUp:(NSEvent *) event
{
	if (self.window_)
	{
		std::optional<RF::virtual_key_t> keyopt = RF::cocoa_key_map[event.keyCode];
		if (keyopt.has_value())
		{ self.window_->handle_virtual_key_up(keyopt.value()); }
	}
}
@end
// cocoa_responder

// internal cocoa_window_m
@implementation cocoa_window_m
- (void) set_window:(RF::cocoa_window *) window
{
	if (self)
	{ self.window_ = window; }
}

- (BOOL) windowShouldClose:(id) sender
{
	if (self.window_)
	{ self.window_->cocoa_call_close_callback(); }
	return NO; // handle it ourselves
}

- (void) windowDidBecomeKey:(NSNotification *) notification
{
	if (self.window_)
	{ self.window_->update_window_state(RF::window_state_t::Focused); }
}

- (void) windowDidResignKey:(NSNotification *) notification
{
	if (self.window_)
	{ self.window_->update_window_state(RF::window_state_t::Visible); }
}

- (void) windowDidMiniaturize:(NSNotification *) notification
{
	if (self.window_)
	{ self.window_->update_window_state(RF::window_state_t::Hidden); }
}
@end
// cocoa_window_m

// RF::cocoa_window implementation:
RF::cocoa_window::cocoa_window(RF::window_info info) : RF::window(info)
{
	@autoreleasepool
	{
		NSRect rect = NSMakeRect(0, 0, info.size.x, info.size.y);
		this->ns_window_ = [
			[NSWindow alloc]
			initWithContentRect:rect
			styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable )
			backing:NSBackingStoreType::NSBackingStoreBuffered
			defer:NO
		];

		[this->ns_window_ setTitle:[NSString stringWithUTF8String:info.title.data()]];

		[this->ns_window_ setCollectionBehavior:NSWindowCollectionBehaviorDefault];
		[this->ns_window_ setDelegate:this->window_m_];

		this->window_m_ = [cocoa_window_m alloc];
		[this->window_m_ set_window:this];
		this->ns_window_.delegate = this->window_m_;

		this->view_m_ = [[cocoa_view_m alloc] initWithFrame:rect];
		[this->view_m_ set_window:this];
		[this->ns_window_ setContentView:this->view_m_];

		[this->ns_window_ center];
		[this->ns_window_ makeKeyAndOrderFront:this->window_m_];

		this->responder_m_ = [cocoa_responder_m alloc];
		[this->responder_m_ set_window:this];
		[this->ns_window_ makeFirstResponder:this->responder_m_];
	}
}

RF::cocoa_window::~cocoa_window()
{ this->close(); }

// vulkan
#define VK_USE_PLATFORM_METAL_EXT
#include <vulkan/vulkan.hpp>
vk::ResultValue<vk::SurfaceKHR> RF::cocoa_window::create_surface(vk::Instance instance, const vk::AllocationCallbacks *allocator)
{
	@autoreleasepool
	{
		this->metal_layer_ = [CAMetalLayer layer];
		if (!this->metal_layer_)
		{ return vk::ResultValue<vk::SurfaceKHR>(vk::Result::eErrorExtensionNotPresent, nullptr); }

		NSView *content_view = [this->ns_window_ contentView];
		if (!content_view)
		{ return vk::ResultValue<vk::SurfaceKHR>(vk::Result::eErrorInitializationFailed, nullptr); }

		[content_view setLayer:this->metal_layer_];
		[content_view setWantsLayer:YES];

		// C type
		VkMetalSurfaceCreateInfoEXT sci = {};
		sci.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
		sci.pLayer = this->metal_layer_;

		auto vk_create_metal_surface_ext = reinterpret_cast<PFN_vkCreateMetalSurfaceEXT>(instance.getProcAddr("vkCreateMetalSurfaceEXT"));
		
		if (!vk_create_metal_surface_ext)
		{ return vk::ResultValue<vk::SurfaceKHR>(vk::Result::eErrorExtensionNotPresent, nullptr); }

		VkSurfaceKHR raw_surface;
		vk::Result result = static_cast<vk::Result>(vk_create_metal_surface_ext(
			static_cast<VkInstance>(instance), 
			&sci, 
			reinterpret_cast<const VkAllocationCallbacks*>(allocator), 
			&raw_surface
		));

		if (result != vk::Result::eSuccess)
		{ return vk::ResultValue<vk::SurfaceKHR>(result, nullptr); }

		return vk::ResultValue<vk::SurfaceKHR>(result, static_cast<vk::SurfaceKHR>(raw_surface));
	}
}

void RF::cocoa_window::close()
{
	@autoreleasepool
	{
		if (this->ns_window_)
		{
			[this->ns_window_ orderOut:nil];
			[this->ns_window_ release];
			this->ns_window_ = nil;
		}

		if (this->responder_m_)
		{
			[this->responder_m_ release];
			this->responder_m_ = nil;
		}

		if (this->view_m_)
		{
			[this->view_m_ release];
			this->view_m_ = nil;
		}

		if (this->window_m_)
		{
			[this->window_m_ release];
			this->window_m_ = nil;
		}

		if (this->metal_layer_)
		{
			[this->metal_layer_ release];
			this->metal_layer_ = nil;
		}
	}
}

void RF::cocoa_window::cocoa_call_close_callback()
{
	if (this->close_callback_)
	{ this->close_callback_(this); }
}

void RF::cocoa_window::update_window_state(RF::window_state_t new_state)
{
	if (this->state_ != new_state)
	{
		this->state_ = new_state;
		if (this->state_changed_callback_)
		{
			this->state_changed_callback_(this, new_state);
		}
	}
}

void RF::cocoa_window::focus()
{
	[this->ns_window_ makeKeyAndOrderFront:nil];
}

void RF::cocoa_window::minimise()
{
	[this->ns_window_ performMiniaturize:nil];
}

void RF::cocoa_window::handle_virtual_key_down(RF::virtual_key_t key)
{
	auto call_key_event_callback = [this](RF::virtual_key_t key, RF::key_state_t state) -> void
	{
		if (this->virtual_key_event_callback_)
		{ this->virtual_key_event_callback_(this, key, state); }
	};

	RF::key_state_t &state = this->virtual_key_states_.at(key);

	if (state == RF::key_state_t::Inactive || state == RF::key_state_t::Suppressed)
	{
		state = RF::key_state_t::Triggered;
		call_key_event_callback(key, state);
	}
	if (state == RF::key_state_t::Triggered)
	{
		state = RF::key_state_t::Ongoing;
		call_key_event_callback(key, state);
	}
	else
	{ call_key_event_callback(key, state); }
}

void RF::cocoa_window::handle_virtual_key_up(RF::virtual_key_t key)
{
	auto call_key_event_callback = [this](RF::virtual_key_t key, RF::key_state_t state) -> void
	{
		if (this->virtual_key_event_callback_)
		{ this->virtual_key_event_callback_(this, key, state); }
	};

	RF::key_state_t& state = this->virtual_key_states_.at(key);

	if (state == RF::key_state_t::Ongoing || state == RF::key_state_t::Triggered)
	{
		state = RF::key_state_t::Suppressed;
		call_key_event_callback(key, state);
		this->handle_virtual_key_up(key);
	}
	else if (state == RF::key_state_t::Suppressed)
	{
		state = RF::key_state_t::Inactive;
		call_key_event_callback(key, state);
	}
}

void RF::cocoa_window::handle_mouse_key_down(RF::mouse_key_t key)
{
	auto call_mouse_event_callback = [this](RF::mouse_key_t key, RF::key_state_t state) -> void
	{
		if (this->mouse_key_event_callback_)
		{ this->mouse_key_event_callback_(this, key, state); }
	};

	RF::key_state_t &state = this->mouse_key_states_.at(key);

	if (state == RF::key_state_t::Inactive || state == RF::key_state_t::Suppressed)
	{
		state = RF::key_state_t::Triggered;
		call_mouse_event_callback(key, state);
	}
	if (state == RF::key_state_t::Triggered)
	{
		state = RF::key_state_t::Ongoing;
		call_mouse_event_callback(key, state);
	}
	else
	{ call_mouse_event_callback(key, state); }
}

void RF::cocoa_window::handle_mouse_key_up(RF::mouse_key_t key)
{
	auto call_mouse_event_callback = [this](RF::mouse_key_t key, RF::key_state_t state) -> void
	{
		if (this->mouse_key_event_callback_)
		{ this->mouse_key_event_callback_(this, key, state); }
	};

	RF::key_state_t &state = this->mouse_key_states_.at(key);

	if (state == RF::key_state_t::Ongoing || state == RF::key_state_t::Triggered)
	{
		state = RF::key_state_t::Suppressed;
		call_mouse_event_callback(key, state);
		this->handle_mouse_key_up(key);
	}
	else if (state == RF::key_state_t::Suppressed)
	{
		state = RF::key_state_t::Inactive;
		call_mouse_event_callback(key, state);
	}
}

void RF::cocoa_window::set_size(RF::uivec2 size)
{
	NSSize ns_size = NSMakeSize(size.x, size.y);
	NSRect ns_rect = this->ns_window_.frame;

	NSRect frame = NSMakeRect(ns_rect.origin.x, ns_rect.origin.y, ns_size.width, ns_size.height);
    
    	[this->ns_window_ setFrame:frame display:YES animate:NO];
}