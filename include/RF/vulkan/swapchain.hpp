#pragma once

#include <span>
#include <vulkan/vulkan.hpp>

#include "RF/vulkan/device.hpp"
#include "RF/vulkan/render_pass.hpp"
#include "RF/handle_ptr.hpp"

namespace RF::vulkan
{
	class swapchain_impl_t;
	using swapchain_t = RF::handle_ptr<swapchain_impl_t>;

	class swapchain_impl_t
	{
	private:
		RF::vulkan::device_t device_;
		vk::SurfaceKHR surface_;
		RF::vulkan::render_pass_t pass_;
		std::vector<vk::ImageView> buffer_attachments_;

		vk::SwapchainKHR handle_;
		vk::Format format_;
		vk::Extent2D extent_;

		std::vector<vk::Image> images_;
		std::vector<vk::ImageView> image_views_;
		std::vector<vk::Framebuffer> buffers_;

		vk::Queue present_queue_;
		std::uint32_t present_family_;
	public:
		swapchain_impl_t() = default;

		// better structure:
		void init(const RF::vulkan::device_t device,
		          const vk::SurfaceKHR surface, const RF::vulkan::render_pass_t pass,
		          const std::span<vk::ImageView> &buffer_attachments);

		void destroy();

		[[nodiscard]] vk::Result submit_and_present(const vk::ArrayProxy<const vk::Semaphore> wait_semaphores,
		                                            const vk::ArrayProxy<const vk::PipelineStageFlags> wait_stages,
		                                            const vk::ArrayProxy<const vk::CommandBuffer> command_buffers,
		                                            const vk::ArrayProxy<const vk::Semaphore> signal_semaphores,
		                                            const std::uint32_t image_index,
		                                            const vk::Fence fence = nullptr) const;
		
		[[nodiscard]] vk::RenderPassBeginInfo begin_pass(const std::uint32_t image_index,
		                                                 const vk::ArrayProxy<const vk::ClearValue> clears) const;
		
		void recreate(const bool wait_idle = false);

		[[nodiscard]] const vk::SwapchainKHR &handle() const;
		[[nodiscard]] const vk::Format &format() const;
		[[nodiscard]] const vk::Extent2D &extent() const;

		[[nodiscard]] const vk::ArrayProxy<const vk::Image> images() const;
		[[nodiscard]] const vk::ArrayProxy<const vk::ImageView> image_views() const;
		[[nodiscard]] const vk::ArrayProxy<const vk::Framebuffer> buffers() const;

		[[nodiscard]] const vk::Queue &present_queue() const;
		[[nodiscard]] const std::uint32_t &present_family() const;
	};
}
