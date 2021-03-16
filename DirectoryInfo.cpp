#include "DirectoryInfo.h"
#include <iostream>
namespace fs = std::filesystem;

DirectoryInfo::DirectoryInfo(const ArgSet& flags) {
	
	if (flags.find(Arguments::All) != flags.end())
		list_all = true;
	if (flags.find(Arguments::Directories) != flags.end())
		 only_dirs = true;
	if (flags.find(Arguments::Files) != flags.end())
		 only_files = true;
	if (flags.find(Arguments::GroupDirectories) != flags.end())
		 group_directories = true;
	if (flags.find(Arguments::HumanReadable) != flags.end())
		 list_human = true;
	if (flags.find(Arguments::List) != flags.end())
		 list_long = true;
	if (flags.find(Arguments::SortSize) != flags.end())
		 sort_size = true;
	if (flags.find(Arguments::SortTime) != flags.end())
		sort_time = true;
}


void DirectoryInfo::List(const fs::path& path) {
	
	if (sort_size && sort_time)
		sort_time = false;
	if (only_dirs) {
		ListDirectories();
		return;
	}
	if (only_files) {
		ListFiles();
		return;
	}
	if (group_directories) {
		ListDirectories();
		ListFiles();
	}
	for (const auto& entry : fs::directory_iterator(path)) {
		std::cout << entry.path().filename().string() << '\n';
	}
}
void DirectoryInfo::ListFiles() { }
void DirectoryInfo::ListDirectories() { }
void DirectoryInfo::Sort() { }