#include "RF/image.hpp"
#include "RF/exception.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#if defined (RF_KTX_SUPPORT)
#include <ktx.h>
#endif

RF::image_data_t::image_data_t(const std::uint8_t *src, std::size_t n)
{
	this->data.size = n;
	this->data.raw = new std::byte[n];
	for (std::size_t i = 0; i < n; ++i)
	{
		this->data.raw[i] = static_cast<std::byte>(src[i]);
	}
}

RF::image_data_t::~image_data_t()
{
	delete[] this->data.raw;
}

RF::image_data_t RF::load_image(RF::image_t type, std::string_view file)
{
	switch (type)
	{
		case (RF::image_t::PNG):
		case (RF::image_t::JPEG):
		{
			int width, height, channels;
			stbi_set_flip_vertically_on_load(true);
			std::uint8_t *data = stbi_load(file.data(), &width, &height, &channels, 4);

			if (!data)
			{
				throw RF::engine_error("Failed to get image data from file '<0>' loaded with standard image type '<1>'", file, static_cast<int>(type));
			}

			std::size_t data_size = width * height * 4; // 4 channels (RGBA)

			RF::image_data_t result(data, data_size);
			result.size = RF::uivec2(width, height);
			result.type = type;

			stbi_image_free(data);

			return result;
		}

		case (RF::image_t::KTX):
		{
		#if defined (RF_KTX_SUPPORT)	
			ktxTexture *kTexture;
			ktx_error_code_e error_code = ktxTexture_CreateFromNamedFile(file.data(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &kTexture);
			if(error_code != KTX_SUCCESS || !kTexture)
			{
				throw RF::runtime_error("Failed to load KTX texture. KTX error code: <0>", ktxErrorString(error_code));
			}

			// get image data
			ktx_uint8_t *data = ktxTexture_GetData(kTexture);
			ktx_size_t data_size = ktxTexture_GetDataSize(kTexture);
			
			// handle flipping
			// TODO: find better way to do this
			ktx_uint32_t row_size = kTexture->baseWidth * 4;

			std::vector<ktx_uint8_t> temp_row(row_size);

			ktx_uint8_t *top = data;
			ktx_uint8_t *bottom = data + (kTexture->baseHeight - 1) * row_size;

			while(top < bottom)
			{
				// swap rows
				std::memcpy(temp_row.data(), top, row_size);
				std::memcpy(top, bottom, row_size);
				std::memcpy(bottom, temp_row.data(), row_size);

				top += row_size;
				bottom -= row_size;
			}

			// handle vector translation
			RF::image_data_t result(data, data_size);
			result.size = RF::uivec2(kTexture->baseWidth, kTexture->baseHeight);
			result.type = type;

			ktxTexture_Destroy(kTexture);

			return result;
		#else
			throw RF::runtime_error("Failed to load KTX texture. RedFox Engine build was compiled without libktx");
		#endif
		}

		case (RF::image_t::DDS):
		{
			// TODO: implement a dds loader
			throw RF::engine_error("Attempt to load file of an unimplemented type", file);
		}

		case (RF::image_t::BIT):
		{
			// TODO: implement a simple bmp loader
			throw RF::engine_error("Attempt to load file of an unimplemented type", file);
		}
	}
}