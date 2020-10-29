/*
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
    /**
     * \brief Exception for when the the callback fails
     *
     * The callbacks may fail for reasons like the parser function returning
     * failure, or for any other, really. No, the user cannot make it fail atm.
     * If this happens, this exception is thrown.
     */
    struct INFO_CLI_API callback_error : std::exception {
        /// \copydoc bad_option_value::what()
        [[nodiscard]] const char* what() const noexcept override;

        /**
         * \brief Contsructs a callback_error exception
         *
         * Takes the option whose callback failed and the argument it
         * failed on and creates an exception instance.
         *
         * \param opt_name The options whose callback returned false
         * \param args The arguments for the current opion. Likely caused the failure
         */
        explicit callback_error(std::string opt_name, std::string args);

        std::string opt_name;///< The name of the failed callback's option
        std::string args;    ///< The value for which the callback failed
    };
}
