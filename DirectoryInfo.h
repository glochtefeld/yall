#ifndef DIRECTORYINFO_H
#define DIRECTORYINFO_H
#include "Arguments.h"
#include <filesystem>
#include <vector>
#include <string>
#include <unordered_set>
using ArgSet = std::unordered_set<Arguments>;
class DirectoryInfo
{
	bool list_all = false;
	bool only_dirs = false;
	bool only_files = false;
	bool group_directories = false;
	bool list_human = false;
	bool list_long = false;
	bool sort_time = false;
	bool sort_size = false;
public:
	DirectoryInfo() {};
	explicit DirectoryInfo(const ArgSet&);
	void List(const std::filesystem::path&);
private:
	void ListFiles();
	void ListDirectories();
	void Sort();
};
#endif
