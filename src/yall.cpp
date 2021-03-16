#include "include/argparser.h"
#include "include/DirectoryInfo.h"
#include <iostream>
#include <algorithm>
#include <regex>

auto main(int argc, char* argv[]) -> int {
	if (argc == 1) {
		// list contents normally
		DirectoryInfo di;
		di.List(std::filesystem::current_path());
		return 0;
	}
	
	std::vector<std::string> args; 
	args.reserve(argc);
	std::vector<std::string> filenames;
	filenames.reserve(argc);

	// unavoidable pointer arithmetic
	for (int i = 1; i != argc; ++i) {
		if (*argv[i] == '-')
			args.push_back({ argv[i] });
		else
			filenames.push_back({ argv[i] });
	}
	try {
		auto parsed = parse_args(args);
		DirectoryInfo di{ parsed };
#ifdef _WIN64 
		std::regex root("$[A-Z]:");
#else
		std::regex root("$/");
#endif

		std::regex rel("$\.\.");
		if (filenames.size() > 0)
			for (const auto path : filenames) {
				std::filesystem::path as_path;
				if (std::regex_search(path, root))
					as_path = std::filesystem::path{ path };
				else if (std::regex_search(path, rel)) {
					std::ostringstream rel_path;
					rel_path << "\"" << path << "\"";
					as_path = std::filesystem::path{ rel_path.str() };
				}
				else
					as_path = std::filesystem::current_path() / path;
				std::cout << as_path.filename().string() << '\n';
				di.List(as_path);
			}
		else {
			di.List(std::filesystem::current_path());
		}
		return 0;
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return 0;
	}
	
}