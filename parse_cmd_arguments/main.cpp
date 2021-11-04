// parse_cmd_arguments.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "command_line_parser.h"

int main(int argc, char *argv[])
{
	try
	{
		common::cli::args_parser parser(
		{
			{ "-c", "--command", "Command of", true },
			{ "-d", "", "Not wait", false },
			{ "-p", "--path", "Path of", true },
			{ "-trace", "", "Trace request", false }
		}, {});

		std::vector<std::string> args(argv, argv + argc);
		const auto& options = parser.process_cmd_line(args);

		if (!options.empty())
		{
			std::cout << "Command line options: " << std::endl;
			for (const auto& op : options)
			{
				std::cout << op.first << " " << op.second << std::endl;
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}
