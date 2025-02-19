// delegate.hpp
#pragma once

#include "RedFox/render/interface/types/framework.hpp" // RF::framwork_t
#include "RedFox/render/interface/window.hpp"

#include <vector>
#include <memory>
#include <string>

namespace RF
{
	struct delegate_info
	{
		std::string name;
		RF::framework_t framework;
	};

	class delegate
	{
	private:
		RF::delegate_info info_;
		std::vector<std::shared_ptr<RF::window>> windows_;
	public:
		delegate(RF::delegate_info info);
		~delegate();

		std::shared_ptr<RF::window> create_window(RF::window_info);
	};

	std::unique_ptr<RF::delegate> create_delegate(RF::delegate_info info);
} // namespace RF