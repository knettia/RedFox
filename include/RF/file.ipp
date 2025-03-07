template<RF::ull scale>
inline RF::memory_t<scale> RF::file_m::file_size(const std::string_view path)
{
	return RF::memory_cast<scale>(RF::file_m::get_file_size_(path));
}