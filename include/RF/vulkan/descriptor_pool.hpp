#pragma once

#include "RF/handle_ptr.hpp"
#include <vulkan/vulkan.hpp>

namespace RF::vulkan
{
	class descriptor_binding_impl_t;
	using descriptor_binding_t = RF::handle_ptr<descriptor_binding_impl_t>;
	
	class descriptor_set_impl_t;
	using descriptor_set_t = RF::handle_ptr<descriptor_set_impl_t>;

	class buffer_impl_t;
	using buffer_t = RF::handle_ptr<buffer_impl_t>;

	class descriptor_pool_impl_t
	{
	private:
		bool initialised_ = false;

		vk::Device device_;
		vk::DescriptorPool handle_;
	public:
		descriptor_pool_impl_t() = default;

		void init(const vk::DescriptorPoolCreateFlagBits flag, const std::uint16_t sets,
		          const vk::ArrayProxy<const vk::DescriptorPoolSize> sizes, const vk::Device device);
		void destroy();
		
		[[nodiscard]] const vk::DescriptorPool &handle() const;
	};

	using descriptor_pool_t = RF::handle_ptr<descriptor_pool_impl_t>;
}
