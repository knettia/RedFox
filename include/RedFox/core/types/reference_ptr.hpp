#pragma once

#include <stdexcept>
#include <cassert>

namespace RF
{
	template<typename T>
	class reference_ptr
	{
	private:
		T *ptr_;
	public:
		explicit reference_ptr(T *object);
		reference_ptr(std::nullptr_t) = delete;

		// copy/move instructions
		reference_ptr(const reference_ptr& other) noexcept = default;
		reference_ptr(reference_ptr&& other) noexcept = default;
		reference_ptr& operator=(const reference_ptr& other) noexcept = default;
		reference_ptr& operator=(reference_ptr&& other) noexcept = default;

		T &operator*() const noexcept;
		T *operator->() const noexcept;
		operator T*() const noexcept;
		bool valid() const noexcept;
	};
}

#include "reference_ptr.ipp"