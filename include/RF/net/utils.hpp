#pragma once

#include <cstring>
#include "RF/log.hpp"

namespace RF::net
{
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
}
