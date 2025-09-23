#include <RF/handle_ptr.hpp>
#include <cstdint>
#include <cstring>
#include <optional>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>

#include "RF/exception.hpp"
#include "RF/vulkan/buffer.hpp"
#include "RF/vulkan/common.hpp"
#include "RF/vulkan/framebuffer.hpp"
#include "RF/vulkan/image.hpp"

void RF::vulkan::sampler_impl_t::init(const vk::SamplerCreateInfo info, const vk::Device device)
{
	if (this->initialised_) throw RF::engine_error("Cannot initialise an already initialised RF::vulkan::sampler_impl_t");
	this->initialised_ = true;

	this->device_ = device;

	this->handle_ = this->device_.createSampler(info);
}

void RF::vulkan::sampler_impl_t::destroy()
{
	if (!this->initialised_) throw RF::engine_error("Cannot destroy an empty RF::vulkan::sampler_impl_t");
	this->initialised_ = false;

	this->device_.destroySampler(this->handle_);
}

const vk::Sampler &RF::vulkan::sampler_impl_t::handle() const
{
	return this->handle_;
}

std::uint32_t find_memory_type_(std::uint32_t type_filter, vk::MemoryPropertyFlags properties, const vk::PhysicalDeviceMemoryProperties mem_properties)
{
	for (std::uint32_t i = 0; i < mem_properties.memoryTypeCount; ++i)
	{
		if (
			(type_filter & (1 << i)) &&
			(mem_properties.memoryTypes[i].propertyFlags & properties) == properties
		)
		{
			return i;
		}
	}
	throw std::runtime_error("Failed to find suitable memory type.");
}

RF::vulkan::image_config_t::image_config_t(const vk::Format format, const RF::uivec2 extent, const vk::ImageAspectFlagBits aspect)
{
	this->image = vk::ImageCreateInfo
	{
		{},
		vk::ImageType::e2D,
		format,
		vk::Extent3D(extent.x, extent.y, 1),
		1,
		1,
		vk::SampleCountFlagBits::e1,
		vk::ImageTiling::eLinear,
		vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled
	};

	this->view = vk::ImageViewCreateInfo
	{
		{},
		nullptr,
		vk::ImageViewType::e2D,
		format,
		vk::ComponentMapping{},
		vk::ImageSubresourceRange
		{
			aspect,
			0,
			1,
			0,
			1
		}
	};

	vk::BufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = aspect;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = vk::Offset3D{0, 0, 0};
	region.imageExtent = vk::Extent3D(extent.x, extent.y, 1);
	
	this->region = region;
}

RF::vulkan::image_trans_t::image_trans_t(const vk::ImageAspectFlags aspect,
                                         const vk::ImageLayout old_layout, const vk::ImageLayout new_layout,
                                         const vk::AccessFlags src_access, const vk::AccessFlags dst_access,
                                         const vk::PipelineStageFlags src_stage, const vk::PipelineStageFlags dst_stage)
{
	this->barrier = vk::ImageMemoryBarrier
	{
		{},
		{},
	};

	this->barrier.oldLayout = old_layout;
	this->barrier.newLayout = new_layout;
	this->barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	this->barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	this->barrier.subresourceRange.aspectMask = aspect;
	this->barrier.subresourceRange.baseMipLevel = 0;
	this->barrier.subresourceRange.levelCount = 1;
	this->barrier.subresourceRange.baseArrayLayer = 0;
	this->barrier.subresourceRange.layerCount = 1;

	this->barrier.srcAccessMask = src_access;
	this->barrier.dstAccessMask = dst_access;

	this->stages.src = src_stage;
	this->stages.dst = dst_stage;
}

void RF::vulkan::image_impl_t::trans(const vk::ImageLayout layout, const vk::AccessFlags access,
                                     const vk::PipelineStageFlags stage, const vk::Fence fence)
{
	// Create the temporary command buffer
	vk::CommandBufferAllocateInfo buffer_alloc
	{
		this->pool_,
		vk::CommandBufferLevel::ePrimary,
		1
	};

	auto command_buffer = device_->handle().allocateCommandBuffers(buffer_alloc).front();

	vk::CommandBufferBeginInfo begin_info
	{
		{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit }
	};

	// Transition using the temporary command buffer
	command_buffer.begin(begin_info);

	auto barrier = vk::ImageMemoryBarrier
	{
		{},
		{},
	};

	barrier.oldLayout = this->barrier_data_.layout;
	barrier.newLayout = layout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = this->barrier_data_.aspect;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	barrier.srcAccessMask = this->barrier_data_.access;
	barrier.dstAccessMask = access;

	barrier.image = this->handle_;

	command_buffer.pipelineBarrier(
		this->barrier_data_.stage,
		stage,
		{},
		nullptr,
		nullptr,
		barrier
	);

	command_buffer.end();

	// Submit the temporary command buffer
	std::array<vk::CommandBuffer, 1> temp_buf = {command_buffer};
	vk::SubmitInfo submit_info
	{
		nullptr,
		nullptr,
		temp_buf,
		nullptr
	};

	this->device_->graphics_queue().submit(submit_info, fence);
	this->device_->graphics_queue().waitIdle();

	// Free the temporary command buffer.
	device_->handle().freeCommandBuffers(this->pool_, command_buffer);

	// Update data
	this->barrier_data_.layout = layout;
	this->barrier_data_.access = access;
	this->barrier_data_.stage = stage;
}

void RF::vulkan::image_impl_t::init(const RF::vulkan::image_config_t config,
                                    const RF::vulkan::device_t device, const vk::CommandPool pool)
{
	if (this->initialised_) throw RF::engine_error("Cannot initialise an already initialised RF::vulkan::image_impl_t");
	this->initialised_ = true;

	this->device_ = device;
	this->pool_ = pool;
	this->config_ = config;

	this->barrier_data_.aspect = config.view.subresourceRange.aspectMask;

	// Create the image without initializing it with data.
	this->handle_ = device_->handle().createImage(config.image);

	vk::MemoryRequirements mem_requirements = device_->handle().getImageMemoryRequirements(this->handle_);

	vk::MemoryAllocateInfo memory_alloc
	{
		mem_requirements.size,
		find_memory_type_
		(
			mem_requirements.memoryTypeBits,
			vk::MemoryPropertyFlagBits::eDeviceLocal,
			device->gpu().getMemoryProperties()
		)
	};

	memory_ = device_->handle().allocateMemory(memory_alloc);
	device_->handle().bindImageMemory(this->handle_, this->memory_, 0);

	vk::ImageViewCreateInfo view_info = config.view;
	view_info.image = this->handle_;

	view_ = device_->handle().createImageView(view_info);
}

void RF::vulkan::image_impl_t::copy(const std::byte *data, const std::uint64_t n)
{
	RF::vulkan::buffer_t staging_buffer;
	
	staging_buffer->init(this->device_,
	                     vk::BufferUsageFlagBits::eTransferSrc,
	                     n, true);
	staging_buffer->copy(data, this->pool_);

	vk::CommandBufferAllocateInfo buffer_alloc
	{
		this->pool_,
		vk::CommandBufferLevel::ePrimary,
		1
	};
	
	vk::CommandBuffer command_buffer = this->device_->handle().allocateCommandBuffers(buffer_alloc).front();

	vk::CommandBufferBeginInfo begin_info
	{
		{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit }
	};

	command_buffer.begin(begin_info);

	// Copy buffer data to image
	command_buffer.copyBufferToImage(staging_buffer->handle(), this->handle_, vk::ImageLayout::eTransferDstOptimal, this->config_.region);
	
	// End and submit the command buffer
	command_buffer.end();

	std::array<vk::CommandBuffer, 1> temp_buf = {command_buffer};
	vk::SubmitInfo submit_info
	{
		nullptr,
		nullptr,
		temp_buf,
		nullptr
	};

	this->device_->graphics_queue().submit(submit_info, nullptr);
	this->device_->graphics_queue().waitIdle();

	// Cleanup temporary command buffer
	this->device_->handle().freeCommandBuffers(pool_, command_buffer);

	staging_buffer->destroy();
}

std::vector<std::uint8_t> RF::vulkan::image_impl_t::data()
{
	// auto old_barrier_data = this->barrier_data_;

	// vk::DeviceSize image_size = this->config_.image.extent.width * this->config_.image.extent.height * 4; // Assuming 4 bytes per pixel (RGBA8)

	// // Step 1: Create staging buffer
	// vk::BufferCreateInfo buffer_info
	// {
	// 	{},
	// 	image_size,
	// 	vk::BufferUsageFlagBits::eTransferDst,
	// 	vk::SharingMode::eExclusive
	// };
	// vk::Buffer staging_buffer = this->device_.createBuffer(buffer_info);

	// vk::MemoryRequirements mem_reqs = this->device_.getBufferMemoryRequirements(staging_buffer);
	// std::uint32_t mem_type_index = find_memory_type_(
	// 	mem_reqs.memoryTypeBits,
	// 	vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
	// 	this->gpu_.getMemoryProperties()
	// );

	// vk::CommandBufferAllocateInfo buffer_alloc
	// {
	// 	this->pool_,
	// 	vk::CommandBufferLevel::ePrimary,
	// 	1
	// };

	// vk::MemoryAllocateInfo alloc_info
	// {
	// 	mem_reqs.size,
	// 	mem_type_index
	// };

	// vk::DeviceMemory staging_memory = this->device_.allocateMemory(alloc_info);
	// this->device_.bindBufferMemory(staging_buffer, staging_memory, 0);

	// // Step 2: Transition image layout
	// vk::CommandBuffer command_buffer = this->device_.allocateCommandBuffers(buffer_alloc).front();

	// vk::CommandBufferBeginInfo begin_info
	// {
	// 	{ vk::CommandBufferUsageFlagBits::eOneTimeSubmit }
	// };

	// command_buffer.begin(begin_info);

	// vk::ImageMemoryBarrier barrier
	// {
	// 	vk::AccessFlagBits::eMemoryRead,
	// 	vk::AccessFlagBits::eTransferRead,
	// 	vk::ImageLayout::eGeneral,
	// 	vk::ImageLayout::eTransferSrcOptimal,
	// 	VK_QUEUE_FAMILY_IGNORED,
	// 	VK_QUEUE_FAMILY_IGNORED,
	// 	this->handle_,
	// 	{
	// 		vk::ImageAspectFlagBits::eDepth,
	// 		0, 1, 0, 1
	// 	}
	// };

	// command_buffer.pipelineBarrier(
	// 	vk::PipelineStageFlagBits::eAllCommands,
	// 	vk::PipelineStageFlagBits::eTransfer,
	// 	{},
	// 	nullptr, nullptr,
	// 	barrier
	// );

	// // Step 3: Copy image to buffer
	// vk::BufferImageCopy copy_region
	// {
	// 	0,
	// 	0, 0,
	// 	{
	// 		vk::ImageAspectFlagBits::eDepth,
	// 		0, 0, 1
	// 	},
	// 	{ 0, 0, 0 },
	// 	{ this->config_.image.extent.width, this->config_.image.extent.height, 1 }
	// };

	// command_buffer.copyImageToBuffer(this->handle_, vk::ImageLayout::eTransferSrcOptimal, staging_buffer, copy_region);

	// command_buffer.end();

	// std::array<vk::CommandBuffer, 1> temp_buf = {command_buffer};
	// vk::SubmitInfo submit_info
	// {
	// 	nullptr,
	// 	nullptr,
	// 	temp_buf,
	// 	nullptr
	// };

	// vk::Queue graphics_queue = this->device_.getQueue(this->graphics_, 0);
	// graphics_queue.submit(submit_info, nullptr);
	// graphics_queue.waitIdle();

	// // Cleanup temporary command buffer
	// this->device_.freeCommandBuffers(pool_, command_buffer);

	// // Step 4: Map and save
	// void *data = this->device_.mapMemory(staging_memory, 0, image_size);
	std::vector<std::uint8_t> vector_data(1);

	// std::memcpy(vector_data.data(), data, image_size);

	// // stbi_write_png("hello", this->config_.image.extent.width, this->config_.image.extent.height, 4, data, this->config_.image.extent.width * 4);
	// this->device_.unmapMemory(staging_memory);

	// // Step 5: Cleanup
	// this->device_.destroyBuffer(staging_buffer);
	// this->device_.freeMemory(staging_memory);

	return vector_data;
}

void RF::vulkan::image_impl_t::destroy()
{
	if (!this->initialised_) throw RF::engine_error("Cannot destroy an empty RF::vulkan::image_impl_t");
	this->initialised_ = false;

	this->device_->handle().destroyImageView(this->view_);
	this->device_->handle().freeMemory(this->memory_);
	this->device_->handle().destroyImage(this->handle_);
}

const vk::Extent2D RF::vulkan::image_impl_t::extent() const
{
	return {this->config_.image.extent.width, this->config_.image.extent.height };
}

const vk::Image &RF::vulkan::image_impl_t::handle() const
{
	return this->handle_;
}

const vk::ImageView &RF::vulkan::image_impl_t::view() const
{
	return this->view_;
}

const RF::vulkan::pipeline_barrier_t &RF::vulkan::image_impl_t::barrier() const
{
	return this->barrier_data_;
}

void RF::vulkan::image_impl_t::update_barrier(const RF::vulkan::pipeline_barrier_t barrier)
{
	this->barrier_data_ = barrier;
}

RF::vulkan::framebuffer_t RF::vulkan::image_impl_t::create_framebuffer(const RF::vulkan::render_pass_t pass) const
{
	RF::vulkan::framebuffer_t framebuffer;

	framebuffer->init(this->device_,
	                  RF::handle_ptr<image_impl_t>(*this), this->extent(), pass);

	return framebuffer;
}
