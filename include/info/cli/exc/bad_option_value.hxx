/*
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
    /**
     * \brief Exception for invalid value for an option
     *
     * Thrown when the parser can't contort the string into the appropriate
     * type for the option. This is also thrown when no value was given, yet
     * one was expected.
     */
    struct INFO_CLI_API bad_option_value : std::exception {
        /**
         * \brief Returns a string explaining the error
         *
         * \return A string explaining the error which can be printed as-is, if need be
         */
        [[nodiscard]] const char* what() const noexcept override;

        /**
         * \brief Constructs a bad_option_value
         *
         * Takes the required value and constructs a bad_optoin_value exception.
         * Type is expected to be a string_view to a type_data object's static
         * members, thus it does not copy the whole string, be aware of that.
         *
         * \param opt_name The name of the option whose value failed parsing
         * \param type The name of the type expected for the option
         * \param args The actually found value
         */
        explicit bad_option_value(std::string opt_name, std::string_view type, std::string args);

        std::string opt_name; ///< The name of the failed option
        std::string_view type;///< The type of the failed option
        std::string args;     ///< What the parser found for the value for the option
    };
}
