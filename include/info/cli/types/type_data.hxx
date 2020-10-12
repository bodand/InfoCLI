//// BSD 3-Clause License
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
    namespace impl {
        template<class T>
        struct INFO_CLI_LOCAL is_non_char_arithmetic : std::is_arithmetic<T> {
        };

        template<>
        struct INFO_CLI_LOCAL is_non_char_arithmetic<char> : std::false_type {
        };
        template<>
        struct INFO_CLI_LOCAL is_non_char_arithmetic<signed char> : std::false_type {
        };
        template<>
        struct INFO_CLI_LOCAL is_non_char_arithmetic<unsigned char> : std::false_type {
        };
        template<>
        struct INFO_CLI_LOCAL is_non_char_arithmetic<wchar_t> : std::false_type {
        };
#ifdef __cpp_char8_t
        template<>
        struct INFO_CLI_LOCAL is_non_char_arithmetic<char8_t> : std::false_type { };
#endif
        template<>
        struct INFO_CLI_LOCAL is_non_char_arithmetic<char16_t> : std::false_type {
        };
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

    INFO_CLI_API INFO_CLI_CONST
           std::function<bool(char)>
           parse_type_accepts(parse_type type);

    template<class T>
    struct INFO_CLI_LOCAL type_data {
        constexpr const static bool allow_nothing = false;
        constexpr const static std::string_view default_value = "";
        constexpr const static int length = -1;
        constexpr const static parse_type expected_type =
               impl::is_non_char_arithmetic<T>::value ? parse_type::numeric
                                                      : parse_type::printable;
        constexpr const static std::string_view type_name = meta::type_name<T>();
    };

    template<class T>
    class INFO_CLI_LOCAL type_data<cli::repeat<T>> {
        constexpr const static auto nonchar_arithmetic = impl::is_non_char_arithmetic<T>::value;

    public:
        constexpr const static bool allow_nothing = nonchar_arithmetic
                                                    || type_data<T>::allow_nothing;
        constexpr const static std::string_view default_value = nonchar_arithmetic ? "1"
                                                                                   : type_data<T>::default_value;
        constexpr const static int length = type_data<T>::length;
        constexpr const static parse_type expected_type = type_data<T>::expected_type;
        constexpr const static std::string_view type_name = type_data<T>::type_name;
    };

    template<>
    struct INFO_CLI_LOCAL type_data<bool> {
        constexpr const static bool allow_nothing = true;
        constexpr const static std::string_view default_value = "1";
        constexpr const static int length = -1;
        constexpr const static parse_type expected_type = parse_type::alphanumeric;
        constexpr const static std::string_view type_name = "bool";
    };

    template<>
    struct INFO_CLI_LOCAL type_data<char> {
        constexpr const static bool allow_nothing = false;
        constexpr const static std::string_view default_value = "";
        constexpr const static int length = 1;
        constexpr const static parse_type expected_type = parse_type::printable;
        constexpr const static std::string_view type_name = "char";
    };

    template<>
    struct INFO_CLI_LOCAL type_data<unsigned char> {
        constexpr const static bool allow_nothing = false;
        constexpr const static std::string_view default_value = "";
        constexpr const static int length = 1;
        constexpr const static parse_type expected_type = parse_type::any;
        constexpr const static std::string_view type_name = "unsigned char";
    };

    struct INFO_CLI_API rt_type_data {
        bool allow_nothing;
        std::string_view default_val;
        int length;
        parse_type expected_type;
        std::string_view type_name;

        [[nodiscard]] INFO_CLI_PURE bool finite() const noexcept;

        template<class T>
        explicit rt_type_data(type_data<T>)
             : allow_nothing{type_data<T>::allow_nothing},
               default_val{type_data<T>::default_value},
               length{type_data<T>::length},
               expected_type{type_data<T>::expected_type},
               type_name{type_data<T>::type_name} { }
    };

}
