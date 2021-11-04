#pragma once

#include <map>
#include <vector>
#include <string>
#include <variant>

namespace common::cli
{
	class parser_exception : public std::exception
	{
	public:
		parser_exception(const std::string &message)
			: std::exception(message.c_str())
		{}
	};

	struct option_info_t
	{
		std::string name;
		std::string alias;
		std::string description;
		bool must_has_value;
	};

	class args_parser final
	{
	public:
		args_parser(const std::vector<option_info_t>& supported_options, const std::vector<std::string> &usage_examples);
		~args_parser() = default;

		args_parser(const args_parser&) = delete;
		args_parser& operator = (const args_parser&) = delete;
		args_parser(args_parser&&) = delete;
		args_parser& operator = (args_parser&&) = delete;

	public:
		std::map<std::string, std::string> process_cmd_line(const std::vector<std::string>& args) const;

	private:
		std::vector<option_info_t> m_supported_options;
		std::vector<std::string>   m_usage_examples;
	};
}