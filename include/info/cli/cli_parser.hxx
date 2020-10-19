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
#include <utility>
#include <vector>

#include <info/cli/macros.hxx>
#include <info/cli/option.hxx>
#include <info/cli/types/type_data.hxx>

namespace info::_cli {
    template<class Callback>
    struct INFO_CLI_LOCAL help_text {
        std::string help_msg;
        std::vector<Callback> callbacks;

        help_text(std::string help_msg,
                  std::vector<Callback>&& callbacks)
             : help_msg(std::move(help_msg)),
               callbacks(std::move(callbacks)) { }

        bool
        operator==(const help_text& other) const {
            return help_msg == other.help_msg;
        }
    };
}

namespace std {
    template<class Callback>
    struct hash<info::_cli::help_text<Callback>> {
        std::size_t
        operator()(const info::_cli::help_text<Callback>& txt) const {
            return ::std::hash<std::string>{}(txt.help_msg);
        }
    };
}

namespace info::cli {
    struct INFO_CLI_API cli_parser {
        std::vector<std::string_view> operator()(int argc, char** argv);
        INFO_CLI_PURE [[nodiscard]] std::size_t size() const noexcept;

        cli_parser(std::initializer_list<option> opts);

    private:
        struct INFO_CLI_LOCAL option_info {
            rt_type_data type_data;
            std::size_t callback;
        };

        using callback_type = std::function<bool(std::string_view, const char*&)>;
        using options_type = std::unordered_map<std::string, option_info>;
        using help_innards = std::pair<std::string_view, const option_info*>;
        using help_type = _cli::help_text<help_innards>;

        std::vector<callback_type> _callbacks;
        options_type _options;
        std::unordered_set<help_type> _helps;

        void short_option(char* arg, int argc, char** argv, int& i);
        void unpacked_shorts(char* arg, int argc, char** argv, int& i);
        void packed_shorts(char* arg, int argc, char** argv, int& i);
        void long_option(int argc, char** argv, int& i);

        INFO_CLI_PURE [[nodiscard]] static char* strip_option(char* opt, bool lng = false);
    };

}
