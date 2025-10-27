#pragma once

#include <span>
#include <vector>

#include "RF/net/types/body.hpp"
#include "RF/template.hpp"

namespace RF::net
{
	typedef std::uint64_t ulong_t;

	enum internal_message_id_t : ulong_t
	{
		Application = 0,
		Disconnect  = 1,
		SetAgent    = 2,
		Heartbeat   = 3
	};

	struct internal_message
	{
		internal_message_id_t id;
		ulong_t app_id;
		std::vector<RF::net::body> payload;
	};

	struct message
	{
		ulong_t id;
		std::vector<RF::net::body> payload;
	};
} // namespace RF::net
