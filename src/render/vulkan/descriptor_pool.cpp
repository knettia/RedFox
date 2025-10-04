#include "RF/vulkan/descriptor_pool.hpp" // header

#include "RF/exception.hpp"

void RF::vulkan::descriptor_pool_impl_t::init(const vk::DescriptorPoolCreateFlagBits flag, const std::uint16_t sets,
                                              const vk::ArrayProxy<const vk::DescriptorPoolSize> sizes, const vk::Device device)
{
	if (this->initialised_) throw RF::engine_error("Cannot initialise an already initialised RF::vulkan::descriptor_pool_impl_t");
	this->initialised_ = true;

	this->device_ = device;

	vk::DescriptorPoolCreateInfo info(
		flag,
		sets,
		sizes.size(),
		sizes.data()
	);

	try
	{ 
		this->handle_ = this->device_.createDescriptorPool(info, nullptr);
	}
	catch (vk::SystemError &error)
	{ throw RF::engine_error(RF::format_view("Failed to create Vulkan descriptor pool: <0>", error.what())); }
}

void RF::vulkan::descriptor_pool_impl_t::destroy()
{
	if (!this->initialised_) throw RF::engine_error("Cannot destroy an empty RF::vulkan::descriptor_pool_impl_t");
	this->initialised_ = false;

	this->device_.destroyDescriptorPool(this->handle_);
}

const vk::DescriptorPool &RF::vulkan::descriptor_pool_impl_t::handle() const
{
	return this->handle_;
}
