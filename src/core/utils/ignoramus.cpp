#include "redfox/core/utils/ignoramus.hpp" // header

std::string RF::ignoramus::type_to_hue(RF::ignoramus_t t)
{
	switch (t)
	{
		case (RF::ignoramus_t::info):
		{
			return "\033[32m";
		}
		case (RF::ignoramus_t::warning):
		{
			return "\033[33m";
		}
		case (RF::ignoramus_t::error):
		{
			return "\033[31m";
		}
	}
}

std::string RF::ignoramus::type_to_str(RF::ignoramus_t t)
{
	switch (t)
	{
		case (RF::ignoramus_t::info):
		{
			return "[INFO]";
		}
		case (RF::ignoramus_t::warning):
		{
			return "[WARNING]";
		}
		case (RF::ignoramus_t::error):
		{
			return "[ERROR]";
		}
	}
}