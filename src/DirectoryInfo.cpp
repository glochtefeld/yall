#include "DirectoryInfo.h"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>

#ifndef _WIN32
#include <iomanip>
#endif

namespace fs = std::filesystem;
namespace tme = std::chrono;

DirectoryInfo::DirectoryInfo(const FlagSet& flags) {
	for (auto arg : flags) {
		switch (arg) {
			case Flag::All:
				list_all = true;
				break;
			case Flag::Directories:
				only_dirs = true;
				break;
			case Flag::Files:
				only_files = true;
				break;
			case Flag::GroupDirectories:
				group_directories = true;
				break;
			case Flag::ListLong:
				list_long = true;
				break;
			case Flag::SortSize:
				sort_size = true;
				break;
			case Flag::SortTime:
				sort_time = true;
				break;
			case Flag::Help:
				std::cout 
					<< "A help flag has been passed to DirectoryInfo. Whoops!"
					<< std::endl;
				[[fallthrough]];
			default:
				throw std::invalid_argument("Bad flag list");
		}
	}
}


void DirectoryInfo::List(const fs::path& path) {
	std::vector<FileListing> files;
	for (const auto& entry : fs::directory_iterator(path)) {
		files.emplace_back(LongInfo(entry));
	}

	if (sort_size) {
		std::sort(files.begin(), files.end(),
			[](FileListing& a, FileListing& b)-> bool {
				return a.size < b.size;
			});
	}
	if (sort_time) {
		std::sort(files.begin(), files.end(),
			[](FileListing& a, FileListing& b)-> bool {
				return a.date < b.date;
			});
	}

	if (list_long) {
		std::cout << "Permissions:\tSize:\tLast Modified:\t\tName:\n";
	}

	if (only_dirs) {
		ListDirectories(files);
		return;
	}
	if (only_files) {
		ListFiles(files);
		return;
	}
	if (group_directories) {
		ListDirectories(files);
		ListFiles(files);
		return;
	}

	for (const auto& entry : files) {
		PrintListing(entry);
	}
}
void DirectoryInfo::ListFiles(const std::vector<FileListing>& files) {
	for (const auto& file : files) {
		if (!file.is_directory) {
			PrintListing(file);
		}
	}
}


void DirectoryInfo::ListDirectories(const std::vector<FileListing>& files) {
	for (const auto& entry : files) {
		if (entry.is_directory) {
			if (list_all || entry.name[0] != '.') {
				std::cout << entry.name << "\t<DIR>\n";
			}
		}
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

auto DirectoryInfo::PrettyFilesize(double fsize)->std::string {
	double hsize = fsize;
	auto byte_power = 0;
	const char* power_size[]{"B", "KB", "MB", "GB", "TB", "EB"};
	while (hsize > 1024.) {
		hsize /= 1024.;
		byte_power++;
	}
	hsize = std::ceil(hsize * 10.) / 10.;
	
	std::ostringstream size;
	size << hsize << power_size[byte_power];
	return size.str();
}

auto DirectoryInfo::PrettyDate(const fs::file_time_type& ftime)->std::string {
	std::ostringstream time;
	auto elapsed = tme::duration_cast<tme::seconds>(fs::file_time_type::clock::now().time_since_epoch() 
		- tme::system_clock::now().time_since_epoch()).count();
	time_t sys_time = tme::duration_cast<tme::seconds>(ftime.time_since_epoch()).count() - elapsed;

	// see note: Deprecation
#ifdef _WIN32
	struct tm new_time{};
	localtime_s(&new_time,&sys_time);
	time << std::setw(2) << new_time.tm_hour 
		<< ":" << std::setfill('0') << std::setw(2) << new_time.tm_min << ", "
		<< (new_time.tm_year + 1900) 
		<< "-" << std::setfill('0') << std::setw(2) << new_time.tm_mon
		<< "-" << std::setfill('0') <<  std::setw(2) << new_time.tm_mday;
#else
	struct tm* new_time = localtime(&sys_time);
	time << std::setw(2) << new_time->tm_hour
		<< ":" << std::setfill('0') << std::setw(2) << new_time->tm_min << ", "
		<< (new_time->tm_year + 1900)
		<< "-" << std::setfill('0') << std::setw(2) << new_time->tm_mon
		<< "-" << std::setfill('0') << std::setw(2) << new_time->tm_mday;
#endif
	return time.str();	
}

auto DirectoryInfo::LongInfo(const fs::directory_entry& entry)->FileListing {
	auto perms = entry.status().permissions();
#ifdef _WIN32
	auto size = entry.file_size();
#else
	auto size = (entry.is_directory()) ? 4096 : entry.file_size();
#endif
	auto date = entry.last_write_time();
	auto name = entry.path().filename().string();
	auto is_dir = entry.is_directory();
	FileListing fl{ perms, size, date, name, is_dir };
	return fl;
}

void DirectoryInfo::PrintListing(const FileListing& file) {
	// see note: Truth Table
	if (list_all || file.name[0] != '.') {
		if (list_long) {
			std::cout << PrettyPermissions(file.perms) << '\t'
				<< PrettyFilesize(file.size) << '\t'
				<< PrettyDate(file.date) << '\t'
				<< file.name << '\n';
		}
		else {
			std::cout << file.name << '\n';
		}
	}
}

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

Deprecation:
	localtime is unsafe and going to be deprecated by MSVC, localtime_s
	is preferred. However, clang complains about localtime_s, 
	so we get preprocessor logic blocks! 

*/