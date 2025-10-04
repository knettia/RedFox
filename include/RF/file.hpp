#pragma once

#include "RF/memory.hpp"

#include <string>
#include <string_view>
#include <vector>

namespace RF
{
	class file_m // static
	{
	private:
		static std::string normalise_path_(const std::string_view path);
		static RF::bytes get_file_size_(const std::string_view path);
	public:
		file_m() = delete;
		~file_m() = delete;

		static std::string home_path();

		static bool exists(const std::string_view filename);

		static bool create_directory(const std::string_view path);

		static void create_file(const std::string_view filename);
		static void delete_file(const std::string_view filename);
		static void rename_file(const std::string_view src, const std::string_view dst);

		static std::string read_file(const std::string_view filename);
		static void write_file(const std::string_view filename, const std::string &content, const bool overwrite = true);

		static void open_file(const std::string_view path);
		static std::vector<std::string> files(const std::string_view dir);
		
		template<RF::ull scale>
		static RF::memory_t<scale> file_size(const std::string_view path);
	};
} // namespace RF

#include "RF/file.ipp"
