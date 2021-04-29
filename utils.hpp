#pragma once

#include <filesystem>

namespace fs = std::filesystem;

namespace client {

	inline void print_usage(const char* execute_path)
	{
		const auto app_name = fs::path(execute_path).filename().string();
		std::cout << "Usage " << app_name << "[options] messages...\n"
			<< "\t-h - Hostname or address sent messages.\n"
			<< "\t-p - Port for connection.\n";
	}

	inline void parse_client_args(int argc,
		char* argv[],
		std::string& host,
		std::uint16_t& port,
		std::vector<std::string>& messages)
	{
		for (int i = 1; i < argc; ++i)
		{
			if (std::strcmp(argv[i], "-p") == 0
				&& ++i < argc)
			{
				port = std::atoi(argv[i]);
			}
			else if (std::strcmp(argv[i], "-h") == 0
				&& ++i < argc)
			{
				host = argv[i];
			}
			else
				messages.push_back(argv[i]);
		}
	}
}