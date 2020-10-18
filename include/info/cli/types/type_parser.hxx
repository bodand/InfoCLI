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

    template<class T>
    struct INFO_CLI_API type_parser {
        // default, low-performance parser
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

    template<class T>
    struct INFO_CLI_API type_parser<cli::repeat<T>> : type_parser<T> { };

#define PARSER_DECL(T)                                                      \
    template<>                                                              \
    struct INFO_CLI_API type_parser<T> {                                    \
        info::expected<T, parser_opcode>                                    \
        operator()(std::string_view str, const char*& last) const noexcept; \
    };

    PARSER_DECL(std::string)
    PARSER_DECL(std::string_view)
    PARSER_DECL(char)
    PARSER_DECL(unsigned char)
    PARSER_DECL(short)
    PARSER_DECL(unsigned short)
    PARSER_DECL(int)
    PARSER_DECL(unsigned)
    PARSER_DECL(long)
    PARSER_DECL(unsigned long)
    PARSER_DECL(long long)
    PARSER_DECL(unsigned long long)
    PARSER_DECL(float)
    PARSER_DECL(double)
    PARSER_DECL(double long)
    PARSER_DECL(bool)
}
#undef PARSER_DECL
