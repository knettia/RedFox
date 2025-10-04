#pragma once

#include <RF/handle_ptr.hpp>
#include <RF/interface/delegate.hpp>
#include <RF/interface/window.hpp>
#include <vulkan/vulkan.hpp>

namespace RF::vulkan
{
	struct instance_config_t
	{
		std::vector<std::string> extensions = std::vector<std::string>(0);
		std::vector<std::string> layers = std::vector<std::string>(0);

		instance_config_t(const RF::delegate_info info, const bool surface = false, const bool debug = false);
	};

	class instance_impl_t
	{
	private:
		bool initialised_;

		vk::Instance handle_;
	public:
		instance_impl_t() = default;

		void init(const vk::ApplicationInfo app,
		          const RF::vulkan::instance_config_t config);

		vk::SurfaceKHR create_surface(RF::window *window) const;

		void destroy();

		[[nodiscard]] const vk::Instance &handle() const;
	};

	using instance_t = RF::handle_ptr<instance_impl_t>;
}
