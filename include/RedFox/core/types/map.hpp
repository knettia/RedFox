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
                map(std::initializer_list<std::pair<T1, T2>> list);

		[[nodiscard]] std::optional<T2> operator[](T1 key) const;

		[[nodiscard]] std::optional<T1> operator[](T2 key) const;

		bool insert(T1 k, T2 v);

		bool exsert(T1 k, T2 v);
	};
} // namespace RF

#include "map.ipp"