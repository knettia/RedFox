template <typename T>
inline std::string RF::to_oss_str(T &&value)
{
	std::ostringstream oss;
	oss << std::forward<T>(value);
	return std::move(oss).str();
}

template <typename ...Args>
inline std::string RF::format_view(std::string_view fmt, Args &&...args)
{
	const std::vector<std::string> list = { RF::to_oss_str(std::forward<Args>(args))... };

	return RF::format_view(fmt, list);
}
