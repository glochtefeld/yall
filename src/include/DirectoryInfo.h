#ifndef DIRECTORYINFO_H
#define DIRECTORYINFO_H
#include "Arguments.h"
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_set>

using ArgSet = std::unordered_set<Arguments>;
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
	void ListDirectories(const std::filesystem::path&);
	auto PrettyPermissions(std::filesystem::perms)->std::string;
	auto PrettyFilesize(const std::filesystem::directory_entry&)->std::string;
	auto PrettyDate(const std::filesystem::path&)->std::string;
	void Sort();
};

#endif
