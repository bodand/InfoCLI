/*
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Implements the no_such_option exception
 */

#include <info/cli/exc/callback_error.hxx>

#include <fmt/format.h>

const char*
info::cli::callback_error::what() const noexcept {
    static std::string ret;
    ret = fmt::format("error: error encountered when processing option '{}' with value '{}'",
                      opt_name,
                      args);
    return ret.c_str();
}

info::cli::callback_error::callback_error(std::string opt_name, std::string args)
     : opt_name(std::move(opt_name)),
       args(std::move(args)) {
}
