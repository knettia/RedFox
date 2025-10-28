#pragma once

#include <cstdint>
#include <sys/types.h>

#include <vector>

namespace RF::net
{
	using body = std::vector<std::uint8_t>;

	enum internal_message_id_t : ulong
	{
		Application = 0,
		Disconnect  = 1,
		SetAgent    = 2,
		Heartbeat   = 3
	};

	struct internal_message
	{
		internal_message_id_t id;
		ulong app_id;
		std::vector<RF::net::body> payload;
	};

	struct message
	{
		ulong id;
		std::vector<RF::net::body> payload;
	};
} // namespace RF::net
