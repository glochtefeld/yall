#include "argparser.h"
#include <iostream>
#include <algorithm>

auto parse_args(const StrVec& args) -> ArgSet {
	std::unordered_set<Arguments> parsed;
	try {
		std::for_each(args.cbegin(), args.cend(),
			[&parsed](auto arg) {
				// we're dealing with an arg like -Adfghls
				if (arg.size() > 2) {
					auto arg_list = arg.substr(1);
					for (auto& ch : arg_list)
						parsed.insert(check_arg(ch));
				}
				else {
					parsed.insert(check_arg(arg[1]));
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

auto check_arg(const char arg)->Arguments {
	switch (arg) {
	case 'A':
	case 'a':
		return Arguments::All;
	case 'd':
		return Arguments::Directories;
	case 'f':
		return Arguments::Files;
	case 'g':
		return Arguments::GroupDirectories;
	case 'h':
		return Arguments::HumanReadable;
	case 'l':
		return Arguments::List;
	case 's':
		return Arguments::SortSize;
	case 't':
		return Arguments::SortTime;
	default:
		throw std::invalid_argument(&arg);
	}
}

/*Args to support:
 -A: show hidden files (hiding implicit . and ..)
 -d: show only directories
 -f: show only files
 -g: group directories first
 -h: human readable file sizes (used with -l)
 -l: list files with permissions, owner, file size, date, and filename
 -s: sort by file size
 -t: sort by time
 */