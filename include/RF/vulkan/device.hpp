#pragma once

#include "RF/handle_ptr.hpp"
#include "RF/vulkan/instance.hpp"

#include <cstdint>
#include <vulkan/vulkan.hpp>

namespace RF::vulkan
{
	struct gpu_find_info_t
	{
		vk::PhysicalDevice gpu = nullptr;
		vk::PhysicalDeviceFeatures required_features = {};

		bool has_graphics_queue = true;
		bool has_compute_queue = false;
		bool has_present_queue = false;
		vk::SurfaceKHR surface = nullptr;
	};

	struct device_config_t
	{
		std::vector<std::string> extensions = std::vector<std::string>(0);
		std::vector<std::string> layers = std::vector<std::string>(0);

		device_config_t(const bool debug = false);
	};

	class device_impl_t
	{
	private:
		bool initialised_;

		vk::Device handle_;
		vk::PhysicalDevice gpu_;

		vk::Queue graphics_queue_;
		std::uint32_t graphics_family_;

		vk::Queue compute_queue_;
		std::uint32_t compute_family_;
	public:
		device_impl_t() = default;

		void init(const RF::vulkan::instance_t instance,
		          const RF::vulkan::gpu_find_info_t info,
		          const RF::vulkan::device_config_t config);

		void destroy();

		[[nodiscard]] const vk::Device &handle() const;
		[[nodiscard]] const vk::PhysicalDevice &gpu() const;
		
		[[nodiscard]] const vk::Queue &graphics_queue() const;
		[[nodiscard]] const std::uint32_t &graphics_family() const;

		[[nodiscard]] const vk::Queue &compute_queue() const;
		[[nodiscard]] const std::uint32_t &compute_family() const;
	};

	using device_t = RF::handle_ptr<device_impl_t>;
}
