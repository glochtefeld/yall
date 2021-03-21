#include "argparser.h"
#include "DirectoryInfo.h"
#include "Flag.h"
#include <algorithm>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

#ifdef _WIN32
const char flag_marker = '/';
#else
const char flag_marker = '-';
#endif

void print_help(std::string& name) {
	std::cout << "Usage: "<<name<<" [flags] [files]\n"
		<< "Possible Flags:\n"
		<< "\t"<<flag_marker<<"A,"<<flag_marker<<"a\n\t\tShow hidden files (hiding implicit . and ..)\n"
		<< "\t"<<flag_marker<<"d\n\t\tShow only directories\n"
		<< "\t"<<flag_marker<<"f\n\t\tShow only files\n"
		<< "\t"<<flag_marker<<"g\n\t\tGroup directories first\n"
		<< "\t"<<flag_marker<<"h\n\t\tPrint this message and exit\n"
		<< "\t"<<flag_marker<<"l\n\t\tList files with permissions, human readable file size, date, and filename\n"
		<< "\t"<<flag_marker<<"s\n\t\tSort by file size\n"
		<< "\t"<<flag_marker<<"t\n\t\tSort by modified time"
		<< std::endl;
}

auto main(int argc, char* argv[]) -> int {
	if (argc == 1) {
		// list contents normally
		DirectoryInfo di;
		di.List(std::filesystem::current_path());
		return 0;
	}

	std::vector<std::string> args(argv, argv + argc);
	std::string name = args[0];
	args.erase(args.begin());

	std::vector<std::string> flags;
	std::vector<std::string> filenames;
	std::for_each(args.cbegin(), args.cend(), [&flags,&filenames](auto& arg) {
		if (arg[0] == flag_marker) {
			flags.push_back(arg);
		}
		else {
			filenames.push_back(arg);
		}
		});

	try {
		auto parsed = parse_flags(flags);

		if (parsed.find(Flag::Help) != parsed.end()) {
			print_help(name);
			return 0;
		}

		DirectoryInfo di{ parsed };
#ifdef _WIN64 
		std::regex root("$[A-Z]:");
#else
		std::regex root("$/");
#endif

		std::regex rel("$\.\.");
		if (!filenames.empty()) {
			for (auto& path : filenames) {
				std::filesystem::path as_path;
				if (std::regex_search(path, root)) {
					as_path = std::filesystem::path{ path };
				}
				else if (std::regex_search(path, rel)) {
					std::ostringstream rel_path;
					rel_path << "\"" << path << "\"";
					as_path = std::filesystem::path{ rel_path.str() };
				}
				else {
					as_path = std::filesystem::current_path() / path;
				}
				std::cout << as_path.filename().string() << ":\n";
				di.List(as_path);
			}
		}
		else {
			di.List(std::filesystem::current_path());
		}
		return 0;
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		print_help(name);
		return 0;
	}
	
}