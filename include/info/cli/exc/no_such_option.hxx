/*
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
    /**
     * \brief Exception for when an option does not exists
     *
     * When the parser encounters an option (short or long, packed or not,
     * GNU-style or not), which was not provided during the parsers construction,
     * this exception is thrown.
     */
    struct INFO_CLI_API no_such_option : std::exception {
        /// \copydoc bad_option_value::what()
        [[nodiscard]] const char* what() const noexcept override;

        /**
         * \brief Constructs a no_such_option exception
         *
         * Takes the option which the parser was unable to recognize and
         * creates an exception.
         *
         * \param opt_name The unknown option
         */
        explicit no_such_option(std::string opt_name);

        std::string opt_name;///< The name of the unrecognized option
    };
}
