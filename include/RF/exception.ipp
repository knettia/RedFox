template <typename ...Args>
inline RF::engine_error::engine_error(const std::string_view message, Args &&...args)
:
	RF::engine_error(RF::format_view(message, std::forward<Args>(args)...))
{ }

template <typename ...Args>
inline RF::runtime_error::runtime_error(const std::string_view message, Args &&...args)
:
	RF::runtime_error(RF::format_view(message, std::forward<Args>(args)...))
{ }
