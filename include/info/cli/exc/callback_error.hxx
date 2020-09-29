/** InfoCLI project
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Defines callback_error exception thrown when a user defined callback explicitly
 * returned false to terminate processing of cli arguments
 */
#pragma once

#include <exception>
#include <string>

#include <info/cli/macros.hxx>

namespace info::cli {
    struct INFO_CLI_API callback_error : std::exception {
        [[nodiscard]] const char* what() const noexcept override;

        explicit callback_error(std::string opt_name, std::string args);

        std::string opt_name;
        std::string args;
    };
}
