#pragma once

#include <unordered_map>
#include <initializer_list>
#include <optional>

namespace RF
{
	// WIP: still working on it
	template<typename T1, typename T2>
	class map
	{
	private:
		std::unordered_map<T1, T2> forward_map_;
		std::unordered_map<T2, T1> reverse_map_;
	public:
		// copy/move instructions:
                map(const map &) = delete;
                map(map &&) = delete;
                map &operator=(const map &) = delete;
                map &operator=(map &&) = delete;

		// constructor/destructor instructions:
		map() = default;
		~map() = default;
                map(std::initializer_list<std::pair<T1, T2>> list)
		{
			for (const auto &[key, value] : list)
			{
				this->forward_map_.emplace(key, value);
				this->reverse_map_.emplace(value, key);
			}
                }

		[[nodiscard]] std::optional<T2> operator[](T1 key) const
		{
			auto it = this->forward_map_.find(key);
			if (it != this->forward_map_.end())
			{
				return it->second;
			}
			return std::nullopt;
		}

		[[nodiscard]] std::optional<T1> operator[](T2 key) const
		{
			auto it = this->reverse_map_.find(key);
			if (it != this->reverse_map_.end())
			{
				return it->second;
			}
			return std::nullopt;
		}

		bool insert(T1 k, T2 v)
		{
			if (
				this->forward_map_.find(k) == this->forward_map_.end() &&
				this->reverse_map_.find(v) == this->reverse_map_.end()
			)
			{
				this->forward_map_.emplace(k, v);
				this->reverse_map_.emplace(v, k);
				return true;
			}

			return false;
		}

		bool exsert(T1 k, T2 v)
		{
			if (
				this->forward_map_.find(k) != this->forward_map_.end() &&
				this->reverse_map_.find(v) != this->reverse_map_.end()
			)
			{
				// remove k from forward_map_ and v from reverse_map_
				this->forward_map_.erase(k);
				this->reverse_map_.erase(v);
				return true;
			}
			return false;
		}
	};
} // namespace RF