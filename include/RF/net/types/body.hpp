#pragma once

#include <cstdint>
#include <span>
#include <vector>

namespace RF::net
{
	struct body
	{
	private:
		std::uint16_t size_;
		std::vector<char> data_;
	public:
		body() : size_(0), data_(0) {}
		body(std::uint16_t size, std::vector<char> data) : size_(size), data_(std::move(data)) {}
		~body() = default;

		std::uint16_t size() const { return this->size_; }
		std::span<const char> data() const { return this->data_; }
	};
} // namespace RF::net
