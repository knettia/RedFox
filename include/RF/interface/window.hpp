#pragma once
// RedFox
#include "RF/vec2.hpp" // RF::uivec2

namespace vk
{
	struct Instance;
	struct AllocationCallbacks;

	template<typename T> struct ResultValue;
	struct SurfaceKHR;
}

// std
#include <string>
#include <functional>
#include <unordered_map>

namespace RF
{
	// forward declared
	enum class virtual_key_t;
	enum class mouse_key_t; 
	enum class key_state_t;

	enum class graphics_t;

	struct window_info
	{
		std::string title;
		RF::graphics_t graphics;
		RF::uivec2 size;
	};

	enum class window_state_t
	{
		Hidden,
		Visible,
		Focused
	};

	inline std::string to_string(RF::window_state_t t)
	{
		switch (t)
		{
			case (RF::window_state_t::Hidden):
			{ return std::string("Hidden"); }
			
			case (RF::window_state_t::Visible):
			{ return std::string("Visible"); }

			case (RF::window_state_t::Focused):
			{ return std::string("Focused"); }
		}
	};

	class window
	{
	protected:
		RF::window_info info_;
		RF::window_state_t state_;

		std::function<void(RF::window *)> close_callback_;
		std::function<void(RF::window *, RF::window_state_t)> state_changed_callback_;

		std::function<void(RF::window *, RF::virtual_key_t, RF::key_state_t)> virtual_key_event_callback_;
		std::function<void(RF::window *, RF::mouse_key_t, RF::key_state_t)> mouse_key_event_callback_;

		std::unordered_map<RF::virtual_key_t, RF::key_state_t> virtual_key_states_;
		std::unordered_map<RF::mouse_key_t, RF::key_state_t> mouse_key_states_;
	public:
		// move/copy instructions:
                window(const window &) = delete;
                window(window &&) = delete;
                window &operator=(const window &) = delete;
                window &operator=(window &&) = delete;

		// constructor/destructor instructions:
		virtual ~window() = default;
                window(RF::window_info info);

                // callbacks
		inline void set_close_callback(std::function<void(RF::window *)> callback)
		{ this->close_callback_ = callback; }

		inline void set_state_changed_callback(std::function<void(RF::window *, RF::window_state_t)> callback)
		{ this->state_changed_callback_ = callback; }

		inline void set_virtual_key_event_callback(std::function<void(RF::window *, RF::virtual_key_t, RF::key_state_t)> callback)
		{ this->virtual_key_event_callback_ = callback; }

		inline void set_mouse_key_event_callback(std::function<void(RF::window *, RF::mouse_key_t, RF::key_state_t)> callback)
		{ this->mouse_key_event_callback_ = callback; }

		// virtual
		virtual vk::ResultValue<vk::SurfaceKHR> create_surface(vk::Instance instance, const vk::AllocationCallbacks *allocator) = 0; // TODO: create_surface overloading for DirectX and Metal

		// virtual manipulation functions
		virtual void close() = 0;
		virtual void focus() = 0;
		virtual void minimise() = 0;

		virtual void set_size(RF::uivec2 size) = 0;

		RF::window_info get_info() const;
		RF::window_state_t get_state() const;
		RF::key_state_t get_key_state(RF::virtual_key_t key) const;
		RF::key_state_t get_key_state(RF::mouse_key_t key) const;
	};
} // namespace RF