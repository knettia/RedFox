#pragma once

#include <cstdint> // std::uint16_t

namespace internal
{
	enum class msg_t : std::uint16_t
	{
		Ping, 
		PingAll
	};
}