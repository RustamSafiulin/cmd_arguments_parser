
#include <iostream>
#include <algorithm>
#include <string_view>
#include <sstream>
#include <stdexcept>

#include "command_line_parser.h"

namespace common::cli
{
	using namespace std::literals;

	constexpr std::string_view help_option_name = "-help"sv;
	constexpr std::string_view help_option_alias_name = "--help"sv;

	constexpr std::size_t minimal_args_count = 2;

	namespace
	{
		template <typename ...Args>
		void throw_parser_error(const std::string& message, Args ...args)
		{
			std::ostringstream ss;
			ss << message << " ";
			((ss << args), ...);

			const std::string error_message = ss.str();
			throw parser_exception(error_message);
		}

		void show_help(const std::vector<option_info_t>& supported_options, const std::vector<std::string>& usage_examples)
		{
			std::cout << "=============Usage=============" << std::endl;

			for (const auto& option : supported_options)
			{
				std::cout << option.name;
				if (!option.alias.empty()) std::cout << "  " << option.alias;
				if (!option.description.empty()) std::cout << " - " << option.description;
				std::cout << std::endl;
			}
			
			if (!usage_examples.empty())
			{
				std::cout << std::endl;
				
				for (const auto& example_string : usage_examples)
				{
					std::cout << example_string << std::endl;
				}
			}
		}

		void validate_supported_options(const std::vector<option_info_t>& supported_options)
		{
			if (supported_options.empty())
			{
				throw_parser_error("Empty supported options was passed.");
			}

			bool has_invalid_option_name = std::any_of(std::cbegin(supported_options), std::cend(supported_options), [](const option_info_t& info) 
			{
				return info.name.empty();
			});

			if (has_invalid_option_name)
			{
				throw_parser_error("Invalid parser supported options was passed.");
			}
		}
	}

	args_parser::args_parser(const std::vector<option_info_t>& supported_options, const std::vector<std::string>& usage_examples)
		: m_supported_options(supported_options)
		, m_usage_examples(usage_examples)
	{
		validate_supported_options(m_supported_options);
	}

	std::map<std::string, std::string> args_parser::process_cmd_line(const std::vector<std::string>& args) const
	{
		if (args.size() < minimal_args_count)
		{
			show_help(m_supported_options, m_usage_examples);
			return {};
		}

		if (args.size() == minimal_args_count && (args.at(1) == std::string{ help_option_name } || args.at(1) == std::string{ help_option_alias_name }))
		{
			show_help(m_supported_options, m_usage_examples);
		}

		std::map<std::string, std::string> options_map;

		const auto is_key_param = [](const std::string& param)
		{
			const auto key_feature_find_pos = param.find("-");
			const auto key_feature_alias_find_pos = param.find("--");

			bool is_key = (key_feature_find_pos == 0 && param.size() > 1);
			bool is_key_alias = (key_feature_alias_find_pos == 0 && param.size() > 2);

			return is_key || is_key_alias;
		};

		for (auto it = std::cbegin(args) + 1; it != std::cend(args); ++it)
		{
			const auto& param_name = *it;
			if (param_name.empty())
				continue;

			if (is_key_param(param_name))
			{
				const auto param_value_it = std::next(it);
				bool has_value = (param_value_it != std::cend(args) && !is_key_param(*param_value_it));

				const auto find_iter = std::find_if(std::cbegin(m_supported_options), std::cend(m_supported_options), [&](const option_info_t& option_info)
				{
					return (option_info.name == param_name || option_info.alias == param_name);
				});

				if (find_iter == std::cend(m_supported_options))
				{
					throw_parser_error("Unknown argument:", param_name);
				}

				if (find_iter->must_has_value && !has_value)
				{
					throw_parser_error("Argument must have value. Argument name:", param_name);
				}

				const std::string& param_value = has_value ? *param_value_it : std::string{};
				options_map.emplace(param_name, param_value);
			}
		}

		return options_map;
	}
}