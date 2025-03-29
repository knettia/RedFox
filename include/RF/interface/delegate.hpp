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
		const RF::video_mode_t video_mode_;

		RF::delegate_info info_;
		std::function<void()> terminate_callback_;
	public:
		// copy/move instructions:
		delegate(const delegate &) = default;
		delegate(delegate &&) = delete;
		delegate &operator=(delegate &&) = delete;

		// constructor/destructor instructions:
		virtual ~delegate();
                delegate(RF::delegate_info info, const RF::video_mode_t mode);

		// static polymorphic create:
		static RF::delegate *create(RF::delegate_info info);

		// ---- Video Mode API ----
		/**
		 * Returns the default video mode of the delegate
		 */
		inline RF::video_mode_t video_mode()
		{ return this->video_mode_; }

		/**
		 * Returns the current video mode of the main display
		 */
		virtual RF::video_mode_t current_video_mode() = 0;

		/**
		 * Returns a list of the available video modes of the main display
		 */
		virtual std::vector<RF::video_mode_t> enumerate_video_modes() = 0;

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