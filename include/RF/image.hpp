#pragma once

#include "RF/maths/vec/vec2.hpp"

#include <cstddef>
#include <vector>
#include <string_view>

namespace RF
{
	enum class image_type
	{
		None = 0x0,
		KTX,
		DDS,
		EXR, // newly added
		BIT,
		TGA, // newly added
		PNG,
		JPEG,
	};

	inline std::string to_string(const image_type type)
	{
		switch (type)
		{
		case (image_type::None): return "None";
		case (image_type::KTX): return "KTX";
		case (image_type::DDS): return "DDS";
		case (image_type::EXR): return "EXR";
		case (image_type::BIT): return "BIT";
		case (image_type::TGA): return "TGA";
		case (image_type::PNG): return "PNG";
		case (image_type::JPEG): return "JPEG";
		default: return "Unknown";
		}
	}

	enum class image_format
	{
		None = 0x0,
		RGBA8,
		RGB8,
		RGBA16F,
		RGBA32F,
	};

	inline std::string to_string(const image_format format)
	{
		switch (format)
		{
		case (image_format::None): return "None";
		case (image_format::RGBA8): return "RGBA8";
		case (image_format::RGB8): return "RGB8";
		case (image_format::RGBA16F): return "RGBA16F";
		case (image_format::RGBA32F): return "RGBA32F";
		default: return "Unknown";
		}
	}

	struct image_data_t
	{
		std::vector<std::byte> bytes;
		RF::uivec2 size {};
		RF::image_type type { RF::image_type::None };
		RF::image_format format { RF::image_format::None };

		image_data_t() = default;
		image_data_t(const image_data_t &other) = default;
		image_data_t &operator=(const image_data_t &other) = default;

		static image_data_t from_raw(const std::uint8_t *src, std::size_t n, RF::uivec2 size, RF::image_type type, RF::image_format format);

		bool valid() const noexcept;
	};

	RF::image_data_t load_image(RF::image_type type, const std::string_view file);
} // namespace RF
