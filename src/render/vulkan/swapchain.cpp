#include "vulkan/vulkan_core.h"
#define NOMINMAX
#include <vulkan/vulkan.hpp>

#include "RF/exception.hpp"
#include "RF/vulkan/swapchain.hpp"

namespace
{
	vk::SurfaceFormatKHR find_surface_format(const std::span<vk::SurfaceFormatKHR> available)
	{
		for (const auto &surface_format : available)
		{
			if (surface_format.format == vk::Format::eB8G8R8A8Srgb && surface_format.colorSpace == vk::ColorSpaceKHR::eVkColorspaceSrgbNonlinear) return surface_format;
		}
	
		return available[0];
	}
	
	vk::PresentModeKHR find_present_mode(const std::span<vk::PresentModeKHR> available)
	{
		for (const auto &present_mode : available)
		{
			if (present_mode == vk::PresentModeKHR::eMailbox) return present_mode;
		}
	
		return vk::PresentModeKHR::eFifo;
	}

	std::optional<std::uint32_t> find_present_family(const vk::PhysicalDevice gpu, const vk::SurfaceKHR surface)
	{
		std::optional<std::uint32_t> present_family_index;

		std::vector<vk::QueueFamilyProperties> queue_families = gpu.getQueueFamilyProperties();
		for (std::uint32_t i = 0; i < queue_families.size(); ++i)
		{
			if (gpu.getSurfaceSupportKHR(i, surface))
			{
				present_family_index = i;
				break;
			}
		}

		return present_family_index;
	}

}

void RF::vulkan::swapchain_impl_t::init(const RF::vulkan::device_t device,
		                        const vk::SurfaceKHR surface, const RF::vulkan::render_pass_t pass,
		                        const std::span<vk::ImageView> &buffer_attachments)
{
	this->device_ = device;
	this->surface_ = surface;
	this->pass_ = pass;
	this->buffer_attachments_ = std::vector<vk::ImageView>(buffer_attachments.begin(),
	                                                       buffer_attachments.end());

	std::optional<uint32_t> present_family_index = find_present_family(this->device_->gpu(), surface);

	if (present_family_index.has_value()) this->present_family_ = std::move(present_family_index.value());
	else throw RF::engine_error("No absolute presentation family queue found for GPU");

	this->present_queue_ = this->device_->handle().getQueue(this->present_family_, 0);

	vk::SurfaceCapabilitiesKHR capabilities = this->device_->gpu().getSurfaceCapabilitiesKHR(surface);
	std::vector<vk::SurfaceFormatKHR> formats = this->device_->gpu().getSurfaceFormatsKHR(surface);
	std::vector<vk::PresentModeKHR> present_modes = this->device_->gpu().getSurfacePresentModesKHR(surface);

	vk::SurfaceFormatKHR surface_format = find_surface_format(formats);
	vk::PresentModeKHR present_mode = find_present_mode(present_modes);
	vk::Extent2D extent;

	extent = capabilities.currentExtent;

	std::uint32_t image_count = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount) image_count = capabilities.maxImageCount;

	vk::SwapchainCreateInfoKHR create_info(
		vk::SwapchainCreateFlagsKHR(0),
		this->surface_,
		image_count,
		surface_format.format,
		surface_format.colorSpace,
		extent,
		1,
		vk::ImageUsageFlags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
	);

	if (this->device_->graphics_family() != this->present_family_)
	{
		std::vector<std::uint32_t> queue_family_indices
		{
			this->device_->graphics_family(),
			this->present_family_
		};

		create_info.imageSharingMode = vk::SharingMode::eConcurrent;
		create_info.queueFamilyIndexCount = queue_family_indices.size();
		create_info.pQueueFamilyIndices = queue_family_indices.data();
	}
	else create_info.imageSharingMode = vk::SharingMode::eExclusive;

	create_info.preTransform = capabilities.currentTransform;
	create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR(VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR);
	create_info.presentMode = present_mode;
	create_info.clipped = true;

	create_info.oldSwapchain = VK_NULL_HANDLE;

	try
	{
		this->handle_ = this->device_->handle().createSwapchainKHR(create_info, nullptr);

		this->extent_ = extent;
		this->format_ = surface_format.format;

		this->images_ = this->device_->handle().getSwapchainImagesKHR(this->handle_);
	}
	catch (vk::SystemError &error) { throw RF::engine_error(error.what()); }

	// handle image views
	this->image_views_.resize(this->images_.size());

	try
	{
		for (std::size_t i = 0; i < this->images_.size(); i++)
		{
			vk::ComponentMapping component(vk::ComponentSwizzle::eIdentity,
			                               vk::ComponentSwizzle::eIdentity,
			                               vk::ComponentSwizzle::eIdentity,
			                               vk::ComponentSwizzle::eIdentity);

			vk::ImageSubresourceRange subresource_range(vk::ImageAspectFlagBits::eColor,
			                                            0, 1,
			                                            0, 1);

			vk::ImageViewCreateInfo create_info(
				{},
				this->images_[i],
				vk::ImageViewType::e2D,
				surface_format.format,
				component,
				subresource_range
			);

			this->image_views_[i] = this->device_->handle().createImageView(create_info, nullptr);
		}
	}
	catch (vk::SystemError &error) { throw RF::engine_error(error.what()); }

	// handle framebuffers
	this->buffers_.resize(this->image_views_.size());
	
	try
	{
		for (std::size_t i = 0; i < this->image_views_.size(); i++)
		{
			std::vector<vk::ImageView> attachments
			{
				this->image_views_[i]
			};

			for (const vk::ImageView &image_view : buffer_attachments) attachments.push_back(image_view);

			vk::FramebufferCreateInfo framebuffer_info(vk::FramebufferCreateFlags(0),
			                                           this->pass_->handle(),
			                                           attachments.size(),
			                                           attachments.data(),
			                                           this->extent_.width,
			                                           this->extent_.height,
			                                           1, nullptr);

			this->buffers_[i] = this->device_->handle().createFramebuffer(framebuffer_info, nullptr);
		}
	}
	catch (vk::SystemError &error) { throw RF::engine_error(error.what()); }
}

vk::RenderPassBeginInfo RF::vulkan::swapchain_impl_t::begin_pass(const std::uint32_t image_index,
                                                                 const vk::ArrayProxy<const vk::ClearValue> clears) const
{
	if (image_index >= this->buffers_.size())
	throw RF::engine_error(RF::format_view("Image index <0> out of bounds for swapchain framebuffers of size <1>", image_index, this->buffers_.size()));

	vk::RenderPassBeginInfo info
	{
		this->pass_->handle(),
		this->buffers_[image_index],
		vk::Rect2D({0, 0}, this->extent_),
		static_cast<std::uint32_t>(clears.size()),
		clears.data()
	};

	return info;
}

void RF::vulkan::swapchain_impl_t::destroy()
{
	for (const vk::ImageView &image_view : this->image_views_) this->device_->handle().destroyImageView(image_view, nullptr);

	for (const vk::Framebuffer &framebuffer : this->buffers_) this->device_->handle().destroyFramebuffer(framebuffer, nullptr);

	this->device_->handle().destroySwapchainKHR(this->handle_);
}

vk::Result RF::vulkan::swapchain_impl_t::submit_and_present(const vk::ArrayProxy<const vk::Semaphore> wait_semaphores,
                                                            const vk::ArrayProxy<const vk::PipelineStageFlags> wait_stages,
                                                            const vk::ArrayProxy<const vk::CommandBuffer> command_buffers,
                                                            const vk::ArrayProxy<const vk::Semaphore> signal_semaphores,
                                                            const std::uint32_t image_index, const vk::Fence fence) const
{
	vk::SubmitInfo submit_info(wait_semaphores.size(),
	                           wait_semaphores.data(),
	                           wait_stages.data(),
	                           command_buffers.size(),
	                           command_buffers.data(),
	                           signal_semaphores.size(),
	                           signal_semaphores.data());

	auto result = this->device_->graphics_queue().submit(1, &submit_info, fence);

	if (result != vk::Result::eSuccess) return result;

	vk::PresentInfoKHR present_info(signal_semaphores.size(),
	                                signal_semaphores.data(),
	                                1, &this->handle_,
	                                &image_index,
	                                nullptr);

	return this->present_queue_.presentKHR(present_info);
}

void RF::vulkan::swapchain_impl_t::recreate(const bool wait_idle)
{
	if (wait_idle) this->device_->handle().waitIdle();

	this->destroy();

	this->init(this->device_, this->surface_, this->pass_, this->buffer_attachments_);
}

const vk::SwapchainKHR &RF::vulkan::swapchain_impl_t::handle() const
{
	return this->handle_;
}

const vk::Format &RF::vulkan::swapchain_impl_t::format() const
{
	return this->format_;
}

const vk::Extent2D &RF::vulkan::swapchain_impl_t::extent() const
{
	return this->extent_;
}

const vk::ArrayProxy<const vk::Image> RF::vulkan::swapchain_impl_t::images() const
{
	return this->images_;
}

const vk::ArrayProxy<const vk::ImageView> RF::vulkan::swapchain_impl_t::image_views() const
{
	return this->image_views_;
}

const vk::ArrayProxy<const vk::Framebuffer> RF::vulkan::swapchain_impl_t::buffers() const
{
	return this->buffers_;
}

const vk::Queue &RF::vulkan::swapchain_impl_t::present_queue() const
{
	return this->present_queue_;
}

const std::uint32_t &RF::vulkan::swapchain_impl_t::present_family() const
{
	return this->present_family_;
}

