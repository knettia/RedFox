// header
#include "RF/vulkan/buffer.hpp"

// RedFox
#include "RF/exception.hpp"

std::uint32_t RF::vulkan::buffer_impl_t::memory_type_(std::uint32_t memory_type_bits, vk::MemoryPropertyFlags required_flags)
{
	vk::PhysicalDeviceMemoryProperties mem_properties = this->device_->gpu().getMemoryProperties();

	std::uint32_t memory_type_index = 0;
	for (std::uint32_t i = 0; i < mem_properties.memoryTypeCount; ++i)
	{
		if (
			(memory_type_bits & (1 << i)) && 
			(mem_properties.memoryTypes[i].propertyFlags & 
			(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent))
		)
		{ return i; }
	}

	throw RF::engine_error("Failed to find memory type.");
}

void RF::vulkan::buffer_impl_t::create_buffer_(vk::Buffer &buffer, vk::DeviceMemory &memory,
                                          vk::DeviceSize size, vk::BufferUsageFlags usage,
                                          vk::MemoryPropertyFlags properties)
{
	vk::BufferCreateInfo buffer_info({}, size, usage, vk::SharingMode::eExclusive);
	buffer = this->device_->handle().createBuffer(buffer_info);

	vk::MemoryRequirements mem_requirements = this->device_->handle().getBufferMemoryRequirements(buffer);

	vk::MemoryAllocateInfo alloc_info(mem_requirements.size, memory_type_(mem_requirements.memoryTypeBits, properties));

	memory = this->device_->handle().allocateMemory(alloc_info);
	this->device_->handle().bindBufferMemory(buffer, memory, 0);
}

void RF::vulkan::buffer_impl_t::copy_buffer_(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size, const std::optional<vk::CommandPool> pool)
{
	vk::CommandBufferAllocateInfo alloc_info(pool.value(), vk::CommandBufferLevel::ePrimary, 1);
	vk::CommandBuffer command_buffer = this->device_->handle().allocateCommandBuffers(alloc_info).at(0);

	vk::CommandBufferBeginInfo begin_info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	command_buffer.begin(begin_info);

	vk::BufferCopy copy_region(0, 0, size);
	command_buffer.copyBuffer(src, dst, copy_region);

	command_buffer.end();

	vk::Queue graphics_queue = this->device_->handle().getQueue(0, 0);
	vk::SubmitInfo submit_info({}, {}, {}, 1, &command_buffer);

	graphics_queue.submit(submit_info);
	graphics_queue.waitIdle();

	this->device_->handle().freeCommandBuffers(pool.value(), command_buffer);
}

void RF::vulkan::buffer_impl_t::init(const RF::vulkan::device_t device,
                                     const vk::BufferUsageFlags usage,
                                     const std::uint64_t n, const bool stage)
{
	if (this->initialised_) throw RF::engine_error("Cannot initialise an already initialised RF::vulkan::buffer_impl_t");
	this->initialised_ = true;

	this->size_ = n;
	this->device_ = device;
	this->staged_ = stage;

	vk::BufferUsageFlags final_usage = usage;
	if (stage) final_usage |= vk::BufferUsageFlagBits::eTransferDst;

	create_buffer_(
		this->handle_,
		this->memory_,
		size_,
		final_usage,
		vk::MemoryPropertyFlagBits::eDeviceLocal
	);
}

void RF::vulkan::buffer_impl_t::destroy()
{
	if (!this->initialised_) throw RF::engine_error("Cannot destroy an empty RF::vulkan::buffer_impl_t");
	this->initialised_ = false;

	this->device_->handle().destroyBuffer(this->handle_);
	this->device_->handle().freeMemory(this->memory_);
}

void RF::vulkan::buffer_impl_t::copy(const void *src, const std::optional<vk::CommandPool> pool)
{
	if (this->staged_)
	{
		if (!pool.has_value()) throw RF::engine_error("No Vulkan command pool provided for staging buffer. Cannot copy to VRAM.");

		vk::Buffer staging_buffer;
		vk::DeviceMemory staging_memory;

		create_buffer_(
			staging_buffer,
			staging_memory,
			size_,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		auto staging_data = this->device_->handle().mapMemory(staging_memory, 0, size_);
		::memcpy(staging_data, src, size_);
		this->device_->handle().unmapMemory(staging_memory);

		copy_buffer_(staging_buffer, this->handle_, size_, pool.value());
		this->device_->handle().destroyBuffer(staging_buffer);
		this->device_->handle().freeMemory(staging_memory);
	}
	else
	{
		this->data_ = this->device_->handle().mapMemory(this->memory_, 0, size_);
		::memcpy(this->data_, src, size_);
		this->device_->handle().unmapMemory(this->memory_);
	}
}

const bool RF::vulkan::buffer_impl_t::staged() const
{
	return this->staged_;
}

const vk::Buffer &RF::vulkan::buffer_impl_t::handle() const
{
	return this->handle_;
}

const vk::DeviceMemory &RF::vulkan::buffer_impl_t::memory() const
{
	return this->memory_;
}

const vk::DeviceSize &RF::vulkan::buffer_impl_t::size() const
{
	return this->size_;
}

const void *RF::vulkan::buffer_impl_t::data() const
{
	return this->data_;
}

void RF::vulkan::super_buffer_impl_t::init(const RF::vulkan::device_t device,
                                           vk::BufferUsageFlags usage,
                                           std::size_t element_size,
                                           std::size_t count,
                                           bool stage)
{
	// Query alignment requirement
	vk::PhysicalDeviceProperties props = device->gpu().getProperties();
	std::size_t alignment = props.limits.minUniformBufferOffsetAlignment;

	// Align the stride
	stride_ = (element_size + alignment - 1) & ~(alignment - 1);
	count_ = count;
	element_size_ = element_size;

	// Allocate a buffer large enough for all slots
	std::size_t total_size = stride_ * count_;
	buffer_impl_t::init(device, usage, total_size, stage);
}

void RF::vulkan::super_buffer_impl_t::destroy()
{
	buffer_impl_t::destroy();
}

// void RF::vulkan::super_buffer_impl_t::copy(const std::size_t index, const void *src,
//                                            const std::optional<vk::CommandPool> pool)
// {
// 	if (index >= count_) throw RF::engine_error("super_buffer_impl_t::copy out of range");

// 	// Compute destination offset
// 	std::size_t dst_offset = stride_ * index;

// 	// For staging: we need to allocate a temporary buffer and copy into the region
// 	if (staged())
// 	{
// 		// Copy into a temporary buffer and blit into the correct region
// 		// Slightly different to buffer_impl_t::copy (which assumes full overwrite)
// 		vk::Buffer staging_buffer;
// 		vk::DeviceMemory staging_memory;

// 		create_buffer_(
// 			staging_buffer,
// 			staging_memory,
// 			stride_,
// 			vk::BufferUsageFlagBits::eTransferSrc,
// 			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
// 		);

// 		auto staging_data = this->device_->handle().mapMemory(staging_memory, 0, stride_);
// 		::memcpy(staging_data, src, stride_);
// 		this->device_->handle().unmapMemory(staging_memory);

// 		// Record and submit copy into the correct region
// 		vk::CommandBufferAllocateInfo alloc_info(pool.value(), vk::CommandBufferLevel::ePrimary, 1);
// 		vk::CommandBuffer command_buffer = this->device_->handle().allocateCommandBuffers(alloc_info).at(0);

// 		vk::CommandBufferBeginInfo begin_info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
// 		command_buffer.begin(begin_info);

// 		vk::BufferCopy region(0, dst_offset, stride_);
// 		command_buffer.copyBuffer(staging_buffer, this->handle(), region);

// 		command_buffer.end();

// 		vk::Queue graphics_queue = this->device_->handle().getQueue(0, 0);
// 		vk::SubmitInfo submit_info({}, {}, {}, 1, &command_buffer);
// 		graphics_queue.submit(submit_info);
// 		graphics_queue.waitIdle();

// 		this->device_->handle().freeCommandBuffers(pool.value(), command_buffer);
// 		this->device_->handle().destroyBuffer(staging_buffer);
// 		this->device_->handle().freeMemory(staging_memory);
// 	}
// 	else
// 	{
// 		void *dst = this->device_->handle().mapMemory(this->memory(), dst_offset, stride_);
// 		::memcpy(dst, src, stride_);
// 		this->device_->handle().unmapMemory(this->memory());
// 	}
// }

void RF::vulkan::super_buffer_impl_t::copy(const vk::ArrayProxy<std::size_t> indices_proxy,
                                           const vk::ArrayProxy<void *> data_proxy,
                                           const std::optional<vk::CommandPool> pool)
{
	auto indices = std::vector<std::size_t>(indices_proxy.begin(), indices_proxy.end());
	auto data = std::vector<void *>(data_proxy.begin(), data_proxy.end());

	if (indices.size() != data.size())
	{
		throw RF::engine_error("super_buffer_impl_t::copy: index and data arrays must be the same size");
	}

	if (indices.empty()) return;

	// Validate ranges
	for (auto index : indices)
	{
		if (index >= count_) throw RF::engine_error("super_buffer_impl_t::copy out of range");
	}

	if (staged())
	{
		if (!pool.has_value())
		{
			throw RF::engine_error("super_buffer_impl_t::copy requires a command pool when staged");
		}

		// Allocate one staging buffer large enough to hold all copies
		vk::Buffer staging_buffer;
		vk::DeviceMemory staging_memory;

		std::size_t total_size = stride_ * indices.size();

		create_buffer_(
			staging_buffer,
			staging_memory,
			total_size,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		);

		// Map and populate all subregions
		auto staging_data = this->device_->handle().mapMemory(staging_memory, 0, total_size);

		for (std::size_t i = 0; i < indices.size(); ++i)
		{
			void *dst = static_cast<std::uint8_t *>(staging_data) + (i * stride_);
			::memcpy(dst, data[i], stride_);
		}

		this->device_->handle().unmapMemory(staging_memory);

		// Record one command buffer with multiple regions
		vk::CommandBufferAllocateInfo alloc_info(pool.value(), vk::CommandBufferLevel::ePrimary, 1);
		vk::CommandBuffer command_buffer = this->device_->handle().allocateCommandBuffers(alloc_info).at(0);

		vk::CommandBufferBeginInfo begin_info(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		command_buffer.begin(begin_info);

		std::vector<vk::BufferCopy> regions;
		regions.reserve(indices.size());

		for (std::size_t i = 0; i < indices.size(); ++i)
		{
			std::size_t dst_offset = stride_ * indices[i];
			vk::DeviceSize src_offset = i * stride_;
			regions.emplace_back(src_offset, dst_offset, stride_);
		}

		command_buffer.copyBuffer(staging_buffer, this->handle(), regions);

		command_buffer.end();

		vk::Queue graphics_queue = this->device_->handle().getQueue(0, 0);
		vk::SubmitInfo submit_info({}, {}, {}, 1, &command_buffer);
		graphics_queue.submit(submit_info);
		graphics_queue.waitIdle();

		this->device_->handle().freeCommandBuffers(pool.value(), command_buffer);
		this->device_->handle().destroyBuffer(staging_buffer);
		this->device_->handle().freeMemory(staging_memory);
	}
	else
	{
		// Map once and update all regions directly
		void *base_ptr = this->device_->handle().mapMemory(this->memory(), 0, size_);

		for (std::size_t i = 0; i < indices.size(); ++i)
		{
			std::size_t dst_offset = stride_ * indices[i];
			void *dst = static_cast<std::uint8_t *>(base_ptr) + dst_offset;
			::memcpy(dst, data[i], stride_);
		}

		this->device_->handle().unmapMemory(this->memory());
	}
}


vk::DeviceSize RF::vulkan::super_buffer_impl_t::offset(std::size_t index) const
{
	if (index >= count_) throw RF::engine_error("super_buffer_impl_t::offset out of range");
	return stride_ * index;
}


const bool RF::vulkan::super_buffer_impl_t::staged() const
{
	return this->staged_;
}

const vk::Buffer &RF::vulkan::super_buffer_impl_t::handle() const
{
	return this->handle_;
}

const vk::DeviceMemory &RF::vulkan::super_buffer_impl_t::memory() const
{
	return this->memory_;
}

const vk::DeviceSize &RF::vulkan::super_buffer_impl_t::size() const
{
	return this->size_;
}

const void *RF::vulkan::super_buffer_impl_t::data() const
{
	return this->data_;
}
