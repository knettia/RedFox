// RedFox
#include <RF/monitor.hpp>
#include <RF/file.hpp>
#include <RF/log.hpp>

int main()
{
	RF::monitor_m::activate(true);

	std::string home = RF::file_m::home_path();
	RF::logf::info("Home directory: <0>", home);

	std::string test_dir = home + "/test_rf";
	RF::file_m::create_directory(test_dir);
	RF::logf::info("Created directory: <0>", test_dir);

	std::string test_file = test_dir + "/test.txt";
	RF::file_m::create_file(test_file);
	RF::logf::info("Created file: <0>", test_file);

	std::string content = "Hello, RF::file_m!";
	RF::file_m::write_file(test_file, content);
	RF::logf::info("Wrote to file: <0>", content);

	std::string read_content = RF::file_m::read_file(test_file);
	RF::logf::info("Read from file: <0>", read_content);

	RF::bytes file_size = RF::file_m::file_size<RF::byte_scale>(test_file);
	RF::kibibytes size_kib = RF::file_m::file_size<RF::kibibyte_scale>(test_file);
	RF::mebibytes size_mib = RF::file_m::file_size<RF::mebibyte_scale>(test_file);
	RF::logf::info("File size: <0> bytes, <1> KiB, <2> MiB", file_size.count(), size_kib.count(), size_mib.count());

	bool exists = RF::file_m::exists(test_file);
	RF::logf::info("File exists: <0>", exists);

	std::vector<std::string> files = RF::file_m::files(test_dir);
	for (const auto& file : files)
	{
		RF::logf::info("Found file: <0>", file);
	}

	std::string renamed_file = test_dir + "/renamed.txt";
	RF::file_m::rename_file(test_file, renamed_file);
	RF::logf::info("Renamed file to: <0>", renamed_file);

	RF::file_m::delete_file(renamed_file);
	RF::logf::info("Deleted file: <0>", renamed_file);

	RF::file_m::open_file(test_dir);
	RF::logf::info("Opened directory in file explorer: <0>", test_dir);

	return 0;
}