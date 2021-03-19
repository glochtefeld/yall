#include "include/argparser.h"
#include <algorithm>
#include <iostream>

auto parse_flags(const StrVec& args) -> FlagSet {
	std::unordered_set<Flag> parsed;
	try {
		std::for_each(args.cbegin(), args.cend(),
			[&parsed](auto arg) {
				// we're dealing with an arg like -Adfghls
				if (arg.size() > 2) {
					auto arg_list = arg.substr(1);
					for (auto& ch : arg_list)
						parsed.insert(check_flag(ch));
				}
				else {
					parsed.insert(check_flag(arg[1]));
				}
			});
	}
	catch (const std::invalid_argument& e) {
		std::string errm = "Bad argument: -";
		errm.append(e.what());
		throw std::invalid_argument(errm.c_str());
	}
	return parsed;
}

auto check_flag(const char arg)->Flag {
	switch (arg) {
	case 'A':
	case 'a':
		return Flag::All;
	case 'd':
		return Flag::Directories;
	case 'f':
		return Flag::Files;
	case 'g':
		return Flag::GroupDirectories;
	case 'h':
		return Flag::Help;
	case 'l':
		return Flag::ListLong;
	case 's':
		return Flag::SortSize;
	case 't':
		return Flag::SortTime;
	default:
		throw std::invalid_argument(&arg);
	}
}

/*Args to support:
 -A: show hidden files (hiding implicit . and ..)
 -d: show only directories
 -f: show only files
 -g: group directories first
 -l: list files with permissions, file size, date, and filename
 -s: sort by file size
 -t: sort by time
 */