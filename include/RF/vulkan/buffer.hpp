#pragma once

#include "RF/handle_ptr.hpp"
#include "RF/vulkan/device.hpp"

#include <optional>
#include <cstdint>

#include <vulkan/vulkan.hpp>

namespace RF::vulkan
{
	class buffer_impl_t
	{
	protected:
		bool initialised_;

		RF::vulkan::device_t device_;

		std::uint32_t memory_type_(std::uint32_t memory_type_bits, vk::MemoryPropertyFlags required_flags);

		void create_buffer_(vk::Buffer &buffer, vk::DeviceMemory &memory,
		                    vk::DeviceSize size, vk::BufferUsageFlags usage,
		                    vk::MemoryPropertyFlags properties);

		void copy_buffer_(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size,
			          const std::optional<vk::CommandPool> pool);

		bool staged_;

		vk::Buffer handle_;
		vk::DeviceMemory memory_;
		vk::DeviceSize size_;
		void *data_;
	public:
		buffer_impl_t() = default;

		void init(const RF::vulkan::device_t device,
		          const vk::BufferUsageFlags usage,
		          const std::uint64_t n, const bool stage);

		void destroy();
		
		void copy(const void *src,
		          const std::optional<vk::CommandPool> pool = std::nullopt);

		[[nodiscard]] const bool staged() const;

		[[nodiscard]] const vk::Buffer &handle() const;
		[[nodiscard]] const vk::DeviceMemory &memory() const;
		[[nodiscard]] const vk::DeviceSize &size() const;
		[[nodiscard]] const void *data() const;
	};

	using buffer_t = RF::handle_ptr<buffer_impl_t>;

	class super_buffer_impl_t : private buffer_impl_t
	{
	private:
		std::size_t stride_;   // aligned size of each element
		std::size_t count_;    // number of slots
		std::size_t element_size_;    // number of slots

	public:
		super_buffer_impl_t() = default;

		// Initialise a super buffer with count slots of element_size bytes each
		void init(const RF::vulkan::device_t device,
		          vk::BufferUsageFlags usage,
		          std::size_t element_size,
		          std::size_t count,
		          bool stage);
		
		void destroy();

		// Update unique indices within the super buffer with unique data, with arrays for efficient calls
		void copy(const vk::ArrayProxy<std::size_t> indices,
		          const vk::ArrayProxy<void *> data,
		          const std::optional<vk::CommandPool> pool = std::nullopt);

		// Get the Vulkan dynamic offset for a slot
		[[nodiscard]] vk::DeviceSize offset(std::size_t index) const;

		// Access stride and count
		[[nodiscard]] std::size_t stride() const { return stride_; }
		[[nodiscard]] std::size_t count() const { return count_; }
		[[nodiscard]] std::size_t element_size() const { return element_size_; }

		[[nodiscard]] const bool staged() const;

		[[nodiscard]] const vk::Buffer &handle() const;
		[[nodiscard]] const vk::DeviceMemory &memory() const;
		[[nodiscard]] const vk::DeviceSize &size() const;
		[[nodiscard]] const void *data() const;
	};

	using super_buffer_t = RF::handle_ptr<super_buffer_impl_t>;
}
