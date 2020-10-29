/*
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Implements the option member functions
 */

#include <info/cli/option.hxx>
#include <utility>

info::_cli::option_builder info::cli::udl::operator""_opt(const char* str, std::size_t size) {
    return {{str, size}};
}

info::_cli::option_builder info::cli::udl::operator""_opt(char ch) {
    return {std::string_view{std::string(1, ch)}};
}

info::cli::option::option(std::string help,
                          std::function<bool(std::string_view, const char*&)> callback,
                          std::vector<std::string>&& names,
                          rt_type_data type)
     : names(std::move(names)),
       type(type),
       help(std::move(help)),
       callback(std::move(callback)) { }
