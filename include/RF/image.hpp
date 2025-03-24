#pragma once

#include "RF/vec2.hpp"

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
		std::vector<std::byte> data;
		RF::uivec2 size;
		RF::image_t type;

		~image_data_t() = default; // auto-generate deconstructor for data and size
	};

	RF::image_data_t load_image(RF::image_t type, std::string_view file);
} // namespace RF
