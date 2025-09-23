#pragma once

#include "RF/handle_ptr.hpp"
#include "RF/vulkan/common.hpp"

#include "RF/maths/vec/vec2.hpp"
#include "RF/vulkan/device.hpp"
#include "RF/vulkan/framebuffer.hpp"
#include "RF/vulkan/render_pass.hpp"

#include <vulkan/vulkan.hpp>

namespace RF::vulkan
{
	class sampler_impl_t
	{
	private:
		bool initialised_;

		vk::Device device_;
		vk::Sampler handle_;
	public:
		sampler_impl_t() = default;
		void init(const vk::SamplerCreateInfo info,
		          const vk::Device device);

		void destroy();

		[[nodiscard]] const vk::Sampler &handle() const;
	};

	struct image_config_t
	{
		vk::ImageCreateInfo image;
		vk::ImageViewCreateInfo view;
		vk::BufferImageCopy region;

		image_config_t() = default;

		image_config_t(const vk::Format format, const RF::uivec2 extent,
		               const vk::ImageAspectFlagBits aspect);
	};

	struct image_trans_t
	{
		vk::ImageMemoryBarrier barrier;
		struct
		{
			vk::PipelineStageFlags src;
			vk::PipelineStageFlags dst;
		} stages;

		image_trans_t() = default;

		image_trans_t(const vk::ImageAspectFlags aspect,
		              const vk::ImageLayout old_layout, const vk::ImageLayout new_layout,
			      const vk::AccessFlags src_access, const vk::AccessFlags dst_access,
		              const vk::PipelineStageFlags src_stage, const vk::PipelineStageFlags dst_stage);
	};

	class image_impl_t
	{
	private:
		bool initialised_;

		RF::vulkan::device_t device_;
		vk::CommandPool pool_;

		vk::Image handle_;
		vk::ImageView view_;
		vk::DeviceMemory memory_;
		RF::vulkan::image_config_t config_;

		RF::vulkan::pipeline_barrier_t barrier_data_;

		void transition_image_layout_(vk::CommandBuffer command_buffer, vk::ImageLayout old_layout, vk::ImageLayout new_layout);
	public:
		image_impl_t() = default;

		void init(const RF::vulkan::image_config_t config,
		          const RF::vulkan::device_t device, const vk::CommandPool pool);

		void destroy();

		std::vector<std::uint8_t> data();

		void copy(const std::byte *src, const std::uint64_t n);

		void trans(const RF::vulkan::image_trans_t trans);

		void trans(const vk::ImageLayout layout, const vk::AccessFlags access,
		           const vk::PipelineStageFlags stage, const vk::Fence fence = nullptr);

		[[nodiscard]] const RF::vulkan::pipeline_barrier_t &barrier() const;
		void update_barrier(const RF::vulkan::pipeline_barrier_t barrier);
		
		[[nodiscard]] const vk::Extent2D extent() const;
		[[nodiscard]] const vk::Image &handle() const;
		[[nodiscard]] const vk::ImageView &view() const;

		[[nodiscard]] RF::vulkan::framebuffer_t create_framebuffer(const RF::vulkan::render_pass_t pass) const;
	};

	using image_t = RF::handle_ptr<image_impl_t>;
	using sampler_t = RF::handle_ptr<sampler_impl_t>;
} // namespace RF::vulkan
