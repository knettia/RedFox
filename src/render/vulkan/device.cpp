#include "RF/vulkan/device.hpp"
#include "RF/exception.hpp"
#include <optional>

namespace
{
	std::optional<std::uint32_t> find_graphics_family(const vk::PhysicalDevice gpu)
	{
		std::optional<std::uint32_t> graphics_family = std::nullopt;

		std::vector<vk::QueueFamilyProperties> queue_families = gpu.getQueueFamilyProperties();
		for (std::uint32_t i = 0; i < queue_families.size(); ++i)
		{
			if (queue_families[i].queueFlags & vk::QueueFlagBits::eGraphics)
			{
				graphics_family = i;
				break;
			}
		}

		return graphics_family;
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

	std::optional<vk::PhysicalDevice> find_gpu_from_info(const vk::Instance instance, const RF::vulkan::gpu_find_info_t info)
	{
		std::vector<vk::PhysicalDevice> gpus = instance.enumeratePhysicalDevices();

		if (gpus.empty()) throw RF::engine_error("No Vulkan-supported GPUs found.");

		for (const vk::PhysicalDevice &gpu : gpus)
		{
			vk::PhysicalDeviceFeatures gpu_features = gpu.getFeatures();

			// TODO: create a function to compare gpu_features to info.required_features

			if (info.has_graphics_queue)
			{
				auto graphics = find_graphics_family(gpu);
				if (!graphics.has_value()) continue;
			}

			if (info.has_present_queue)
			{
				auto present = find_present_family(gpu, info.surface);
				if (!present.has_value()) continue;
			}

			return gpu;
		}

		return std::nullopt;
	}
}

RF::vulkan::device_config_t::device_config_t(const bool debug)
{
#if defined(__APPLE__)
	this->extensions.emplace_back("VK_KHR_portability_subset");
#endif

	if (debug) layers.emplace_back("VK_LAYER_KHRONOS_validation");
}

void RF::vulkan::device_impl_t::init(const RF::vulkan::instance_t instance,
                                     const RF::vulkan::gpu_find_info_t info,
                                     const RF::vulkan::device_config_t config)
{
	if (this->initialised_) throw RF::engine_error("Cannot initialise an already initialised RF::vulkan::device_impl_t");

	if (info.gpu) this->gpu_ = info.gpu;
	else
	{
		auto gpu_opt = find_gpu_from_info(instance->handle(), info);

		if (gpu_opt.has_value()) this->gpu_ = gpu_opt.value();
		else throw RF::engine_error("No suitable GPU found from info");
	}

	auto graphics_family_opt = find_graphics_family(this->gpu_);

	if (graphics_family_opt.has_value()) this->graphics_family_ = graphics_family_opt.value();
	else
	{
		std::string_view GPU_name = this->gpu_.getProperties().deviceName;
		throw RF::engine_error(RF::format_view("No absolute graphics family queue found for GPU: <0>", GPU_name));
	}

	float queue_priorities = 1.0f;
	vk::DeviceQueueCreateInfo queue_info(vk::DeviceQueueCreateFlags(0),
	                                     this->graphics_family_,
	                                     1, &queue_priorities,
	                                     nullptr);

	vk::PhysicalDeviceFeatures device_features = this->gpu_.getFeatures();

	std::vector<const char *> layers;
	layers.reserve(config.layers.size());
	for (std::string_view str : config.layers) layers.push_back(str.data());

	std::vector<const char *> extensions;
	extensions.reserve(config.extensions.size());
	for (std::string_view str : config.extensions) extensions.push_back(str.data());

	vk::DeviceCreateInfo create_info(vk::DeviceCreateFlags(0),
	                                 1, &queue_info,
	                                 layers.size(), layers.data(),
	                                 extensions.size(), extensions.data(),
	                                 &device_features, nullptr);
	
	this->handle_ = this->gpu_.createDevice(create_info);

	this->graphics_queue_ = this->handle_.getQueue(this->graphics_family_, 0);

	this->initialised_ = true;
}

void RF::vulkan::device_impl_t::destroy()
{
	if (!this->initialised_) throw RF::engine_error("Cannot destroy an empty RF::vulkan::device_impl_t");

	this->handle_.destroy();

	this->initialised_ = false;
}

const vk::Device &RF::vulkan::device_impl_t::handle() const
{
	return this->handle_;
}

const vk::PhysicalDevice &RF::vulkan::device_impl_t::gpu() const
{
	return this->gpu_;
}

const vk::Queue &RF::vulkan::device_impl_t::graphics_queue() const
{
	return this->graphics_queue_;
}

const std::uint32_t &RF::vulkan::device_impl_t::graphics_family() const
{
	return this->graphics_family_;
}
