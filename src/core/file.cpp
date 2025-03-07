#include "RF/file.hpp"

#include <fstream>
#include <stdexcept>

#if defined (_WIN32)
#include <algorithm> // std::replace

#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#endif // _WIN32

#if defined (__linux__) || defined (__APPLE__)
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <cstdlib>
#endif // __linux__, __APPLE__

std::string RF::file_m::normalise_path_(const std::string_view path)
{
#if defined(_WIN32)
	std::string fixed_path = path;
	std::replace(fixed_path.begin(), fixed_path.end(), '/', '\\');
	return fixed_path;
#endif // _WIN32
	return std::string(path);
}

RF::bytes RF::file_m::get_file_size_(const std::string_view path)
{
	std::string fixed_path = RF::file_m::normalise_path_(path);
	std::ifstream file(fixed_path, std::ios::binary | std::ios::ate);
	if (!file)
	{ throw std::runtime_error("Error opening file: " + fixed_path); }

	return RF::bytes(file.tellg());
}

#if defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#endif

std::string RF::file_m::home_path()
{
#if defined(__linux__) || defined(__APPLE__)
	const char *home = getenv("HOME");
	if (home)
	{ return std::string(home); }

	struct passwd *pw = getpwuid(getuid());
	if (pw)
	{ return std::string(pw->pw_dir); }

	throw std::runtime_error("UNIX: failed to get home directory");
#elif defined (_WIN32)
	char path[MAX_PATH];
	if (SHGetFolderPathA(nullptr, CSIDL_PROFILE, nullptr, 0, path) == S_OK)
	{ return std::string(path); }

	throw std::runtime_error("Win32: Failed to get home directory");
#endif
}

bool RF::file_m::exists(const std::string_view path)
{
	std::string fixed_path = RF::file_m::normalise_path_(path);
	std::ifstream file(fixed_path);
	return file.good();
}

#if defined (__linux__) || defined (__APPLE__)
#include <sys/stat.h>
#elif defined (_WIN32)
#include <direct.h>
#endif

bool RF::file_m::create_directory(const std::string_view path)
{
	std::string fixed_path = RF::file_m::normalise_path_(path);
	
#if defined (__linux__) || defined (__APPLE__)
	return mkdir(fixed_path.c_str(), 0777) == 0 || errno == EEXIST;
#elif defined (_WIN32)
	return _mkdir(fixed_path.c_str()) == 0 || errno == EEXIST;
#endif
}

void RF::file_m::create_file(const std::string_view filename)
{
	RF::file_m::write_file(filename, std::string(), true);
}

void RF::file_m::delete_file(const std::string_view filename)
{
	std::string fixed_path = RF::file_m::normalise_path_(filename);
	if (std::remove(fixed_path.c_str()) != 0)
	{ throw std::runtime_error("failed to delete file"); }
}

void RF::file_m::rename_file(const std::string_view src, const std::string_view dst)
{
	std::string fixed_src = RF::file_m::normalise_path_(src);
	std::string fixed_dst = RF::file_m::normalise_path_(dst);

	std::string content = RF::file_m::read_file(fixed_src);

	RF::file_m::delete_file(fixed_src);
	RF::file_m::write_file(fixed_dst, content, true);
}

std::string RF::file_m::read_file(const std::string_view filename)
{
	std::ios::sync_with_stdio(false);

	std::string fixed_filename = RF::file_m::normalise_path_(std::string(filename));
	std::string result;

	std::ifstream file(fixed_filename, std::ios::binary);
	if (!file)
	{ throw std::runtime_error("Error opening file: " + fixed_filename); }

	file.seekg(0, std::ios::end);
	size_t file_size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(file_size);
	file.read(buffer.data(), file_size);

	result.assign(buffer.begin(), buffer.end());
	file.close();

	return std::move(result);
}

void RF::file_m::write_file(const std::string_view filename, const std::string &content, const bool overwrite)
{
	std::ios::sync_with_stdio(false);

	std::string fixed_filename = RF::file_m::normalise_path_(std::string(filename));

	std::ios_base::openmode openmode = overwrite ? std::ios::trunc : std::ios::app; 

	std::ofstream file;
	file.open(fixed_filename, openmode);

	if (!file)
	{ throw std::runtime_error("Error opening file: " + fixed_filename); }

	file << content;
	file.close();
}

void RF::file_m::open_file(const std::string_view path)
{
	std::string fixed_path = RF::file_m::normalise_path_(path);

#if defined(__linux__) || defined(__APPLE__)
#if defined(__linux__)
	std::string command = "xdg-open " + fixed_path;
#elif defined(__APPLE__)
	std::string command = "open " + fixed_path;
#endif
	system(command.c_str());
#elif defined(_WIN32)
	ShellExecuteA(nullptr, "open", fixed_path.c_str(), nullptr, nullptr, SW_SHOWDEFAULT);
#endif
}

#if defined (__linux__) || defined (__APPLE__)
#include <dirent.h>
#endif

std::vector<std::string> RF::file_m::files(const std::string_view dir_path)
{
	std::string fixed_directory = RF::file_m::normalise_path_(dir_path);
	std::vector<std::string> files;

#if defined(__APPLE__) || defined(__linux__)
		DIR *dir;
		struct dirent *entry;
		if ((dir = opendir(fixed_directory.c_str())) != nullptr)
		{
			while ((entry = readdir(dir)) != nullptr)
			{
				std::string file_name = entry->d_name;
				if (file_name != "." && file_name != "..")
				{ files.push_back(file_name); }
			}
			closedir(dir);
		}
#elif defined(_WIN32)
		WIN32_FIND_DATAA find_file_data;
		HANDLE hFind = FindFirstFileA((fixed_directory + "\\*").c_str(), &find_file_data);

		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				std::string file_name = find_file_data.cFileName;
				if (file_name != "." && file_name != "..")
				{ files.push_back(file_name); }
			} while (FindNextFileA(hFind, &find_file_data));

			FindClose(hFind);
		}
#endif

	return files;
}