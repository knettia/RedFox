#include "RF/vulkan/framebuffer.hpp"
#include "RF/vulkan/image.hpp"

// header
#include "RF/vulkan/render_pass.hpp"

// RedFox
#include "RF/exception.hpp"
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_to_string.hpp>

void RF::vulkan::render_pass_impl_t::init(const vk::Device device,
                                          const vk::ArrayProxy<const vk::AttachmentDescription> attachments_proxy,
                                          const vk::ArrayProxy<const RF::vulkan::subpass_description_t> descriptions_proxy,
                                          const vk::ArrayProxy<const RF::vulkan::subpass_dependency_t> dependencies_proxy,
					  const RF::vulkan::pipeline_barrier_t initial_barrier)
{
	if (this->initialised_)
	throw RF::engine_error("Cannot initialise an already initialised RF::vulkan::render_pass_impl_t");

	auto attachments = std::vector<vk::AttachmentDescription>(attachments_proxy.begin(), attachments_proxy.end());
	auto descriptions = std::vector<RF::vulkan::subpass_description_t>(descriptions_proxy.begin(), descriptions_proxy.end());
	auto dependencies = std::vector<RF::vulkan::subpass_dependency_t>(dependencies_proxy.begin(), dependencies_proxy.end());

	this->device_ = device;
	this->initial_barrier_ = initial_barrier;
	this->final_barrier_.layout = attachments.front().finalLayout;

	std::string quantifier = "Too ";

	if (dependencies.size() < descriptions.size()) quantifier += "little";
	else if (dependencies.size() > descriptions.size() + 1) quantifier += "many";

	if (quantifier.size() != 4)
	throw RF::engine_error("<0> dependencies for render pass. expected <1> or <2>, got <3>",
	                       quantifier,
	                       descriptions.size(), descriptions.size() + 1,
	                       dependencies.size());

	std::vector<vk::SubpassDependency> subpass_dependencies;
	std::vector<vk::SubpassDescription> subpass_descriptions;
	std::vector<std::vector<vk::AttachmentReference>> colour_attachment_storage;
	std::vector<vk::AttachmentReference> depth_attachment_storage;

	for (auto &description_data : descriptions)
	{
		auto &colour_attachments = colour_attachment_storage.emplace_back(description_data.colour_attachments);
		auto &depth_attachment = description_data.depth_attachment;

		vk::SubpassDescription description;
		description.pipelineBindPoint = description_data.bind_point;
		description.colorAttachmentCount = colour_attachments.size();
		description.pColorAttachments = colour_attachments.data();

		if (depth_attachment.has_value())
		{
			depth_attachment_storage.push_back(depth_attachment.value());
			description.pDepthStencilAttachment = &depth_attachment_storage.back();
		}
		else description.pDepthStencilAttachment = nullptr;

		subpass_descriptions.push_back(description);
	}

	for (std::size_t i = 0; i < dependencies.size(); i++)
	{
		vk::SubpassDependency dependency;

		if (i == 0) // first dependency
		{
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = this->initial_barrier_.stage;
			dependency.srcAccessMask = this->initial_barrier_.access;
			dependency.dstStageMask = dependencies[i].stage;
			dependency.dstAccessMask = dependencies[i].access;
		}
		else if (i == dependencies.size() - 1) // last dependency
		{
			dependency.srcSubpass = i - 1;
			dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
			dependency.srcStageMask = dependencies[i-1].stage;
			dependency.srcAccessMask = dependencies[i-1].access;
			dependency.dstStageMask = dependencies[i].stage;
			dependency.dstAccessMask = dependencies[i].access;

			this->final_barrier_.stage = dependencies[i].stage;
			this->final_barrier_.access = dependencies[i].access;
		}
		else // any other
		{
			dependency.srcSubpass = i - 1;
			dependency.dstSubpass = i;
			dependency.srcStageMask = dependencies[i-1].stage;
			dependency.srcAccessMask = dependencies[i-1].access;
			dependency.dstStageMask = dependencies[i].stage;
			dependency.dstAccessMask = dependencies[i].access;
		}

		dependency.dependencyFlags = dependencies[i].flags;

		subpass_dependencies.push_back(dependency);
	}

	vk::RenderPassCreateInfo create_info;
	create_info.attachmentCount = attachments.size();
	create_info.pAttachments = attachments.data();

	create_info.subpassCount = subpass_descriptions.size();
	create_info.pSubpasses = subpass_descriptions.data();

	create_info.dependencyCount = subpass_dependencies.size();
	create_info.pDependencies = subpass_dependencies.data();

	this->handle_ = this->device_.createRenderPass(create_info);

	this->initialised_ = true;
}

vk::RenderPassBeginInfo RF::vulkan::render_pass_impl_t::create_info(const RF::vulkan::framebuffer_t framebuffer,
                                                                    const vk::ArrayProxy<const vk::ClearValue> clears) const
{
	auto image = framebuffer->image();
	auto image_barrier = image->barrier();

	// EXPLANATION: compare barrier to initial barrier,
	//              if we do care about a specific field,
	//              then we must ensure the fields match
	if (this->initial_barrier_.aspect != vk::ImageAspectFlagBits::eNone) // we care about the aspect
	{
		if (this->initial_barrier_.aspect != image_barrier.aspect)
		throw RF::engine_error("Render pass' initial barrier aspect `<0>` and the framebuffer's current barrier aspect `<1>` do not match. Failed to create a safe and legal vk::RenderPassCreateInfo",
		                       vk::to_string(this->initial_barrier_.aspect), vk::to_string(image_barrier.aspect));
	}

	if (this->initial_barrier_.layout != vk::ImageLayout::eUndefined) // we care about the layout
	{
		if (this->initial_barrier_.layout != image_barrier.layout)
		throw RF::engine_error("Render pass' initial barrier layout `<0>` and the framebuffer's current barrier layout `<1>` do not match. Failed to create a safe and legal vk::RenderPassCreateInfo",
		                       vk::to_string(this->initial_barrier_.layout), vk::to_string(image_barrier.layout));
	}

	if (this->initial_barrier_.access) // we care about the access
	{
		if (this->initial_barrier_.access != image_barrier.access)
		throw RF::engine_error("Render pass' initial barrier access `<0>` and the framebuffer's current barrier access `<1>` do not match. Failed to create a safe and legal vk::RenderPassCreateInfo",
		                       vk::to_string(this->initial_barrier_.access), vk::to_string(image_barrier.access));
	}

	if (this->initial_barrier_.stage != vk::PipelineStageFlagBits::eTopOfPipe) // we care about the stage
	{
		if (this->initial_barrier_.stage != image_barrier.stage)
		throw RF::engine_error("Render pass' initial barrier stage `<0>` and the framebuffer's current barrier stage `<1>` do not match. Failed to create a safe and legal vk::RenderPassCreateInfo",
		                       vk::to_string(this->initial_barrier_.stage), vk::to_string(image_barrier.stage));
	}

	vk::RenderPassBeginInfo info
	{
		this->handle_,
		framebuffer->handle(),
		{ { 0, 0 }, image->extent()},
		static_cast<std::uint32_t>(clears.size()),
		clears.data()
	};

	auto modified_barrier = final_barrier_;
	modified_barrier.aspect = image_barrier.aspect;

	image->update_barrier(modified_barrier); // update the image's barrier

	return info;
}

void RF::vulkan::render_pass_impl_t::destroy()
{
	if (!this->initialised_) throw RF::engine_error("Cannot destroy an empty RF::vulkan::render_pass_impl_t");
	this->initialised_ = false;

	this->device_.destroyRenderPass(this->handle_);
}

const vk::RenderPass &RF::vulkan::render_pass_impl_t::handle() const
{
	return this->handle_;
}
