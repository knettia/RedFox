#pragma once

#include "RF/maths/vec/vec2.hpp"

#include <cstddef>
#include <vector>
#include <string_view>

namespace RF
{
	enum class image_t
	{
		KTX,
		DDS,
		BIT,
		PNG,
		JPEG,
	};

	struct image_data_t
	{
		std::vector<std::byte> bytes;
		RF::uivec2 size;
		RF::image_t type;

		image_data_t(const image_data_t &other) = default; // Or implement deep copy
		image_data_t &operator=(const image_data_t &other) = default;

		image_data_t(const std::uint8_t *src, std::size_t n);
	};

	RF::image_data_t load_image(RF::image_t type, std::string_view file);
} // namespace RF
