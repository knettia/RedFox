#pragma once

#include "RF/interface/video_mode.hpp"
#include "RF/interface/window.hpp"

#include <string>
#include <functional>
 
namespace RF
{
	enum class framework_t;
	enum class virtual_key_t;

	struct delegate_info
	{
		std::string name;
		RF::framework_t framework;
	};

	class delegate
	{
	private:
		inline static bool created_;
	protected:
		const RF::video_mode_t neutral_video_mode_;

		RF::delegate_info info_;
		std::function<void()> terminate_callback_;
	public:
		// copy/move instructions:
		delegate(const delegate &) = default;
		delegate(delegate &&) = delete;
		delegate &operator=(delegate &&) = delete;

		// constructor/destructor instructions:
		virtual ~delegate();
                delegate(RF::delegate_info info);

		// static polymorphic create:
		static RF::delegate *create(RF::delegate_info info);

		// callbacks: 
		inline void set_terminate_callback(std::function<void()> callback)
		{ this->terminate_callback_ = callback; }

		// platform specifics:
		virtual void terminate() = 0;
		virtual void poll_events() = 0;
		virtual char32_t to_keysym(RF::virtual_key_t key) = 0;
		virtual RF::window *create_window(RF::window_info) = 0;
	};
} // namespace RF