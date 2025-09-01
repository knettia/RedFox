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

RF::image_data_t RF::load_image(RF::image_type type, const std::string_view file)
{
	switch (type)
	{
	case (RF::image_type::PNG):
	case (RF::image_type::JPEG):
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		std::uint8_t *data = stbi_load(file.data(), &width, &height, &channels, 4);

		if (!data) throw RF::engine_error("Failed to get image data from file '<0>'", file);

		RF::image_format format = (channels == 4) ? RF::image_format::RGBA8 : RF::image_format::RGB8;
		auto data_size = static_cast<std::size_t>(width * height * 4);

		auto result = image_data_t::from_raw(data, data_size, RF::uivec2(width, height), type, format);

		stbi_image_free(data);
		return result;
	}

	case (RF::image_type::EXR):
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
		
		auto format = RF::image_format::None;

		if (channels == 4) format = RF::image_format::RGBA8;
		else if (channels == 3) format = RF::image_format::RGB8;
		else throw RF::engine_error("Unsupported number of channels <0> in EXR image '<1>'", channels, file);

		auto result = image_data_t::from_raw(data, data_size, RF::uivec2(width, height), type, format);

		delete[] data;
		return result;
	}

	case (RF::image_type::KTX):
	{
	#if defined (RF_KTX_SUPPORT)
		ktxTexture *kTexture{};
		ktx_error_code_e error_code = ktxTexture_CreateFromNamedFile(file.data(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &kTexture);

		if (error_code != KTX_SUCCESS || !kTexture) throw RF::runtime_error("Failed to load KTX texture. KTX error: <0>", ktxErrorString(error_code));

		ktx_uint8_t *data = ktxTexture_GetData(kTexture);
		ktx_size_t data_size = ktxTexture_GetDataSize(kTexture);

		// For now assume RGBA8, could be extended with more precise format parsing
		image_data_t result = image_data_t::from_raw(data, data_size, RF::uivec2(kTexture->baseWidth, kTexture->baseHeight), type, RF::image_format::RGBA8);

		ktxTexture_Destroy(kTexture);
		return result;
	#else
		throw RF::runtime_error("Failed to load KTX texture. RedFox Engine build was compiled without libktx");
	#endif
	}

	case (RF::image_type::DDS):
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
	case (RF::image_type::BIT):
	case (RF::image_type::TGA): throw RF::engine_error("Attempt to load file of an unimplemented type <0>", RF::to_string(type));

	[[fallthrough]];
	case (RF::image_type::None): throw RF::engine_error("Attempt to load file of type 'None'");
	}
	return {};
}
