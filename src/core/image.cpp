#include "RF/image.hpp"
#include "RF/exception.hpp"

#include <cstdlib>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfArray.h>
#include <OpenEXR/ImfHeader.h>
#include <OpenEXR/ImfChannelList.h>

#if defined (RF_KTX_SUPPORT)
#include <ktx.h>
#endif

#if defined(RF_DDS_SUPPORT)
#define TINYDDSLOADER_IMPLEMENTATION
#include <tinyddsloader.h>
#endif

RF::image_data_t RF::image_data_t::from_raw(const std::uint8_t *src, std::size_t n, RF::uivec2 size, RF::image_type type, RF::image_format format)
{
	image_data_t img;
	img.bytes.resize(n);
	::memcpy(img.bytes.data(), src, n);
	img.size = size;
	img.type = type;
	img.format = format;
	return img;
}

bool RF::image_data_t::valid() const noexcept
{
	return (!bytes.empty() && size.x > 0 && size.y > 0 && type != image_type::None && format != image_format::None);
}

static std::size_t channels_per_image_type(RF::image_type type)
{
	switch (type)
	{
	case (RF::image_type::R): return 1;
	case (RF::image_type::RG): return 2;
	case (RF::image_type::RGB): return 3;
	case (RF::image_type::RGBA): return 4;
	default: return 0;
	}
}

static RF::image_type image_type_from_channels(std::size_t channels)
{
	switch (channels)
	{
	case (1): return RF::image_type::R;
	case (2): return RF::image_type::RG;
	case (3): return RF::image_type::RGB;
	case (4): return RF::image_type::RGBA;
	default: return RF::image_type::None;
	}
}

RF::image_data_t RF::load_image(RF::image_format format, RF::image_type type, const std::string_view file)
{
	switch (format)
	{
	case (RF::image_format::PNG):
	case (RF::image_format::JPEG):
	{
		int width = 0, height = 0, channels = 0;
		stbi_set_flip_vertically_on_load(true);

		std::uint8_t *loaded = stbi_load(file.data(), &width, &height, &channels, 0);
		if (!loaded) throw RF::engine_error("Failed to get image data from file '<0>'", file);


		auto img_type = image_type_from_channels(channels);
		if (img_type == RF::image_type::None)
		{
			stbi_image_free(loaded);
			throw RF::engine_error("Unsupported number of channels <0> in image '<1>'", channels, file);
		}

		if (type == RF::image_type::None)
		{
			type = img_type;
		}

		int desiredChannels = channels_per_image_type(type);
		if (desiredChannels == 0)
		{
			stbi_image_free(loaded);
			throw RF::engine_error("Unsupported target image type requested for '<0>'", file);
		}

		std::size_t pixels = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
		std::size_t final_size = pixels * static_cast<std::size_t>(desiredChannels);

		std::vector<std::uint8_t> converted;
		std::uint8_t *data_ptr = loaded;

		if (desiredChannels != channels)
		{
			converted.resize(final_size);
			for (std::size_t p = 0; p < pixels; ++p)
			{
				std::uint8_t *src = loaded + p * static_cast<std::size_t>(channels);
				std::uint8_t *dst = converted.data() + p * static_cast<std::size_t>(desiredChannels);

				int copyCount = std::min(channels, desiredChannels);
				for (int c = 0; c < copyCount; ++c) dst[c] = src[c];

				for (int c = copyCount; c < desiredChannels; ++c) dst[c] = 0xFF;
			}

			data_ptr = converted.data();
		}

		auto result = image_data_t::from_raw(data_ptr, final_size, RF::uivec2(width, height), type, format);

		stbi_image_free(loaded);

		return result;
	}

	case (RF::image_format::EXR):
	{
		Imf::RgbaInputFile exr_file(file.data());
		auto header = exr_file.header();
		auto channel_list = header.channels();

		struct
		{
			bool r;
			bool g;
			bool b;
			bool a;
		} valid_channels;

		valid_channels.r = channel_list.findChannel("R") != nullptr;
		valid_channels.g = channel_list.findChannel("G") != nullptr;
		valid_channels.b = channel_list.findChannel("B") != nullptr;
		valid_channels.a = channel_list.findChannel("A") != nullptr;

		auto channels = 0;

		if (valid_channels.r) ++channels;
		if (valid_channels.g) ++channels;
		if (valid_channels.b) ++channels;
		if (valid_channels.a) ++channels;

		// past this point, this loader only supports RGB or RGBA images, and always outputs as RGBA,
		// we want it to be dynamic like the JPEG/PNG loader.
		auto dw = exr_file.dataWindow();
		auto width = dw.max.x - dw.min.x + 1;
		auto height = dw.max.y - dw.min.y + 1;

		Imf::Array<Imf::Rgba> pixels(width * height);
		exr_file.setFrameBuffer(&pixels[0] - dw.min.x - dw.min.y * width, 1, width);
		exr_file.readPixels(dw.min.y, dw.max.y);

		auto data_size = static_cast<std::size_t>(width * height * channels);
		auto data = new std::uint8_t[data_size];

		for (auto y = 0; y < height; ++y)
		{
			for (auto x = 0; x < width; ++x)
			{
				auto pixel = pixels[y * width + x];
				auto index = (y * width + x) * channels;
				data[index + 0] = static_cast<std::uint8_t>(pixel.r * 255);
				data[index + 1] = static_cast<std::uint8_t>(pixel.g * 255);
				data[index + 2] = static_cast<std::uint8_t>(pixel.b * 255);

				if (channels == 4)
				data[index + 3] = static_cast<std::uint8_t>(pixel.a * 255);
			}
		}

		auto img_type = image_type_from_channels(channels);

		auto result = image_data_t::from_raw(data, data_size, RF::uivec2(width, height), type, format);

		delete[] data;
		return result;
	}

	case (RF::image_format::KTX):
	{
	#if defined (RF_KTX_SUPPORT)
		ktxTexture *kTexture{};
		ktx_error_code_e error_code = ktxTexture_CreateFromNamedFile(file.data(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &kTexture);

		if (error_code != KTX_SUCCESS || !kTexture) throw RF::runtime_error("Failed to load KTX texture. KTX error: <0>", ktxErrorString(error_code));

		ktx_uint8_t *data = ktxTexture_GetData(kTexture);
		ktx_size_t data_size = ktxTexture_GetDataSize(kTexture);

		// For now assume RGBA8, could be extended with more prcise format parsing
		image_data_t result = image_data_t::from_raw(data, data_size, RF::uivec2(kTexture->baseWidth, kTexture->baseHeight), RF::image_type::RGB, format);

		ktxTexture_Destroy(kTexture);
		return result;
	#else
		throw RF::runtime_error("Failed to load KTX texture. RedFox Engine build was compiled without libktx");
	#endif
	}

	case (RF::image_format::DDS):
	{
	#if defined(RF_DDS_SUPPORT)
		DDSFile dds;
		auto ret = dds.Load(file.data());
		if (tinyddsloader::Result::Success != ret) throw RF::runtime_error("Failed to load DDS texture. TinyDDSLoader error: <0>", static_cast<int>(ret));

		// TODO: finish dds format support
		return {};
	#else
		throw RF::runtime_error("Failed to load DDS texture. RedFox Engine build was compiled without TinyDDSLoader");
	#endif
	}
	case (RF::image_format::BIT):
	case (RF::image_format::TGA): throw RF::engine_error("Attempt to load file of an unimplemented type <0>", RF::to_string(format));

	[[fallthrough]];
	case (RF::image_format::None): throw RF::engine_error("Attempt to load file of type 'None'");
	}
	return {};
}

#include <vector>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <span>

RF::image_data_t RF::load_atlas(RF::image_type type, std::span<const atlas_element_t> elements)
{

	const int channels = channels_per_image_type(type);
	if (channels == 0) throw RF::engine_error("Unsupported target image type requested for atlas");

	if (elements.empty())
	{
		auto empty = image_data_t{};
		empty.type = type;
		return empty;
	}

	struct loaded_elem
	{
		image_data_t img;
		RF::ivec2 pos;
		const atlas_element_t *spec;
	};

	std::vector<loaded_elem> loaded;
	loaded.reserve(elements.size());

	for (const auto &elem : elements)
	{
		auto img = RF::load_image(elem.format, type, elem.file);

		if (img.bytes.empty() && (img.size.x == 0 || img.size.y == 0))
			throw RF::engine_error("Failed to load atlas element '<0>'", elem.file);

		const int w = static_cast<int>(img.size.x);
		const int h = static_cast<int>(img.size.y);

		int ox = 0;
		int oy = 0;
		if (elem.offset.type == atlas_offset::Absolute)
		{
			ox = static_cast<int>(std::lround(elem.offset.size.x));
			oy = static_cast<int>(std::lround(elem.offset.size.y));
		}
		else // Relative
		{
			ox = static_cast<int>(std::lround(elem.offset.size.x * static_cast<float>(w)));
			oy = static_cast<int>(std::lround(elem.offset.size.y * static_cast<float>(h)));
		}

		loaded.push_back(loaded_elem{ std::move(img), RF::ivec2(ox, oy), &elem });
	}

	int minx = std::numeric_limits<int>::max();
	int miny = std::numeric_limits<int>::max();
	int maxx = std::numeric_limits<int>::min();
	int maxy = std::numeric_limits<int>::min();

	for (const auto &le : loaded)
	{
		const int w = static_cast<int>(le.img.size.x);
		const int h = static_cast<int>(le.img.size.y);

		minx = std::min(minx, le.pos.x);
		miny = std::min(miny, le.pos.y);
		maxx = std::max(maxx, le.pos.x + w);
		maxy = std::max(maxy, le.pos.y + h);
	}

	if (minx >= maxx || miny >= maxy)
	{
		auto empty = image_data_t{};
		empty.type = type;
		return empty;
	}

	const int width  = maxx - minx;
	const int height = maxy - miny;

	for (auto &le : loaded)
	{
		le.pos.x -= minx;
		le.pos.y -= miny;
	}

	const std::size_t pixels = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
	const std::size_t atlas_size = pixels * static_cast<std::size_t>(channels);
	std::vector<std::byte> buffer(atlas_size, std::byte(0u));

	for (const auto &le : loaded)
	{
		const int src_w = static_cast<int>(le.img.size.x);
		const int src_h = static_cast<int>(le.img.size.y);
		const int dst_x = le.pos.x;
		const int dst_y = le.pos.y;

		if (src_w <= 0 || src_h <= 0) continue;

		const std::size_t src_row = static_cast<std::size_t>(src_w) * static_cast<std::size_t>(channels);
		const std::size_t dst_row = static_cast<std::size_t>(width) * static_cast<std::size_t>(channels);

		const std::byte *src_pixels = le.img.bytes.data();
		std::byte *dst_pixels = buffer.data();

		int copy_x0 = std::max(0, dst_x);
		int copy_y0 = std::max(0, dst_y);
		int copy_x1 = std::min(width, dst_x + src_w);	// exclusive
		int copy_y1 = std::min(height, dst_y + src_h);   // exclusive

		if (copy_x1 <= copy_x0 || copy_y1 <= copy_y0) continue; // nothing visible

		for (int row = copy_y0; row < copy_y1; ++row)
		{
			const int src_row = row - dst_y;			   // row in source image
			const int src_col = copy_x0 - dst_x;			// starting column in source row
			const std::size_t srcOffset = (static_cast<std::size_t>(src_row) * static_cast<std::size_t>(src_w) + static_cast<std::size_t>(src_col))
										  * static_cast<std::size_t>(channels);

			const std::size_t dstOffset = (static_cast<std::size_t>(row) * static_cast<std::size_t>(width) + static_cast<std::size_t>(copy_x0))
										  * static_cast<std::size_t>(channels);

			const std::size_t bytesToCopy = static_cast<std::size_t>(copy_x1 - copy_x0) * static_cast<std::size_t>(channels);

			std::memcpy(dst_pixels + dstOffset, src_pixels + srcOffset, bytesToCopy);
		}
	}

	auto result = image_data_t::from_raw(reinterpret_cast<std::uint8_t *>(buffer.data()), atlas_size, RF::uivec2(width, height), type, RF::image_format::None);

	return result;
}
