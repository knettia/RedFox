#pragma once

#include "RF/vulkan/device.hpp"
#include "RF/handle_ptr.hpp"
#include <vulkan/vulkan.hpp>

namespace RF::vulkan
{
	class image_impl_t;
	using image_t = RF::handle_ptr<image_impl_t>;

	class render_pass_impl_t;
	using render_pass_t = RF::handle_ptr<render_pass_impl_t>;

	class framebuffer_impl_t
	{
	private:
		bool initialised_;

		RF::vulkan::device_t device_;
		vk::Framebuffer handle_;
		
		RF::vulkan::image_t image_;
		RF::vulkan::render_pass_t pass_;
	public:
		framebuffer_impl_t() = default;

		void init(const RF::vulkan::device_t device,
		          const RF::vulkan::image_t image,
		          const vk::Extent2D extent,
		          const RF::vulkan::render_pass_t pass);

		void destroy();

		[[nodiscard]] const vk::Framebuffer &handle() const;
		[[nodiscard]] const RF::vulkan::image_t &image() const;
		[[nodiscard]] const RF::vulkan::render_pass_t &pass() const;
	};

	using framebuffer_t = RF::handle_ptr<framebuffer_impl_t>;
}
