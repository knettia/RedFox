template <typename ...Args>
void RF::logf::out(RF::logf::level_t level, std::string_view fmt, Args &&...args)
{
	std::string format = RF::format_view(fmt, std::forward<Args>(args)...);
	std::vector<std::string> split = RF::split_lines(format);

	std::string_view prefix;
	std::string_view color;

	switch (level)
	{
		case (RF::logf::level_t::error):
		{
			prefix = "[ERROR]";
			color = RF::logf::detail::error;
			break;
		}

		case (RF::logf::level_t::info):
		{
			prefix = "[INFO]";
			color = RF::logf::detail::info;
			break;
		}

		case (RF::logf::level_t::warn):
		{
			prefix = "[WARN]";
			color = RF::logf::detail::warn;
			break;
		}
	}

	for (std::string_view view : split)
	{
		std::cout << color << prefix << RF::logf::detail::reset << " :: " << view << '\n';
	}
}

template <typename ...Args>
inline void RF::logf::error(std::string_view fmt, Args &&...args)
{
	RF::logf::out(RF::logf::level_t::error, fmt, std::forward<Args>(args)...);
}

template <typename ...Args>
inline void RF::logf::info(std::string_view fmt, Args &&...args)
{
	RF::logf::out(RF::logf::level_t::info, fmt, std::forward<Args>(args)...);
}

template <typename ...Args>
inline void RF::logf::warn(std::string_view fmt, Args &&...args)
{
	RF::logf::out(RF::logf::level_t::warn, fmt, std::forward<Args>(args)...);
}