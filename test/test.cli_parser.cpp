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
// Created by bodand on 2020-05-16.
//

#include <catch2/catch.hpp>

#include "type_equality.hpp"

// test'd
#include <info/cli.hpp>
using namespace info::cli;

TEST_CASE("cli_parser test cases", "[cli_parser][api]") {
    SECTION("cli_parser matches GNU long options") {
        int called = 0;
        int not_called = 0;
        cli_parser cli{
               "int|i"_opt->*called,
               "oi"_opt->*not_called
        };
        auto args = std::array{"a.out", "--int=42"};

        (void) cli(args.size(), const_cast<char**>(args.data()));

        CHECK(called == 42);
        CHECK(not_called == 0);
    }

    SECTION("cli_parser matches separated long options") {
        int called = 0;
        int not_called = 0;
        cli_parser cli{
               "int|i"_opt->*called,
               "oi"_opt->*not_called
        };
        auto args = std::array{"a.out", "--int", "42"};

        (void) cli(args.size(), const_cast<char**>(args.data()));

        CHECK(called == 42);
        CHECK(not_called == 0);
    }

    SECTION("bool option does not take up following arg") {
        bool opt = false;
        cli_parser cli{
               "bool"_opt->*opt
        };
        auto args = std::array{"a.out", "--bool", "keep"};

        auto ret = cli(args.size(), const_cast<char**>(args.data()));

        CHECK(opt);
        CHECK(ret.size() == 2);
        CHECK_THAT(ret, Catch::VectorContains(std::string_view{"keep"}));
    }

    SECTION("non-empty accepting option throws with explicit empty input") {
        int opt = 0;
        cli_parser cli{
               "int"_opt->*opt
        };
        auto args = std::array{"a.out", "--int=", "42"};

        CHECK_THROWS_WITH(
               (void) cli(args.size(), const_cast<char**>(args.data())),
               "empty value for int typed option"
        );
    }

    SECTION("non-empty accepting option throws at the end of input") {
        int opt = 0;
        cli_parser cli{
               "int"_opt->*opt
        };
        auto args = std::array{"a.out", "--int"};

        CHECK_THROWS_WITH(
               (void) cli(args.size(), const_cast<char**>(args.data())),
               "Expected value after encountering option: --int. But found end of input. Use --help for usage."
        );
    }

    SECTION("cli_parser throws when encountering unknown option") {
        int opt = 0;
        cli_parser cli{
               "int"_opt->*opt
        };
        auto args = std::array{"a.out", "--unknown"};

        CHECK_THROWS_WITH(
               (void) cli(args.size(), const_cast<char**>(args.data())),
               "Unexpected option found: --unknown. Use --help for usage."
        );
    }

    SECTION("cli_parser ignores input after --") {
        bool opt = false;
        cli_parser cli{
               "bool"_opt->*opt
        };
        auto args = std::array{"a.out", "--", "--bool"};

        auto ret = cli(args.size(), const_cast<char**>(args.data()));

        CHECK_FALSE(opt);
        CHECK(ret.size() == 2);
        CHECK_THAT(ret, Catch::VectorContains(std::string_view{"--bool"}));
    }
}
