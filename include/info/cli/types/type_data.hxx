// BSD 3-Clause License
//
// Copyright (c) 2020, bodand
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//
// Created by bodand on 2020-05-18.
//

#pragma once

#include <functional>
#include <string_view>
#include <type_traits>

#include <info/cli/macros.hxx>
#include <info/cli/meta/meta.hxx>
#include <info/cli/meta/type_name.hxx>

namespace info::cli {
    /**
     * \brief Old implementation namespace from InfoCLI v1.x
     *
     * The implementation detail namespace from InfoCLI v1.x. Still counts
     * as an implementation detail, but will not be expanded, and everything
     * is slowly to be replaced in it, with something from info::_cli.
     */
    namespace impl {
        /**
         * \brief Type-trait for arithmetic types that are not a char type.
         *
         * Checks true for all types that check true for std::is_arithmetic,
         * except for character types. The handled character types are the following:
         * \c char, `signed char`, `unsigned char`, \c wchar_t, \c char8_t (if it exists),
         * \c char16_t, and \c char32_t.
         *
         * \tparam T The type to check
         */
        template<class T>
        struct INFO_CLI_LOCAL is_non_char_arithmetic : std::is_arithmetic<T> {
        };

        /// \copydoc is_non_char_arithmetic
        template<>
        struct INFO_CLI_LOCAL is_non_char_arithmetic<char> : std::false_type {
        };
        /// \copydoc is_non_char_arithmetic
        template<>
        struct INFO_CLI_LOCAL is_non_char_arithmetic<signed char> : std::false_type {
        };
        /// \copydoc is_non_char_arithmetic
        template<>
        struct INFO_CLI_LOCAL is_non_char_arithmetic<unsigned char> : std::false_type {
        };
        /// \copydoc is_non_char_arithmetic
        template<>
        struct INFO_CLI_LOCAL is_non_char_arithmetic<wchar_t> : std::false_type {
        };
#ifdef __cpp_char8_t
        /// \copydoc is_non_char_arithmetic
        template<>
        struct INFO_CLI_LOCAL is_non_char_arithmetic<char8_t> : std::false_type { };
#endif
        /// \copydoc is_non_char_arithmetic
        template<>
        struct INFO_CLI_LOCAL is_non_char_arithmetic<char16_t> : std::false_type {
        };
        /// \copydoc is_non_char_arithmetic
        template<>
        struct INFO_CLI_LOCAL is_non_char_arithmetic<char32_t> : std::false_type {
        };
    }

    /**
     * \brief What types of characters are accepted for the C++ type that are marked by them
     *
     * This enumeration defines the types of characters accepted by the type of
     * the option they are parsing. This means that actual C++ types that are
     * marked with parse_type::alpha, for example, will only accept non-digit
     * characters. Same Spiel for parse_type::numeric with digits.
     *
     * This knowledge is used by the parser when using packed options such as the
     * following, for the options described below it.
     * \verbatim
     * exec -O1df9
     *
     * For options:
     *   -O <int>
     *   -d <str of non-digits> (there are no built-in types for this, but user-defined is a thing)
     *   -9 (simple bool flag)
     * \endverbatim
     *
     * If it did not have any information about anything it'd try to match
     * `-O` with the value `1df9`, which may or may not be intended. However, InfoCLI
     * is smart and does know the C++ types expected for all options, thus it is able
     * to realize that `1df` for an integer does not make sense.(..that much, only a little):
     * In fact, we know that `-O` takes parse_type::numeric input (because it's `int`) and
     * that `-d` takes parse_type::alpha (the user-defined type that only accepts non-digits).
     * `-9` is boolean, but that doesn't matter much because there is nothing after it.
     * With this we know that the input to `-O` will end up as `1`, because that's the
     * longest input value that's accepted by the numeric parser, and also for `-d` will
     * be `f` because `9` cannot be given to an parser for alpha. That leaves `-9` which
     * chills it the back safely.
     */
    enum class parse_type {
        alpha,       ///< Only accepts non-digit characters (as given by `std::isalpha`)
        alphanumeric,///< Accepts all alphanumeric characters (as given by `std::isalnum`)
        numeric,     ///< Only accepts numeric characters (as given by `std::isdigit`)
        printable,   ///< Accepts all printable characters (as given by `std::isprint`)
        any,         ///< Accepts all characters without any validation
    };

    /**
     * \brief Function to check if parse_type accepts the given character
     *
     * Returns a validator functor that returns a boolean representing whether
     * the character given to it is accepted by the parse_type given to this
     * function.
     *
     * \param type The parse_type to check
     *
     * \return A functor taking a char and returning a boolean representing
     *          whether the parse_type accepts that char
     */
    INFO_CLI_API INFO_CLI_CONST
           std::function<bool(char)>
           parse_type_accepts(parse_type type);
    // ^ clang-format died for this ^

    /**
     * \brief Represents a set of information for a type to be used by InfoRTTI.
     *
     * Bundles information about a type for InfoRTTI. This is used to construct
     * the rt_type_data object which will then provide the parser with the required
     * information about a type at runtime.
     *
     * \attention Currently the name member is in beta and may change.
     *
     * \tparam T The type to get the information from.
     */
    template<class T>
    struct INFO_CLI_LOCAL type_data {
        constexpr const static bool allow_nothing = false;                       ///< Whether the type can be provided with no value and it can handle it
        constexpr const static std::string_view default_value = "";              ///< The default value to use if a value was not provided
        constexpr const static int length = -1;                                  ///< How long is the expected value. If varying, or unknown like a string, use \c -1
        constexpr const static std::string_view type_name = meta::type_name<T>();///< The name of the type in a human readable manner. !Beta!
        constexpr const static parse_type expected_type =                        ///< The parse_type representing what kind of characters are accepted in the value
               impl::is_non_char_arithmetic<T>::value ? parse_type::numeric
                                                      : parse_type::printable;
    };

    /**
     * \copydoc type_data
     *
     * \note
     * For documentation of the data members see the unspecialized
     * type_data class template
     */
    template<class T>
    class INFO_CLI_LOCAL type_data<cli::repeat<T>> {
        constexpr const static auto nonchar_arithmetic = impl::is_non_char_arithmetic<T>::value;

    public:
        constexpr const static bool allow_nothing =
               nonchar_arithmetic || type_data<T>::allow_nothing;
        constexpr const static std::string_view default_value =
               nonchar_arithmetic ? "1"
                                  : type_data<T>::default_value;
        constexpr const static int length = type_data<T>::length;
        constexpr const static parse_type expected_type = type_data<T>::expected_type;
        constexpr const static std::string_view type_name = type_data<T>::type_name;
    };

    /**
     * \copydoc type_data
     *
     * \note
     * For documentation of the data members see the unspecialized
     * type_data class template
     *
     * This is the only built-in, non-modified type that allows a valueless
     * option call.
     */
    template<>
    struct INFO_CLI_LOCAL type_data<bool> {
        constexpr const static bool allow_nothing = true;
        constexpr const static std::string_view default_value = "1";
        constexpr const static int length = -1;
        constexpr const static parse_type expected_type = parse_type::alphanumeric;
        constexpr const static std::string_view type_name = "bool";
    };

    /**
     * \copydoc type_data
     *
     * \note
     * For documentation of the data members see the unspecialized
     * type_data class template
     */
    template<>
    struct INFO_CLI_LOCAL type_data<char> {
        constexpr const static bool allow_nothing = false;
        constexpr const static std::string_view default_value = "";
        constexpr const static int length = 1;
        constexpr const static parse_type expected_type = parse_type::printable;
        constexpr const static std::string_view type_name = "char";
    };

    /**
     * \copydoc type_data
     *
     * \note
     * For documentation of the data members see the unspecialized
     * type_data class template
     */
    template<>
    struct INFO_CLI_LOCAL type_data<unsigned char> {
        constexpr const static bool allow_nothing = false;
        constexpr const static std::string_view default_value = "";
        constexpr const static int length = 1;
        constexpr const static parse_type expected_type = parse_type::any;
        constexpr const static std::string_view type_name = "unsigned char";
    };

    /**
     * \brief Runtime incarnation of type information for InfoRTTI
     *
     * This is the runtime class that basically performs the type erasure and
     * deteriorates the mighty and proud types into some attributes of their
     * formal selves.
     *
     * During the option class' instantiations, the type from the callback is
     * used to construct a type_data object, which is immediately passed to the
     * constructor to create a runtime representation that ignores types. The
     * stored attributes are then used by the parsing algorithm to always behave
     * appropriately for types, like do not want more than one character for a
     * \c char, and do not accept non-digit characters for an integer value, for
     * example.
     */
    struct INFO_CLI_API rt_type_data {
        bool allow_nothing;          ///< If no value is found directly given to the option, do not search for it, and allow a valueless option call
        std::string_view default_val;///< When a valueless option call occurs, this is the value to be used
        int length;                  ///< The maximal length of the value when parsed. If unknown \c -1 is used.
        parse_type expected_type;    ///< The parse_type representing what type of characters are accepted for the parser
        std::string_view type_name;  ///< The beta attribute containing the human readable name of the value

        /**
         * \brief Returns whether the value for the type has a known finite length, like \c char does
         * \return If the current value has a known finite length
         */
        [[nodiscard]] INFO_CLI_PURE bool finite() const noexcept;

        /**
         * \brief Constructor for the runtime type information object
         *
         * Constructor of the runtime InfoRTTI type. Takes an InfoRTTI type_data
         * object and constructs a runtime version by erasing the type and taking
         * the value stored.
         *
         * The passed object is not used, only its type is relevant in the
         * construction of the runtime object.
         *
         * \tparam T The templated type of the type_data object.
         */
        template<class T>
        explicit rt_type_data(type_data<T>)
             : allow_nothing{type_data<T>::allow_nothing},
               default_val{type_data<T>::default_value},
               length{type_data<T>::length},
               expected_type{type_data<T>::expected_type},
               type_name{type_data<T>::type_name} { }
    };

}
