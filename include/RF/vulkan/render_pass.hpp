#pragma once

#include "RF/handle_ptr.hpp"
#include "RF/vulkan/common.hpp"

#include <optional>
#include <vulkan/vulkan.hpp>

namespace RF::vulkan
{
	class framebuffer_impl_t;
	using framebuffer_t = RF::handle_ptr<framebuffer_impl_t>;

	struct subpass_dependency_t
	{
		vk::PipelineStageFlags stage;
		vk::AccessFlags access;
		vk::DependencyFlags flags = vk::DependencyFlagBits::eByRegion;
	};

	struct subpass_description_t
	{
		std::vector<vk::AttachmentReference> colour_attachments; 
		std::optional<vk::AttachmentReference> depth_attachment;

		vk::PipelineBindPoint bind_point;
	};

	class render_pass_impl_t
	{
	private:
		bool initialised_;

		vk::Device device_;
		RF::vulkan::pipeline_barrier_t initial_barrier_;
		RF::vulkan::pipeline_barrier_t final_barrier_;
		
		vk::RenderPass handle_;
	public:
		render_pass_impl_t() = default;

		void init(const vk::Device device,
		          const vk::ArrayProxy<const vk::AttachmentDescription> attachments,
		          const vk::ArrayProxy<const RF::vulkan::subpass_description_t> descriptions,
		          const vk::ArrayProxy<const RF::vulkan::subpass_dependency_t> dependencies,
		          const RF::vulkan::pipeline_barrier_t initial_barrier = {});

		void destroy();

		[[nodiscard]] vk::RenderPassBeginInfo create_info(const RF::vulkan::framebuffer_t framebuffer,
		                                                  const vk::ArrayProxy<const vk::ClearValue> clears) const;

		[[nodiscard]] const vk::RenderPass &handle() const;
	};

	using render_pass_t = RF::handle_ptr<render_pass_impl_t>;
}
