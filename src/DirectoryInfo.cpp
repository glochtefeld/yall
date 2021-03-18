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
			case Arguments::Help:
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
	// 
	for (const auto& entry : fs::directory_iterator(path)) {
		// see truth table at end of file
		if (list_all || entry.path().filename().string()[0] != '.') {
			std::cout << entry.path().filename().string() << '\n';
		}
	}
}
void DirectoryInfo::ListFiles(const fs::path& path) {
	std::vector<FileListing> files;
	for (const auto& entry : fs::directory_iterator(path)) {
		if (entry.is_regular_file()) {
			if (list_long) {
				if (sort_time || sort_size) {
					files.emplace_back(LongInfo(entry));
					continue;
				}
				else {
					auto file = LongInfo(entry);
					if (list_all || file.name[0] != '.')
						std::cout << PrettyPermissions(file.perms) << '\t' 
							<< PrettyFilesize(file.size) << '\t' 
							<< PrettyDate(file.date) << "\t" 
							<< file.name << '\n';
				}
			}
			else {
				if (list_all || entry.path().filename().string()[0] != '.')
					std::cout << entry.path().filename().string() << '\n';
			}
		}
	}
	if (sort_size)
		std::sort(files.begin(), files.end(), 
			[](FileListing a,FileListing b)-> bool {
				return a.size < b.size;
			});
	if (sort_time)
		std::sort(files.begin(), files.end(),
			[](FileListing a,FileListing b)-> bool {
				return a.date < b.date;
			});
	std::for_each(files.cbegin(), files.cend(), [this](auto file) {
		if (list_all || file.name[0] != '.')
			std::cout << PrettyPermissions(file.perms) << '\t'
				<< PrettyFilesize(file.size) << '\t'
				<< PrettyDate(file.date) << "\t"
				<< file.name << '\n';
		});

}


void DirectoryInfo::ListDirectories(const fs::path& path) {
	for (const auto& entry : fs::directory_iterator(path)) {
		if (entry.is_directory()) {
			std::cout << entry.path().filename().string() << "\t<DIR>\n";
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
	struct tm new_time {};
	localtime_s(&new_time,&sys_time); 
	time << std::setw(2) << new_time.tm_hour 
		<< ":" << std::setfill('0') << std::setw(2) << new_time.tm_min << ", "
		<< (new_time.tm_year + 1900) 
		<< "-" << std::setfill('0') << std::setw(2) << new_time.tm_mon
		<< "-" << std::setfill('0') <<  std::setw(2) << new_time.tm_mday;
	return time.str();
	
}

auto DirectoryInfo::LongInfo(const fs::directory_entry& entry)->FileListing {
	auto perms = entry.status().permissions();
	auto size = entry.file_size();
	auto date = entry.last_write_time();
	auto name = entry.path().filename().string();
	FileListing fl{ perms,size,date,name };
	return fl;
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

*/