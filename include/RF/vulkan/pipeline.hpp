#pragma once

#include <vulkan/vulkan.hpp>

#include "RF/handle_ptr.hpp"
#include "RF/vulkan/common.hpp"

namespace RF::vulkan
{
	struct pipeline_config_t
	{
		vk::PipelineInputAssemblyStateCreateInfo  input_assembly;
		vk::PipelineDynamicStateCreateInfo        dynamic_state;
		vk::PipelineViewportStateCreateInfo       viewport_state;
		vk::PipelineColorBlendStateCreateInfo     color_blending;
		vk::PipelineVertexInputStateCreateInfo    vertex_input;
		vk::PipelineMultisampleStateCreateInfo    multisampling;
		vk::PipelineDepthStencilStateCreateInfo   depth_stencil;
		vk::PipelineRasterizationStateCreateInfo  rasterization;
		
		std::vector<vk::DynamicState> dynamic_states;
		std::vector<vk::PipelineColorBlendAttachmentState> blend_attachments;

		pipeline_config_t(RF::vulkan::vertex_description_t &);
	};

	vk::PipelineColorBlendStateCreateInfo standard_alpha_blending();

	struct shader_t
	{
		std::string filename;
		vk::ShaderStageFlagBits type;
	};

	class pipeline_impl_t
	{
	private:
		bool initialised_;

		vk::Device device_;
		vk::RenderPass pass_;

		vk::PipelineLayout layout_;
		vk::Pipeline handle_;
	public:
		pipeline_impl_t() = default;

		void init(const vk::ArrayProxy<const vk::DescriptorSetLayout> &sets,
		          const vk::ArrayProxy<const vk::PushConstantRange> &ranges,
		          const vk::ArrayProxy<const RF::vulkan::shader_t> &shaders,
		          const vk::Device device, const vk::RenderPass pass,
		          const RF::vulkan::pipeline_config_t &config);
		
		void destroy();

		[[nodiscard]] const vk::PipelineLayout layout() const;
		[[nodiscard]] const vk::Pipeline handle() const;
	};

	using pipeline_t = RF::handle_ptr<pipeline_impl_t>;
}
