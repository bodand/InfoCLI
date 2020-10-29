/*
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Implements the no_such_option exception
 */

#include <info/cli/exc/no_such_option.hxx>

#include <fmt/format.h>

const char*
info::cli::no_such_option::what() const noexcept {
    static std::string ret;
    ret = fmt::format("error: unknown option encountered while parsing command line: '{}'", opt_name);
    return ret.c_str();
}

info::cli::no_such_option::no_such_option(std::string opt_name)
     : opt_name(std::move(opt_name)) {
}
