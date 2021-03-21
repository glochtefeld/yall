#ifndef INCLUDE_DIRECTORYINFO_H
#define INCLUDE_DIRECTORYINFO_H
#include "Flag.h"
#include <filesystem>
#include <string>
#include <unordered_set>
#include <vector>

using FlagSet = std::unordered_set<Flag>;

struct FileListing {
	std::filesystem::perms perms;
	uintmax_t size;
	std::filesystem::file_time_type date;
	std::string name;
	bool is_directory;
};

class DirectoryInfo
{
	bool list_all = false;
	bool only_dirs = false;
	bool only_files = false;
	bool group_directories = false;
	bool list_long = false;
	bool sort_time = false;
	bool sort_size = false;
public:
	DirectoryInfo() = default;
	explicit DirectoryInfo(const FlagSet&);
	void List(const std::filesystem::path&);
private:
	void ListFiles(const std::vector<FileListing>&);
	void ListDirectories(const std::vector<FileListing>&);
	static auto PrettyPermissions(std::filesystem::perms)->std::string;
	static auto PrettyFilesize(double)->std::string;
	static auto PrettyDate(const std::filesystem::file_time_type&)->std::string;
	static auto LongInfo(const std::filesystem::directory_entry&)->FileListing;
	void PrintListing(const FileListing&);
};



#endif
