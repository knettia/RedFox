#include "RF/net/utils.hpp"

std::array<std::uint8_t, 8> RF::net::native_to_big_endian_ulong(ulong v)
{
	std::array<std::uint8_t, 8> out;
	for (int i = 0; i < 8; ++i)
	{
		out[7 - i] = static_cast<std::uint8_t>(v & 0xFFu);
		v >>= 8;
	}

	return out;
}

ulong RF::net::big_endian_to_native_ulong(const std::uint8_t *buf)
{
	ulong v = 0;
	for (int i = 0; i < 8; ++i)
	{
		v <<= 8;
		v |= static_cast<ulong>(buf[i]);
	}

	return v;
}
		
