#pragma once

#include <memory>
#include <utility>
#include <type_traits>

namespace RF
{
	template<typename T>
	class handle_ptr
	{
	private:
		std::shared_ptr<T> impl_;
	public:
		handle_ptr()
		:
			impl_(std::make_shared<T>())
		{}

		handle_ptr(const handle_ptr &) = default;
		handle_ptr(handle_ptr &&) noexcept = default;
		handle_ptr &operator=(const handle_ptr &) = default;
		handle_ptr &operator=(handle_ptr &&) noexcept = default;

		template<typename ...Args, typename = std::enable_if_t<!std::disjunction_v<std::is_same<std::decay_t<Args>, handle_ptr>...>>>
		handle_ptr(Args &&...args)
		:
			impl_(std::make_shared<T>(std::forward<Args>(args)...))
		{}

		T *operator->()
		{
			return impl_.get();
		}

		const T *operator->() const
		{
			return impl_.get();
		}
	};
} // namespace RF
