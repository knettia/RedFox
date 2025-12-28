#pragma once

#include "RF/maths/vec.hpp"

#include <cstddef>
#include <vector>
#include <string_view>

namespace RF
{
	enum class image_format
	{
		None = 0x0,
		KTX,
		DDS,
		EXR,
		BIT,
		TGA,
		PNG,
		JPEG,
	};

	inline std::string to_string(const image_format type)
	{
		switch (type)
		{
		case (image_format::None): return "None";
		case (image_format::KTX): return "KTX";
		case (image_format::DDS): return "DDS";
		case (image_format::EXR): return "EXR";
		case (image_format::BIT): return "BIT";
		case (image_format::TGA): return "TGA";
		case (image_format::PNG): return "PNG";
		case (image_format::JPEG): return "JPEG";
		default: return "Unknown";
		}
	}

	enum class image_type
	{
		None = 0x0,
		Auto = None, // alias
		Mixed = None, // alias
		R = 0x1,
		Greyscale = R, // alias
		RG = 0x2,
		Twochannel = RG, // alias
		RGB = 0x3,
		Colour = RGB, // alias
		RGBA = 0x4,
		Composited = RGBA, // alias
	};

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

	RF::image_data_t load_image(RF::image_format format, RF::image_type type, const std::string_view file);

	enum class atlas_offset
	{
		Absolute,
		Relative
	};

	struct atlas_offset_t
	{
		RF::fvec2 size;
		atlas_offset type;
	};

	struct atlas_element_t
	{
		RF::image_format format;
		std::string file;
		atlas_offset_t offset;
	};

	RF::image_data_t load_atlas(RF::image_type, std::span<const atlas_element_t> elements);
} // namespace RF
