#pragma once

#include <asio.hpp>
#include <vector>
#include <array>
#include <cstring>

#include "RF/log.hpp"
#include "RF/net/message.hpp"

namespace RF::net
{
	using steady_clock = std::chrono::steady_clock;
	using time_point = std::chrono::steady_clock::time_point;
	using milliseconds = std::chrono::milliseconds;
	using seconds = std::chrono::seconds;

	inline void return_if_error(std::error_code ec, std::size_t)
	{
		if(ec)
		{
			RF::logf::error("Network error: <0>", ec.message());
			return;
		}
	}

	template <typename S>
	const char *data(const S &src)
	{
		static char buffer[sizeof(S)];
		std::memcpy(buffer, &src, sizeof(S));
		return buffer;
	}

	struct asio_endpoint_hash
	{
		std::size_t operator()(asio::ip::udp::endpoint const &ep) const noexcept
		{
			std::size_t h1 = std::hash<std::string>()(ep.address().to_string());
			std::size_t h2 = std::hash<unsigned short>()(ep.port());
			return h1 ^ (h2 << 1);
		}
	};

	struct asio_endpoint_equal
	{
		bool operator()(asio::ip::udp::endpoint const &a, asio::ip::udp::endpoint const &b) const noexcept
		{
			return a.address() == b.address() && a.port() == b.port();
		}
	};

	std::array<std::uint8_t, 8> native_to_big_endian_ulong(ulong v);
	ulong big_endian_to_native_ulong(const std::uint8_t *buf);

	std::vector<std::uint8_t> serialize_internal(const RF::net::internal_message &msg);
	std::vector<std::uint8_t> serialize_public(const RF::net::message &msg);

	RF::net::internal_message deserialize_internal(const std::uint8_t *data, std::size_t len);

	RF::net::message to_public(const RF::net::internal_message &im);
}
