/** InfoCLI project
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Defines the no_such_option exception thrown if a given option is not a known
 * option
 */
#pragma once

#include <exception>
#include <string>

#include <info/cli/macros.hxx>

namespace info::cli {
    struct INFO_CLI_API no_such_option : std::exception {
        [[nodiscard]] const char* what() const noexcept override;

        explicit no_such_option(std::string opt_name);

        std::string opt_name;
    };
}
