// vulkan
#include <vulkan/vulkan.hpp>

// std
#include <string_view>

// header
#include "RF/vulkan/pipeline.hpp"

// RedFox
#include "RF/file.hpp"
#include "RF/exception.hpp"

vk::PipelineColorBlendStateCreateInfo RF::vulkan::standard_alpha_blending()
{
	vk::PipelineColorBlendAttachmentState default_blend{};
	default_blend.colorWriteMask = vk::ColorComponentFlagBits::eR
	                             | vk::ColorComponentFlagBits::eG
	                             | vk::ColorComponentFlagBits::eB
	                             | vk::ColorComponentFlagBits::eA;
	
	default_blend.blendEnable = true;
	default_blend.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
	default_blend.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
	default_blend.colorBlendOp = vk::BlendOp::eAdd;
	default_blend.srcAlphaBlendFactor = vk::BlendFactor::eOne;
	default_blend.dstAlphaBlendFactor = vk::BlendFactor::eZero;
	default_blend.alphaBlendOp = vk::BlendOp::eAdd;

	std::array<vk::PipelineColorBlendAttachmentState, 1> attachments { default_blend };
	return vk::PipelineColorBlendStateCreateInfo
	{
		{},
		false,
		vk::LogicOp::eCopy,
		attachments
	};
}

RF::vulkan::pipeline_config_t::pipeline_config_t(RF::vulkan::vertex_description_t &vd)
{
	this->vertex_input = vk::PipelineVertexInputStateCreateInfo
	{
		{},
		1,
		&vd.binding,
		static_cast<uint32_t>(vd.attributes.size()),
		vd.attributes.data()
	};

	// Default input assembly: triangles, no primitive restart
	this->input_assembly = vk::PipelineInputAssemblyStateCreateInfo
	{
		{},
		vk::PrimitiveTopology::eTriangleList,
		false
	};

	// Default viewport and scissor (dynamic)
	this->viewport_state = vk::PipelineViewportStateCreateInfo
	{
		{},
		1,
		nullptr,
		1,
		nullptr
	};

	// Default rasterization: fill mode, no culling
	this->rasterization = vk::PipelineRasterizationStateCreateInfo
	{
		{},
		false,
		false,
		vk::PolygonMode::eFill,
		vk::CullModeFlagBits::eBack,
		vk::FrontFace::eCounterClockwise,
		false,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	// Default multisampling: 1 sample, no anti-aliasing
	this->multisampling = vk::PipelineMultisampleStateCreateInfo
	{
		{}, vk::SampleCountFlagBits::e1, false
	};

	// Default color blend: no blending, write all colors
	vk::PipelineColorBlendAttachmentState default_blend{};
	default_blend.colorWriteMask = vk::ColorComponentFlagBits::eR |
	                               vk::ColorComponentFlagBits::eG |
	                               vk::ColorComponentFlagBits::eB |
	                               vk::ColorComponentFlagBits::eA;

	default_blend.blendEnable = false;
	
	this->blend_attachments = { default_blend };
	this->color_blending = vk::PipelineColorBlendStateCreateInfo
	{
		{},
		false,
		vk::LogicOp::eCopy,
		blend_attachments
	};

	// Default depth stencil state (disabled)
	this->depth_stencil = vk::PipelineDepthStencilStateCreateInfo
	{
		{},
		false,
		false,
		vk::CompareOp::eLess
	};

	// Default dynamic states (empty by default)
	this->dynamic_states = 
	{
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor,
	};

	this->dynamic_state = vk::PipelineDynamicStateCreateInfo({}, dynamic_states);
}


namespace
{
	// helper
	vk::ShaderModule create_shader_module(const vk::Device device, const std::string_view filename)
	{
		std::string code = RF::file_m::read_file(filename);

		vk::ShaderModuleCreateInfo create_info(
			vk::ShaderModuleCreateFlags(0),
			code.size(),
			reinterpret_cast<const std::uint32_t *>(code.data()),
			nullptr
		);

		return device.createShaderModule(create_info, nullptr);
	}

	// helper
	vk::PipelineShaderStageCreateInfo create_shader_stage_info(const vk::ShaderModule &shader_module, const vk::ShaderStageFlagBits type)
	{
		return
		{
			vk::PipelineShaderStageCreateFlags(0),
			type,
			shader_module,
			"main",
			nullptr,
			nullptr
		};
	}
} // anonymous namespace

void RF::vulkan::pipeline_impl_t::init(const vk::ArrayProxy<const vk::DescriptorSetLayout> &sets,
                                       const vk::ArrayProxy<const vk::PushConstantRange> &ranges,
                                       const vk::ArrayProxy<const RF::vulkan::shader_t> &shaders,
                                       const vk::Device device, const vk::RenderPass pass,
                                       const RF::vulkan::pipeline_config_t &config)
{
	if (this->initialised_) throw RF::engine_error("Cannot initialise an already initialised RF::vulkan::pipeline_impl_t");
	this->initialised_ = true;

	// initialising
	this->device_ = device;
	this->pass_ = pass;

	try
	{
		// Create pipeline layout
		vk::PipelineLayoutCreateInfo pipeline_layout_info({}, sets.size(), sets.data());
		this->layout_ = this->device_.createPipelineLayout(pipeline_layout_info);

		// Create shaders
		std::vector<vk::ShaderModule> shader_modules;
		std::vector<vk::PipelineShaderStageCreateInfo> shader_stages;

		for (auto shader : shaders)
		{
			shader_modules.push_back(create_shader_module(this->device_, shader.filename));

			shader_stages.push_back(create_shader_stage_info(shader_modules.back(), shader.type));
		}
		
		// Use provided config states
		vk::GraphicsPipelineCreateInfo pipeline_info
		(
			{}, shader_stages.size(), shader_stages.data(),
			&config.vertex_input, &config.input_assembly, nullptr,
			&config.viewport_state, &config.rasterization,
			&config.multisampling, &config.depth_stencil,
			&config.color_blending, &config.dynamic_state,
			this->layout_, this->pass_, 0, nullptr, -1
		);
		
		this->handle_ = this->device_.createGraphicsPipeline(nullptr, pipeline_info).value;

		for (auto shader_module : shader_modules) this->device_.destroyShaderModule(shader_module);
	}
	catch (vk::SystemError &err)
	{ throw RF::engine_error(RF::format_view("Failed to create Vulkan pipeline: <0>", err.what())); }
}

void RF::vulkan::pipeline_impl_t::destroy()
{
	if (!this->initialised_) throw RF::engine_error("Cannot destroy an empty RF::vulkan::pipeline_impl_t");
	this->initialised_ = false;

	this->device_.destroyPipeline(this->handle_);
	this->device_.destroyPipelineLayout(this->layout_);
}

const vk::PipelineLayout RF::vulkan::pipeline_impl_t::layout() const
{
	return this->layout_;
}

const vk::Pipeline RF::vulkan::pipeline_impl_t::handle() const
{
	return this->handle_;
}
