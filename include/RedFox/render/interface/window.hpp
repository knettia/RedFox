#pragma once
// RedFox
#include "RedFox/render/interface/types/framework.hpp" // RF::framework_t
#include "RedFox/render/interface/types/graphics.hpp" // RF::graphics_t
#include "RedFox/core/types/vector/vec2.hpp" // RF::uivec2

// std
#include <string>

namespace RF
{
	struct window_info
	{
		std::string title;
		RF::framework_t framework;
		RF::graphics_t graphics;
		RF::uivec2 size;
		int monitor;
	};

	enum class window_state_t : uint8_t
	{
		Hidden,
		Visible,
		Focused
	};

	class window
	{
	protected:
		const RF::window_info info_;
		RF::window_state_t state_;
	public:
		virtual ~window() = default;
		window(RF::window_info info);

		virtual void poll_events() = 0;

		virtual void close() = 0;
		virtual void focus() = 0;
		virtual void minimize() = 0;

		RF::window_info get_info() const;
		RF::window_state_t get_state() const;
	};

	RF::window *create_window(RF::window_info info);
} // namespace RF