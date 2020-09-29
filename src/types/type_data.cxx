/** InfoCLI project
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Implements the functions from the type_data header.
 */

#include <info/cli/types/type_data.hxx>

#include <cctype>

bool
info::cli::rt_type_data::finite() const noexcept {
    return length != -1;
}

std::function<bool(char)>
info::cli::parse_type_accepts(info::cli::parse_type type) {
    switch (type) {
    case parse_type::alpha:
        return [](char ch) {
            return std::isalpha(ch);
        };
    case parse_type::alphanumeric:
        return [](char ch) {
            return std::isalnum(ch);
        };
    case parse_type::numeric:
        return [](char ch) {
            return std::isdigit(ch);
        };
    case parse_type::printable:
        return [](char ch) {
            return std::isprint(ch);
        };
    case parse_type::any:
        return [](char) {
            return true;
        };
    }
    INFO_CLI_NOT_HAPPENING;
}
