#pragma once

#include <vulkan/vulkan.hpp>

namespace RF::vulkan
{
	struct pipeline_barrier_t
	{
		vk::ImageAspectFlags aspect = vk::ImageAspectFlagBits::eNone;
		vk::ImageLayout layout = vk::ImageLayout::eUndefined;
		vk::AccessFlags access = {};
		vk::PipelineStageFlags stage = vk::PipelineStageFlagBits::eTopOfPipe;
	};

	struct vertex_description_t
	{
		vk::VertexInputBindingDescription binding;
		std::vector<vk::VertexInputAttributeDescription> attributes;
	};

	bool has_validation_layers();

	VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
	                                              vk::DebugUtilsMessageTypeFlagsEXT type,
	                                              const vk::DebugUtilsMessengerCallbackDataEXT* data,
	                                              void *);
	
	vk::DebugUtilsMessengerCreateInfoEXT generate_debug_info();
} // namespace RF::vulkan
