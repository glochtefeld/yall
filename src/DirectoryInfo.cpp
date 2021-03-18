#include "include/DirectoryInfo.h"
#include <chrono>
#include <iostream>
#include <vector>
namespace fs = std::filesystem;
namespace tme = std::chrono;

DirectoryInfo::DirectoryInfo(const ArgSet& flags) {
	for (auto arg : flags) {
		switch (arg) {
			case Arguments::All:
				list_all = true;
				break;
			case Arguments::Directories:
				only_dirs = true;
				break;
			case Arguments::Files:
				only_files = true;
				break;
			case Arguments::GroupDirectories:
				group_directories = true;
				break;
			case Arguments::ListLong:
				list_long = true;
				break;
			case Arguments::SortSize:
				sort_size = true;
				break;
			case Arguments::SortTime:
				sort_time = true;
				break;
			default:
				throw "Unknown argument passed";
		}
	}
}


void DirectoryInfo::List(const fs::path& path) {
	if (list_long) {
		std::cout << "Permissions:\tSize:\tLast Modified:\t\tName:\n";
		ListFiles(path);
		return;
	}

	if (only_dirs) {
		ListDirectories(path);
		return;
	}
	if (only_files) {
		ListFiles(path);
		return;
	}
	if (group_directories) {
		ListDirectories(path);
		ListFiles(path);
		return;
	}
	
	for (const auto& entry : fs::directory_iterator(path)) {
		// see truth table at end of file
		if (list_all || entry.path().filename().string()[0] != '.')
			std::cout << entry.path().filename().string() << '\n';
	}
}
void DirectoryInfo::ListFiles(const fs::path& path) {
	for (const auto& entry : fs::directory_iterator(path)) {
		if (entry.is_regular_file()) {
			if (list_long) {
				auto perms = PrettyPermissions(entry.status().permissions());
				auto size = PrettyFilesize(entry);
				auto date = PrettyDate(entry.path());
				auto name = entry.path().filename().string();
				std::cout << perms << '\t' << size << '\t' << date << "\t" << name << '\n';
			}
			else
				std::cout << entry.path().filename().string() << '\n';
		}
	}
}


void DirectoryInfo::ListDirectories(const fs::path& path) {
	for (const auto& entry : fs::directory_iterator(path)) {
		if (entry.is_directory())
			std::cout << entry.path().filename().string() << '\n';
	}
}

// taken from cppreference.com 
auto DirectoryInfo::PrettyPermissions(const fs::perms p)->std::string {
	std::ostringstream permissions;
	permissions << ((p & fs::perms::owner_read) != fs::perms::none ? "r" : "-")
		<< ((p & fs::perms::owner_write) != fs::perms::none ? "w" : "-")
		<< ((p & fs::perms::owner_exec) != fs::perms::none ? "x" : "-")
		<< ((p & fs::perms::group_read) != fs::perms::none ? "r" : "-")
		<< ((p & fs::perms::group_write) != fs::perms::none ? "w" : "-")
		<< ((p & fs::perms::group_exec) != fs::perms::none ? "x" : "-")
		<< ((p & fs::perms::others_read) != fs::perms::none ? "r" : "-")
		<< ((p & fs::perms::others_write) != fs::perms::none ? "w" : "-")
		<< ((p & fs::perms::others_exec) != fs::perms::none ? "x" : "-");
	return permissions.str();
}

auto DirectoryInfo::PrettyFilesize(const fs::directory_entry& entry)->std::string {
	double fsize = entry.file_size();
	auto byte_power = 0;
	const char* power_size[]{"B", "KB", "MB", "GB", "TB", "EB"};
	while (fsize > 1024.) {
		fsize /= 1024.;
		byte_power++;
	}
	fsize = std::ceil(fsize * 10.) / 10.;
	
	std::ostringstream size;
	size << fsize << power_size[byte_power];
	return size.str();
}

auto DirectoryInfo::PrettyDate(const fs::path& file)->std::string {
	std::ostringstream time;
	auto ftime = fs::last_write_time(file);
	auto elapsed = tme::duration_cast<tme::seconds>(fs::file_time_type::clock::now().time_since_epoch() 
		- tme::system_clock::now().time_since_epoch()).count();
	time_t sys_time = tme::duration_cast<tme::seconds>(ftime.time_since_epoch()).count() - elapsed;
	struct tm new_time;
	localtime_s(&new_time,&sys_time); 
	time << std::setw(2) << new_time.tm_hour 
		<< ":" << std::setfill('0') << std::setw(2) << new_time.tm_min << ", "
		<< (new_time.tm_year + 1900) 
		<< "-" << std::setfill('0') << std::setw(2) << new_time.tm_mon
		<< "-" << std::setfill('0') <<  std::setw(2) << new_time.tm_mday;
	return time.str();
	
}

void DirectoryInfo::Sort() { }

/*
Truth table:
	a = list_all
	p = begins with period (hidden)
	| a | p | !p | a V !p |
	:---:---:----:--------:
	| T | T | F  |   T    |
	| T | F | T  |   T    |
	| F | T | F  |   F    |
	| F | F | T  |   T    |

*/