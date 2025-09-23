#include "RF/vulkan/instance.hpp"
#include "RF/vulkan/common.hpp"
// #include "RF/vulkan/debug.hpp"
#include "RF/exception.hpp"
#include <RF/interface/delegate.hpp>
#include <RF/interface/framework.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace
{
	#if defined (NDEBUG)
	constexpr bool debugging = false;
	#else
	constexpr bool debugging = true;
	#endif
	
	#if defined (__DARWIN__)
	constexpr bool on_darwin = true;
	#else
	constexpr bool on_darwin = false;
	#endif
}

// void gfx::set_required_instance_extensions(std::vector<const char *> &extensions)
// {
// 	// required Vulkan extensions
// 	extensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);

// 	// platform-specific extensions
// #if defined(__linux__)
// 	extensions.emplace_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
// #elif defined(__APPLE__)
// 	extensions.emplace_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
// 	extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME); // non-native portability
// #elif defined(_WIN64)
// 	extensions.emplace_back("VK_KHR_win32_surface");
// #endif

// 	// debug-specific extensions
// #if !defined (NDEBUG)
// 	extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
// #endif
// }

// void gfx::set_required_instance_layers(std::vector<const char *> &layers)
// {
// 	// required layers for programme
// 	// none

// 	// debug-specific extensions
// #if !defined (NDEBUG)
// 	layers.emplace_back("VK_LAYER_KHRONOS_validation");
// #endif
// }

#if defined(__linux__)
#include <X11/Xlib.h>
#include <vulkan/vulkan_xlib.h> // VK_KHR_XLIB_SURFACE_EXTENSION_NAME
#include <vulkan/vulkan_wayland.h> // VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME
#elif defined(__APPLE__)
#include <vulkan/vulkan_metal.h> // VK_EXT_METAL_SURFACE_EXTENSION_NAME
#elif defined(_WIN64)
#include <vulkan/vulkan_win32.h> // VK_EXT_WIN32_SURFACE_EXTENSION_NAME
#endif


RF::vulkan::instance_config_t::instance_config_t(const RF::delegate_info info, const bool surface, const bool debug)
{
#if defined (__APPLE__)
	this->extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME); // non-native portability
#endif

	if (surface) // generate surface if rendering in real-time
	{
		this->extensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);

		switch (info.framework)
		{
		#if defined(__linux__)
			case (RF::framework_t::X11):
			{
				this->extensions.emplace_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
				break;
			}
			case (RF::framework_t::Wayland):
			{
				this->extensions.emplace_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
				break;
			}
		#elif defined(__APPLE__)
			case (RF::framework_t::Cocoa):
			{
				this->extensions.emplace_back(VK_EXT_METAL_SURFACE_EXTENSION_NAME);
				break;
			}
		#elif defined(_WIN64)
			case (RF::framework_t::Win32):
			{
				this->extensions.emplace_back(VK_EXT_WIN32_SURFACE_EXTENSION_NAME);
				break;
			}
		#endif
			default: throw RF::engine_error("Unsupported window framework for Vulkan surface creation.");
		}
	}

	if (debug)
	{
		this->extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		this->layers.emplace_back("VK_LAYER_KHRONOS_validation");
	}
}

void RF::vulkan::instance_impl_t::init(const vk::ApplicationInfo app,
                                       const RF::vulkan::instance_config_t config)
{
	if (this->initialised_) throw RF::engine_error("Cannot initialise an already initialised RF::vulkan::instance_impl_t");

	if (debugging) if (!RF::vulkan::has_validation_layers()) throw RF::engine_error("Vulkan validation layer is not supported debug RF::vulkan::instance_impl_t");

	std::vector<const char *> layers;
	layers.reserve(config.layers.size());
	for (std::string_view str : config.layers) layers.push_back(str.data());

	std::vector<const char *> extensions;
	extensions.reserve(config.extensions.size());
	for (std::string_view str : config.extensions) extensions.push_back(str.data());

	auto debug_info = RF::vulkan::generate_debug_info();

	vk::InstanceCreateInfo create_info((on_darwin)
	                                   ? vk::InstanceCreateFlags(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR)
	                                   : vk::InstanceCreateFlags(0),
	                                   &app,
	                                   layers.size(), layers.data(),
	                                   extensions.size(), extensions.data(),
	                                   (debugging)
	                                   ? &debug_info
	                                   : nullptr);

	this->handle_ = vk::createInstance(create_info);

	this->initialised_ = true;
}

vk::SurfaceKHR RF::vulkan::instance_impl_t::create_surface(RF::window *window) const
{
	vk::ResultValue<vk::SurfaceKHR> create_surface = window->create_surface(this->handle_, nullptr);

	if (create_surface.result != vk::Result::eSuccess) throw RF::engine_error(RF::format_view("Failed to create window surface, <0>",
	                                                                          vk::to_string(create_surface.result)));

	return create_surface.value;
}

void RF::vulkan::instance_impl_t::destroy()
{
	if (!this->initialised_) throw RF::engine_error("Cannot destroy an empty RF::vulkan::instance_impl_t");

	this->handle_.destroy();

	this->initialised_ = false;
}

const vk::Instance &RF::vulkan::instance_impl_t::handle() const
{
	return this->handle_;
}
