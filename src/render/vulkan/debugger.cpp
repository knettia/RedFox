#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include "RF/log.hpp"

namespace RF::vulkan
{

bool has_validation_layers()
{
	std::uint32_t count;
	vk::Result result;

	result = vk::enumerateInstanceLayerProperties(&count, nullptr);

	std::vector<vk::LayerProperties> layers(count);

	result = vk::enumerateInstanceLayerProperties(&count, layers.data());

	for (const auto properties : layers)
	if (std::string(properties.layerName.data()) == "VK_LAYER_KHRONOS_validation")
	return true;

	return false;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                              vk::DebugUtilsMessageTypeFlagsEXT type,
                                              const vk::DebugUtilsMessengerCallbackDataEXT* data,
                                              void *)
{
	auto msg = RF::format_view("[VULKAN] :: <0>", data->pMessage);


	switch (severity)
	{
		case (vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning): RF::logf::warn(msg);
		case (vk::DebugUtilsMessageSeverityFlagBitsEXT::eError): RF::logf::error(msg);
		case (vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo): RF::logf::info(msg);
		case (vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose): RF::logf::info("[VERBOSE] :: <0>", msg);
	}

	return VK_FALSE;
}

vk::DebugUtilsMessengerCreateInfoEXT generate_debug_info()
{
	auto info = vk::DebugUtilsMessengerCreateInfoEXT {};

	info.messageSeverity =
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
		// vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose;

	info.messageType =
		// vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding |
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
		vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
	
	info.pfnUserCallback = reinterpret_cast<decltype(vk::DebugUtilsMessengerCreateInfoEXT::pfnUserCallback)>(debug_callback);
	info.pUserData = nullptr;

	return info;
}

} // namespace RF::vulkan
