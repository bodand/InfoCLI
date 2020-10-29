/* InfoCLI project
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Interface of the main (and only) public entry point to the library: cli_parser.
 * This is used to define the options, and then parse the command line.
 */
#pragma once

#include <functional>
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <info/cli/macros.hxx>
#include <info/cli/option.hxx>
#include <info/cli/types/type_data.hxx>

namespace info::_cli {
    /**
     * \brief A type that is used to model the one-to-many relationship of
     *        help descriptions and options
     *
     * A struct which stores a given help message and all the options, in a
     * templated callback format, which are to be described with that help
     * description.
     *
     * \tparam Callback The type which is to be used to refer to the options
     */
    template<class Callback>
    struct INFO_CLI_LOCAL help_text {
        std::string help_msg;           ///< The help message
        std::vector<Callback> callbacks;///< The vector of callbacks the help message applies to

        /**
         * \brief Constructs a help_text object for use
         *
         * Takes the help message and the moves vector of callbacks associated
         * with that message into the newly constructed object.
         *
         * A callback in this context are the whole option descriptors,
         * which are referred back to the help message stored here.
         *
         * \param help_msg The message applied to the callbacks
         * \param callbacks A vector of callbacks which share one help description
         */
        help_text(std::string help_msg,
                  std::vector<Callback>&& callbacks)
             : help_msg(std::move(help_msg)),
               callbacks(std::move(callbacks)) { }

        /**
         * \brief An equality operator for help_text objects
         *
         * Compares two help_text objects for equality. Two help_texts are
         * considered equal if their string members are equal.
         *
         * \param other The object to check equality with
         *
         * \return Whether the to objects are equal
         */
        bool
        operator==(const help_text& other) const {
            return help_msg == other.help_msg;
        }
    };
}

namespace std {
    /**
     * \brief Specialization of \c std::hash<> for a help_text
     *
     * Specializes the \c std::hash<> functor for the help_text type. A hash
     * of the help_text is equal to its string's hash.
     *
     * \tparam Callback The type of callback the help_text's help string is associated to
     */
    template<class Callback>
    struct hash<info::_cli::help_text<Callback>> {
        /**
         * \brief The hash function for help_text
         *
         * The implementation of the hash function for help_text objects.
         *
         * \param txt The help_text object to hash
         * \return The hash of the help_text object's string member
         */
        std::size_t
        operator()(const info::_cli::help_text<Callback>& txt) const {
            return ::std::hash<std::string>{}(txt.help_msg);
        }
    };
}

namespace info::cli {
    /**
     * \brief Handles the parsing of command line arguments and calling of callbacks
     *
     * The main entity of InfoCLI. It owns the complete registry of known options
     * and handles the option parsing.
     *
     * During construction with an initializer_list of options, the set of possible
     * options are registered into the the cli_parser, which then can be used
     * to perform the parsing as needed using the operator() member function.
     *
     * All options must be provided during initialization, and there is no way
     * to modify it after.
     *
     * During the parsing, the parser walks through the given \c argv array
     * and tries matching the found options to the known ones. An exception
     * is thrown if it cannot do so. Then calls the callbacks for the proper
     * options found.
     *
     * Operands are collected and returned after the option handling finishes,
     * or if the operand \c \-\- is encountered.
     */
    struct INFO_CLI_API cli_parser {
        /**
         * \brief Calls the parsing logic for the defined options
         *
         * As defined by the options in the constructor, calling this function
         * crawls through the command line arguments of the program (as given
         * by \c argc and \c argv) and calls the appropriate callback for each
         * option encountered.
         *
         * \warning The returned vector of string_views is only valid, until
         * the provided \c argv array is alive, because the string_views, are,
         * natch, just views and do not copy the contents from that array, thus
         * if \c argv is not the program's command line arguments, make sure
         * to handle its lifetime appropriately.
         *
         * \param argc The number of strings in \c argv
         * \param argv An array of C-strings as given to the program through main
         *
         * \return A vector of string_views containing the operands in order of encounter
         */
        std::vector<std::string_view> operator()(int argc, char** argv);
        /**
         * \brief Returns the number of options
         *
         * Returns the number of options and aliases registered into the cli_parser
         *
         * \return The number of options and aliases
         */
        [[nodiscard]] INFO_CLI_PURE std::size_t size() const noexcept;

        /**
         * \brief Create the cli_parser with the given options
         *
         * Registers the set of options which will be understood by this cli_parser
         * during its lifetime. These options are constant during the lifetime
         * of the parser, and options not provided here and encountered during
         * parsing will throw an exception.
         *
         * \param opts The set of options to handle during parsing
         */
        cli_parser(std::initializer_list<option> opts);

    private:
        /**
         * \brief POD containing the required information to perform a callback
         *         when an option is found and requires so.
         */
        struct INFO_CLI_LOCAL option_info {
            rt_type_data type_data;
            std::size_t callback;
        };

        using callback_type = std::function<bool(std::string_view, const char*&)>;
        using options_type = std::unordered_map<std::string, option_info>;
        using help_innards = std::pair<std::string_view, const option_info*>;
        using help_type = _cli::help_text<help_innards>;

        std::vector<callback_type> _callbacks;
        options_type _options;
        std::unordered_set<help_type> _helps;

        /// The function to handle encountering a short option (packed or not)
        void short_option(char* arg, int argc, char** argv, int& i);
        /// The function handling singular, not packed short options
        void unpacked_shorts(char* arg, int argc, char** argv, int& i);
        /// The function handling packed options
        void packed_shorts(char* arg, int argc, char** argv, int& i);
        /// Handles long options, GNU-style or not
        void long_option(int argc, char** argv, int& i);

        /**
         * Strips the beginning dash (or two dashes) from an option argument.
         * If an option string does not begin with dashes nothing happens.
         *
         * \param opt The C-string pointing at the option with possibly leading dashes.
         * \param lng Whether to consider the second character as well for removal.
         *
         * \return A C-string pointing at the non-dash character of the original C-string
         */
        [[nodiscard]] INFO_CLI_PURE static char* strip_option(char* opt, bool lng = false);
    };

}
