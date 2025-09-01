#include "RF/image.hpp"
#include "RF/exception.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
	memcpy(img.bytes.data(), src, n);
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
		std::uint8_t *data = stbi_load(file.data(), &width, &height, &channels, 3);

		if (!data) throw RF::engine_error("Failed to get image data from file '<0>'", file);

		RF::image_format format = (channels == 4) ? RF::image_format::RGBA8 : RF::image_format::RGB8;
		auto data_size = static_cast<std::size_t>(width * height * channels);

		auto result = image_data_t::from_raw(data, data_size, RF::uivec2(width, height), type, format);

		stbi_image_free(data);
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
