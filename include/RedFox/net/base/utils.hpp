#pragma once

#include "RedFox/core/utils/ignoramus.hpp"

namespace RF::net
{
	inline void return_if_error(std::error_code ec, std::size_t)
	{
		if(ec)
		{
			RF::ignoramus::logf(RF::ignoramus_t::error, "Network error: <0>", ec.message());
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
}