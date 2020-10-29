/*
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 *
 */
#include <info/cli/exc/bad_option_value.hxx>

#include <fmt/format.h>

const char*
info::cli::bad_option_value::what() const noexcept {
    static std::string str;
    str = fmt::format("error: unintelligible value provided for option '{}' (expecting type {}): {}",
                      opt_name,
                      type,
                      args);
    return str.c_str();
}

info::cli::bad_option_value::bad_option_value(std::string opt_name,
                                              std::string_view type,
                                              std::string args)
     : opt_name(std::move(opt_name)),
       type(type),
       args(std::move(args)) {
}
