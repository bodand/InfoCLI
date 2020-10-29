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
// Created by bodand on 2020-05-15.
//

#pragma once

#include <algorithm>
#include <charconv>
#include <cstdlib>
#include <sstream>
#include <string>
#include <string_view>

#include <info/expected.hpp>

#include <info/cli/extra/repeat.hxx>
#include <info/cli/macros.hxx>
#include <info/cli/meta/meta.hxx>

namespace info::cli {
    /**
     * \brief Opcode given to the parser about what to do if the parsing failed.
     * Because of bad input, or if the user writing the callback decided so.
     *
     * A value returned to the parser calling the parser about how to handle different
     * parsing failures.
     * All type_parser<>-s provided with the library return terminate on failure.
     */
    enum class parser_opcode {
        terminate,///< Terminate parsing and throw a command_line_exception
        ignore,   ///< Ignore the parsing error and pretend the whole option \
                        wasn't there in any form or shape
    };

#define SPECIALIZED_PARSERS \
    F(std::string)          \
    F(std::string_view)     \
    F(char)                 \
    F(unsigned char)        \
    F(short)                \
    F(unsigned short)       \
    F(int)                  \
    F(unsigned)             \
    F(long)                 \
    F(unsigned long)        \
    F(long long)            \
    F(unsigned long long)   \
    F(float)                \
    F(double)               \
    F(double long)          \
    F(bool)

    /**
     * \brief The type used to perform the parsing for different types when
     *        the value is not of a string-like type.
     *
     * The type_parser is responsible for parsing the string found on the command
     * line into the desired type.
     * Many types, like most fundamental types have a dedicated parser, but
     * a general parser that works with streams is also provided, for that
     * see the unspecialized type_parser type.
     *
     * \tparam T The type to parse the string into
     */
    template<class T>
    struct INFO_CLI_API type_parser {
        /**
         * \brief Performs the parsing from string to a given type
         *
         * Takes the input string and parses as much as possible into a given
         * type. (This documentation is not written manually for each specialization
         * so just look at the type in the specialization to see which type does
         * "a given type" refer to.) If such a conversion is impossible the
         * parser bails by returning a parser_opcode telling the main cli_parser
         * object how to handle this failure. As described in the parser_opcode
         * documentation, all built-in InfoCLI type_parser-s return
         * \c parser_opcode::terminate which will end the command line's parsing.
         *
         * The unspecialized type_parser does not contain any special logic that
         * is optimized for any given type, it just uses the standard C++ streams
         * and thus works with types overloaded with the operator>> function.
         *
         * As of InfoCLI v2.1.0 - Lithium, the following types have a
         * specialized type_parser, with some differing in behavior from a normal
         * stream based implementation:
         *
         *  - \c std::string and \c std::string_view :
         *      Differ in that they take the whole input not just one word,
         *      contrary to the general stream implementation.
         *  - \c float
         *  - \c double
         *  - \c long double
         *  - \c bool : The following values count as false: the empty string, "off", "false", "0",
         *              everything else if considered truthy.
         *  - \c char and `unsigned char`
         *  - \c short and `unsigned short`
         *  - \c int and `unsigned int`
         *  - \c long and `unsigned long`
         *  - `long long` and `unsigned long long`
         *
         *
         * \param str The string to parse into a given type
         * \param last A pointer pointing at the first unconvertible character
         *              of the input string. This may be an after the end pointer.
         *
         * \return An expected type containing the successfully converted type,
         *          or an parser_opcode telling the parser how to handle this
         *          failure.
         */
        info::expected<T, parser_opcode>
        operator()(std::string_view str, const char*& last) const noexcept {
            T tmp;
            last = str.data() + str.size();

            std::istringstream ss{str.data()};
            if (ss >> tmp) {
                return tmp;
            }
            return INFO_UNEXPECTED{parser_opcode::terminate};
        }
    };

    /** \copybrief type_parser
     * \copydetails type_parser
     *
     * This type_parser is specialized for a type_modifier: \c cli::repeat. This
     * type modifier does not modify the parsing behavior of the given type,
     * at least not on this level, thus it can just simply inherit from the
     * type_parser implementing parsing for the contained type.
     */
    template<class T>
    struct INFO_CLI_API type_parser<cli::repeat<T>> : type_parser<T> { };

    /**
     * Implementation detail. Ignore.
     * Does not creep beyond the boundaries of this header, so its quite tame.
     */
#define F(T)                                                                \
    template<>                                                              \
    struct INFO_CLI_API type_parser<T> {                                    \
        info::expected<T, parser_opcode>                                    \
        operator()(std::string_view str, const char*& last) const noexcept; \
    };

    SPECIALIZED_PARSERS
#undef F
}

#undef SPECIALIZED_PARSERS
