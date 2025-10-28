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

std::vector<std::uint8_t> RF::net::serialize_internal(const RF::net::internal_message &msg)
{
	std::vector<std::uint8_t> buf;
	auto id_bytes = RF::net::native_to_big_endian_ulong(static_cast<ulong>(msg.id));
	auto app_bytes = RF::net::native_to_big_endian_ulong(msg.app_id);

	buf.insert(buf.end(), id_bytes.begin(), id_bytes.end());
	buf.insert(buf.end(), app_bytes.begin(), app_bytes.end());
	if (!msg.payload.empty())
	{
		for (auto body : msg.payload)
		{
			auto size_bytes = RF::net::native_to_big_endian_ulong(static_cast<ulong>(body.size()));

			buf.insert(buf.end(), size_bytes.begin(), size_bytes.end());
			buf.insert(buf.end(), body.begin(), body.end());
		}
	}
	return buf;
}

std::vector<std::uint8_t> RF::net::serialize_public(const RF::net::message &msg)
{
	RF::net::internal_message im;
	im.id = RF::net::internal_message_id_t::Application;
	im.app_id = msg.id;
	im.payload = msg.payload;
	return serialize_internal(im);
}

RF::net::internal_message RF::net::deserialize_internal(const std::uint8_t *data, std::size_t len)
{
	if (len < sizeof(ulong) * 2)
	{
		throw std::runtime_error("Invalid message: too short");
	}

	std::size_t offset = 0;

	auto id = RF::net::big_endian_to_native_ulong(data + offset);
	offset += sizeof(ulong);

	auto app_id = RF::net::big_endian_to_native_ulong(data + offset);
	offset += sizeof(ulong);

	std::vector<RF::net::body> payload;

	while (offset + sizeof(ulong) <= len)
	{
		auto body_size = RF::net::big_endian_to_native_ulong(data + offset);
		offset += sizeof(ulong);

		if (offset + body_size > len)
		{
			throw std::runtime_error("Invalid message: body size exceeds remaining data");
		}

		RF::net::body body;
		body.insert(body.end(), data + offset, data + offset + body_size);
		offset += body_size;

		payload.push_back(std::move(body));
	}

	RF::net::internal_message m;
	m.id = static_cast<RF::net::internal_message_id_t>(id);
	m.app_id = app_id;
	m.payload = std::move(payload);

	return m;
}

RF::net::message RF::net::to_public(const RF::net::internal_message &im)
{
	RF::net::message m;
	m.id = im.app_id;
	m.payload = im.payload;
	return m;
}
