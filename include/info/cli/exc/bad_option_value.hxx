/** InfoCLI project
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * The exception thrown if an option is not provided with the required arguments
 * or these arguments are just plain wrong in all ways imaginable. In any case
 * we can't do anything with them so just yell at the user about it
 */
#pragma once

#include <exception>
#include <string>

#include <info/cli/macros.hxx>

namespace info::cli {
    struct INFO_CLI_API bad_option_value : std::exception {
        [[nodiscard]] const char* what() const noexcept override;

        explicit bad_option_value(std::string opt_name, std::string_view type, std::string args);

        std::string opt_name;
        std::string_view type;
        std::string args;
    };
}
