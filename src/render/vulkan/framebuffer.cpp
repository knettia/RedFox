#include "RF/vulkan/device.hpp"
#include "RF/vulkan/image.hpp"
#include "RF/vulkan/render_pass.hpp"

#include "RF/vulkan/framebuffer.hpp"

// namespace 
void RF::vulkan::framebuffer_impl_t::init(const RF::vulkan::device_t device,
                                          const RF::vulkan::image_t image,
                                          const vk::Extent2D extent,
                                          const RF::vulkan::render_pass_t pass)
{
	if (this->initialised_)
	throw RF::engine_error("Cannot initialise an already initialised RF::vulkan::framebuffer_impl_t");

	this->device_ = device;
	this->image_ = image;
	this->pass_ = pass;

	vk::FramebufferCreateInfo info
	{
		{},
		this->pass_->handle(),
		1,
		&this->image_->view(),
		extent.width,
		extent.height,
		1
	};

	this->handle_ = this->device_->handle().createFramebuffer(info);

	initialised_ = true;
}

void RF::vulkan::framebuffer_impl_t::destroy()
{
	if (!this->initialised_)
	throw RF::engine_error("Cannot destroy an empty RF::vulkan::framebuffer_impl_t");

	this->device_->handle().destroyFramebuffer(this->handle_);

	initialised_ = false;
}

const vk::Framebuffer &RF::vulkan::framebuffer_impl_t::handle() const
{
	return this->handle_;
}

const RF::vulkan::image_t &RF::vulkan::framebuffer_impl_t::image() const
{
	return this->image_;
}

const RF::vulkan::render_pass_t &RF::vulkan::framebuffer_impl_t::pass() const
{
	return this->pass_;
}

