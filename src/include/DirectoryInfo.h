#ifndef DIRECTORYINFO_H
#define DIRECTORYINFO_H
#include "Arguments.h"
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_set>

using ArgSet = std::unordered_set<Arguments>;

struct FileListing {
	std::filesystem::perms perms;
	uintmax_t size;
	std::filesystem::file_time_type date;
	std::string name;
	// bool operator <(const FileListing& other) const { return true; }
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
	explicit DirectoryInfo(const ArgSet&);
	void List(const std::filesystem::path&);
private:
	void ListFiles(const std::filesystem::path&);
	static void ListDirectories(const std::filesystem::path&);
	static auto PrettyPermissions(std::filesystem::perms)->std::string;
	static auto PrettyFilesize(double)->std::string;
	static auto PrettyDate(const std::filesystem::file_time_type&)->std::string;
	auto LongInfo(const std::filesystem::directory_entry&)->FileListing;
};



#endif
