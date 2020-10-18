/** MakeMake project
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Interface of the main (and only) public entry point to the library: cli_parser.
 * This is used to define the options, and then parse the command line.
 */
#pragma once

#include <functional>
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <info/cli/macros.hxx>
#include <info/cli/option.hxx>
#include <info/cli/types/type_data.hxx>

namespace info::cli {
    struct INFO_CLI_API cli_parser {
        std::vector<std::string_view> operator()(int argc, char** argv);
        INFO_CLI_PURE [[nodiscard]] std::size_t size() const noexcept;

        cli_parser(std::initializer_list<option> opts);

    private:
        struct INFO_CLI_LOCAL option_info {
            rt_type_data type_data;
            std::size_t callback;
            std::string_view help;
        };

        std::vector<std::function<bool(std::string_view, const char*&)>> _callbacks;
        std::unordered_set<std::string> _helps;
        std::unordered_map<std::string, option_info> _options;

        void short_option(char* arg, int argc, char** argv, int& i);
        void unpacked_shorts(char* arg, int argc, char** argv, int& i);
        void packed_shorts(char* arg, int argc, char** argv, int& i);
        void long_option(int argc, char** argv, int& i);

        INFO_CLI_PURE [[nodiscard]] static char* strip_option(char* opt, bool lng = false);
    };
}
