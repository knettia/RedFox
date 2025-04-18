#pragma once

#include "RF/maths/vec2.hpp"

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
		struct
		{
			std::byte *raw;
			std::size_t size;
		} data;
		RF::uivec2 size;
		RF::image_t type;

		image_data_t(const std::uint8_t *src, std::size_t n);
		~image_data_t();
	};

	RF::image_data_t load_image(RF::image_t type, std::string_view file);
} // namespace RF
