#pragma once

#include <algorithm>
#include <new>      // For placement new
#include <utility>  // For std::move
#include <stdexcept>
#include <span>
#include <optional>
#include <memory> // std::destroy_at
#include <vector>

namespace RF
{

	template <typename T, std::size_t N>
	class mass
	{
	private:
		std::aligned_storage_t<sizeof(T), alignof(T)> buffer_[N];
		std::size_t size_ = 0;
		mutable std::vector<std::size_t> last_found_;
	public:
		mass() = default;
		~mass() { clear(); }

		T *data() noexcept { return reinterpret_cast<T*>(buffer_); }
		const T *data() const noexcept { return reinterpret_cast<const T*>(buffer_); }

		mass(const mass &other)
		{
			for (std::size_t i = 0; i < other.size_; ++i)
			{ new (this->data() + i) T(other[i]); }

			size_ = other.size_;
		}

		mass &operator=(const mass &other)
		{
			if (this != &other)
			{
				this->clear();
				for (std::size_t i = 0; i < other.size_; ++i)
				{ new (this->data() + i) T(other[i]); }
				size_ = other.size_;
				// no swap, please
			}

			return *this;
		}

		constexpr std::size_t capacity() const noexcept { return N; }
		std::size_t size() const noexcept { return size_; }

		void push_back(const T &value)
		{
			if (size_ >= N) throw std::overflow_error("RF::mass is full");
			new (this->data() + size_) T(value);
			++size_;
		}

		template <typename... Args>
		void emplace_back(Args &&...args)
		{
			if (size_ >= N)
			{ throw std::overflow_error("RF::mass is full"); }

			new (this->data() + size_) T(std::forward<Args>(args)...);
			++size_;
		}

		std::optional<T> pop_back()
		{
			if (size_ > 0)
			{
				--size_;
				T value = std::move(this->data()[size_]);
				std::destroy_at(&this->data()[size_]);
				return value;
			}

			return std::nullopt;
		}

		void erase(std::size_t index)
		{
			if (index >= size_)
			{ throw std::out_of_range("Index out of range"); }

			this->data()[index].~T();

			for (std::size_t i = index; i < size_ - 1; ++i)
			{ new (this->data() + i) T(std::move(this->data()[i + 1])); }

			--size_;
		}

		T &operator[](std::size_t index)
		{
			if (index >= size_)
			{ throw std::out_of_range("Index out of range"); }
			return this->data()[index];
		}

		const T &operator[](std::size_t index) const
		{
			if (index >= size_)
			{ throw std::out_of_range("Index out of range"); }
			return this->data()[index];
		}

		std::optional<std::size_t> find(const T &v)
		{
			auto it = std::find(this->begin(), this->end(), v);

			if (it != this->end())
			{ return std::distance(this->begin(), it); }

			return std::nullopt;
		}

		std::span<std::size_t> find_all(const T &v) const
		{
			last_found_.clear();
			std::size_t index = 0;

			for (const T &e : *this)
			{
				if (e == v)
				{ last_found_.push_back(index); }
				++index;
			}

			return std::span<std::size_t>(last_found_);
		}

		void clear()
		{
			for (std::size_t i = 0; i < size_; ++i)
			{ this->data()[i].~T(); }
			size_ = 0;
		}

		// primitive iterators
		T *begin() noexcept
		{ return this->data(); }

		T *end() noexcept
		{ return this->data() + size_; }

		const T *begin() const noexcept
		{ return this->data(); }

		const T *end() const noexcept
		{ return this->data() + size_; }

		const T *cbegin() const noexcept
		{ return this->data(); }

		const T *cend() const noexcept
		{ return this->data() + size_; }
	};
} // namespace RF