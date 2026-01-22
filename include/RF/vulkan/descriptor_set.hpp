#pragma once

#include "RF/handle_ptr.hpp"
#include "RF/vulkan/buffer.hpp"

#include <vulkan/vulkan.hpp>

namespace RF::vulkan
{
	class descriptor_pool_impl_t;
	using descriptor_pool_t = RF::handle_ptr<descriptor_pool_impl_t>;
	
	class descriptor_binding_impl_t;
	using descriptor_binding_t = RF::handle_ptr<descriptor_binding_impl_t>;

	class descriptor_set_impl_t;
	using descriptor_set_t = RF::handle_ptr<descriptor_set_impl_t>;

	class buffer_impl_t;
	using buffer_t = RF::handle_ptr<buffer_impl_t>;

	class descriptor_binding_impl_t
	{
	private:
		vk::Device device_;
		RF::vulkan::descriptor_set_t descriptor_set_;

		std::uint16_t index_;
		vk::DescriptorType type_;
		vk::ShaderStageFlagBits stage_;
	public:
		descriptor_binding_impl_t() = default;

		void bind(const std::uint16_t index, const vk::DescriptorType type,
		          const vk::ShaderStageFlagBits stage,
		          const RF::vulkan::descriptor_set_t descriptor_set,
		          const vk::Device device);

		void set_buffer(const RF::vulkan::buffer_t buffer, const vk::DeviceSize offset = 0);
		void set_buffer(const RF::vulkan::super_buffer_t buffer, const vk::DeviceSize offset = 0);
		void set_sampler(const vk::Sampler sampler, const vk::ImageView view, const vk::ImageLayout layout);
		void set_image(const vk::ImageView view, const vk::ImageLayout layout);

		[[nodiscard]] const std::uint16_t index() const;
		[[nodiscard]] const vk::DescriptorType type() const;
		[[nodiscard]] const vk::ShaderStageFlagBits stage() const;
	};

	using descriptor_binding_t = RF::handle_ptr<descriptor_binding_impl_t>;

	class descriptor_set_impl_t
	{
	private:
		vk::Device device_;
		std::vector<RF::vulkan::descriptor_binding_t> bindings_;

		vk::DescriptorSet handle_;
		vk::DescriptorSetLayout layout_;
	public:
		descriptor_set_impl_t() = default;

		void init(const vk::Device device);
		void destroy();

		void add_binding(const RF::vulkan::descriptor_binding_t binding);
		void allocate(const RF::vulkan::descriptor_pool_t pool);

		[[nodiscard]] const vk::DescriptorSet handle() const;
		[[nodiscard]] const vk::DescriptorSetLayout layout() const;
	};

	using descriptor_set_t = RF::handle_ptr<descriptor_set_impl_t>;
}
