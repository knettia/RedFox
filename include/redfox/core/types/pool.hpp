#pragma once

#include <vector>
#include <stack>
#include <memory>

#include "./memory.hpp"  // RF::memory_t<scale>

namespace RF
{
	template<typename T>
	class pool
	{
	private:
		const ull max_bytes;
		const ull max_elements;

		std::vector<T> data_;
		std::vector<std::size_t> free_indices_;
		ull allocated_memory_;
		std::size_t allocated_count_;
	public:
		template <RF::ull scale>
		pool(std::size_t n, RF::memory_t<scale> size) : max_elements(n), max_bytes(RF::membytes(size)), allocated_memory_(0), allocated_count_(0)
		{
			data_.reserve(n);
			free_indices_.reserve(n);
		}

		std::size_t alloc(const T& object)
		{
			if (allocated_count_ >= max_elements)
			{ throw std::runtime_error("pool out of space"); }

			const RF::ull object_size = sizeof(T);
			if (allocated_memory_ + object_size > max_bytes)
			{ throw std::runtime_error("pool out of memory"); }

			std::size_t index;
			if (!free_indices_.empty())
			{
				index = free_indices_.back();
				free_indices_.pop_back();
				data_[index] = object;
			}
			else
			{
				index = data_.size();
				data_.push_back(object);
			}

			allocated_memory_ += object_size;
			allocated_count_++;

			return index;
		}

		void free(std::size_t index)
		{
			if (index >= data_.size() || allocated_count_ == 0)
			{ throw std::runtime_error("freeing of invalid index"); }

			free_indices_.push_back(index);
			allocated_count_--;
			allocated_memory_ -= sizeof(T);
		}

		T& get(std::size_t index)
		{
			if (index >= data_.size())
			{ throw std::runtime_error("index out of bounds"); }
			return data_[index];
		}

		const T& get(std::size_t index) const
		{
			if (index >= data_.size())
			{ throw std::runtime_error("index out of bounds"); }
			return data_[index];
		}

		ull memory() const
		{ return allocated_memory_; }

		ull max_memory() const
		{ return max_bytes; }

		std::size_t size() const
		{ return allocated_count_; }

		std::size_t max_size() const
		{ return max_elements; }
	};
} // namespace RF