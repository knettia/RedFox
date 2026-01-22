// vulkan
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>

// RedFox
#include "RF/vulkan/buffer.hpp"
#include "RF/vulkan/descriptor_set.hpp"
#include "RF/vulkan/descriptor_pool.hpp"

void RF::vulkan::descriptor_set_impl_t::init(const vk::Device device)
{
	this->device_ = device;
}

void RF::vulkan::descriptor_set_impl_t::destroy()
{
	this->device_.destroyDescriptorSetLayout(this->layout_);
}

void RF::vulkan::descriptor_set_impl_t::add_binding(const RF::vulkan::descriptor_binding_t binding)
{ this->bindings_.push_back(binding); }

const vk::DescriptorSet RF::vulkan::descriptor_set_impl_t::handle() const
{
	return this->handle_;
}

const vk::DescriptorSetLayout RF::vulkan::descriptor_set_impl_t::layout() const
{
	return this->layout_;
}

void RF::vulkan::descriptor_set_impl_t::allocate(const RF::vulkan::descriptor_pool_t pool)
{
	std::vector<vk::DescriptorSetLayoutBinding> layout_bindings(this->bindings_.size());

	for (std::size_t i = 0; i < this->bindings_.size(); i++)
	{
		layout_bindings[i] = vk::DescriptorSetLayoutBinding(
			this->bindings_[i]->index(),
			this->bindings_[i]->type(),
			1,
			this->bindings_[i]->stage(),
			nullptr
		);
	}

	vk::DescriptorSetLayoutCreateInfo layout_info(
		{},
		layout_bindings.size(),
		layout_bindings.data()
	);

	this->layout_ = this->device_.createDescriptorSetLayout(layout_info);

	vk::DescriptorSetAllocateInfo alloc_info(
		pool->handle(),
		1,
		&this->layout_
	);
	this->handle_ = this->device_.allocateDescriptorSets(alloc_info).front();
}

void RF::vulkan::descriptor_binding_impl_t::bind(const std::uint16_t index, const vk::DescriptorType type,
                                                 const vk::ShaderStageFlagBits stage,
                                                 const RF::vulkan::descriptor_set_t descriptor_set,
                                                 const vk::Device device)
{
	this->index_ = index;
	this->type_ = type;

	this->stage_ = stage;
	this->descriptor_set_ = descriptor_set;

	this->device_ = device;
}

void RF::vulkan::descriptor_binding_impl_t::set_buffer(const RF::vulkan::buffer_t buffer, const vk::DeviceSize offset)
{
	vk::DescriptorBufferInfo buffer_info(
		buffer->handle(),
		offset,
		buffer->size()
	);

	vk::WriteDescriptorSet descriptor_write(
		this->descriptor_set_->handle(),
		this->index_,
		0,
		1,
		this->type_,
		nullptr,
		&buffer_info,
		nullptr
	);

	this->device_.updateDescriptorSets(1, &descriptor_write, 0, nullptr);
}

void RF::vulkan::descriptor_binding_impl_t::set_buffer(const RF::vulkan::super_buffer_t buffer, const vk::DeviceSize offset)
{
	vk::DescriptorBufferInfo buffer_info(
		buffer->handle(),
		offset,
		buffer->element_size()
	);

	vk::WriteDescriptorSet descriptor_write(
		this->descriptor_set_->handle(),
		this->index_,
		0,
		1,
		this->type_,
		nullptr,
		&buffer_info,
		nullptr
	);

	this->device_.updateDescriptorSets(1, &descriptor_write, 0, nullptr);
}

void RF::vulkan::descriptor_binding_impl_t::set_sampler(const vk::Sampler sampler, const vk::ImageView view, const vk::ImageLayout layout)
{
	vk::DescriptorImageInfo image_info(
		sampler,
		view,
		layout
	);

	vk::WriteDescriptorSet descriptor_write(
		this->descriptor_set_->handle(),
		this->index_,
		0,
		1,
		this->type_,
		&image_info,
		nullptr,
		nullptr
	);

	this->device_.updateDescriptorSets(1, &descriptor_write, 0, nullptr);
}

void RF::vulkan::descriptor_binding_impl_t::set_image(const vk::ImageView view,
                                                      const vk::ImageLayout layout)
{
    vk::DescriptorImageInfo image_info(
		{},                 // no sampler for storage images
		view,
		layout              // e.g., vk::ImageLayout::eGeneral
	);

	vk::WriteDescriptorSet descriptor_write(
		this->descriptor_set_->handle(),  // target descriptor set
		this->index_,                     // binding index
		0,                                // array element offset
		1,                                // descriptor count
		this->type_,                      // should be VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
		&image_info,                       // pointer to image info
		nullptr,                           // buffer info (not used)
		nullptr                            // texel buffer view (not used)
	);

	this->device_.updateDescriptorSets(1, &descriptor_write, 0, nullptr);
}


const std::uint16_t RF::vulkan::descriptor_binding_impl_t::index() const
{
	return this->index_;
}

const vk::DescriptorType RF::vulkan::descriptor_binding_impl_t::type() const
{
	return this->type_;
}

const vk::ShaderStageFlagBits RF::vulkan::descriptor_binding_impl_t::stage() const
{
	return this->stage_;
}
