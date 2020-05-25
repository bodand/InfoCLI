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

#define INFO_CLI_HELP_END_EXECUTION false

// test'd
#include <info/cli.hpp>
using namespace info::cli;

TEST_CASE("cli_parser matches long options correctly", "[cli_parser][api][!throws]") {
    SECTION("cli_parser matches GNU long options") {
        int called = 0;
        int not_called = 0;
        auto cli = cli_parser::make(
               "int|i"_opt->*called,
               "oi"_opt->*not_called
        )();
        auto args = std::array{"a.out", "--int=42"};

        (void) cli(args.size(), const_cast<char**>(args.data()));

        CHECK(called == 42);
        CHECK(not_called == 0);
    }

    SECTION("cli_parser matches separated long options") {
        int called = 0;
        int not_called = 0;
        auto cli = cli_parser::make(
               "int|i"_opt->*called,
               "oi"_opt->*not_called
        )();
        auto args = std::array{"a.out", "--int", "42"};

        (void) cli(args.size(), const_cast<char**>(args.data()));

        CHECK(called == 42);
        CHECK(not_called == 0);
    }
}

TEST_CASE("cli_parser throws expected errors", "[cli_parser][api][!throws]") {
    SECTION("non-empty accepting option throws with explicit empty input") {
        int opt = 0;
        auto cli = cli_parser::make(
               "int"_opt->*opt
        )();
        auto args = std::array{"a.out", "--int=", "42"};

        CHECK_THROWS_WITH(
               (void) cli(args.size(), const_cast<char**>(args.data())),
               "empty value for int typed option"
        );
    }

    SECTION("non-empty accepting option throws at the end of input") {
        int opt = 0;
        auto cli = cli_parser::make(
               "int"_opt->*opt
        )();
        auto args = std::array{"a.out", "--int"};

        CHECK_THROWS_WITH(
               (void) cli(args.size(), const_cast<char**>(args.data())),
               "Expected value after encountering option: --int. But found end of input."
        );
    }

    SECTION("cli_parser throws when encountering unknown option") {
        int opt = 0;
        auto cli = cli_parser::make(
               "int"_opt->*opt
        )();
        auto args = std::array{"a.out", "--unknown"};

        CHECK_THROWS_WITH(
               (void) cli(args.size(), const_cast<char**>(args.data())),
               "Unexpected option found: --unknown."
        );
    }
}

TEST_CASE("cli_parser ignores input after --", "[cli_parser][api]") {
    bool opt = false;
    auto cli = cli_parser::make(
           "bool"_opt->*opt
    )();
    auto args = std::array{"a.out", "--", "--bool"};

    auto ret = cli(args.size(), const_cast<char**>(args.data()));

    CHECK_FALSE(opt);
    CHECK(ret.size() == 2);
    CHECK_THAT(ret, Catch::VectorContains(std::string_view{"--bool"}));
}

TEST_CASE("cli_parser matches short options correctly", "[cli_parser][api]") {
    SECTION("single short options") {
        bool called = false;
        bool not_called = false;
        auto cli = cli_parser::make(
               "int|i"_opt->*called,
               "o"_opt->*not_called
        )();
        auto args = std::array{"a.out", "-i"};

        (void) cli(args.size(), const_cast<char**>(args.data()));

        CHECK(called);
        CHECK_FALSE(not_called);
    }

    SECTION("packed short options") {
        bool called = false;
        bool called2 = false;
        auto cli = cli_parser::make(
               "i"_opt->*called,
               "o"_opt->*called2
        )();
        auto args = std::array{"a.out", "-io"};

        (void) cli(args.size(), const_cast<char**>(args.data()));

        CHECK(called);
        CHECK(called2);
    }

    SECTION("shortopts with unknown length value") {
        std::string warn;
        auto cli = cli_parser::make(
               "W"_opt->*warn
        )();
        auto args = std::array{"a.out", "-Weverything"};

        (void) cli(args.size(), const_cast<char**>(args.data()));

        CHECK(warn == "everything");
    }

    SECTION("packed shortopts with known length values") {
        char c = '\0';
        bool b = false;
        auto cli = cli_parser::make(
               "c"_opt->*c,
               "b"_opt->*b
        )();
        auto args = std::array{"a.out", "-cqb"};

        (void) cli(args.size(), const_cast<char**>(args.data()));

        CHECK(c == 'q');
        CHECK(b);
    }

    SECTION("packed shortopts with numeric value") {
        int i = 0;
        bool b = false;
        auto cli = cli_parser::make(
               "i"_opt->*i,
               "b"_opt->*b
        )();
        auto args = std::array{"a.out", "-i42b"};

        (void) cli(args.size(), const_cast<char**>(args.data()));

        CHECK(i == 42);
        CHECK(b);
    }

    SECTION("complex packade shortops") {
        std::string warn;
        int i = 0;
        char c = 0;
        auto cli = cli_parser::make(
               "i"_opt->*i,
               "c"_opt->*c,
               "W"_opt->*warn
        )();
        auto args = std::array{"a.out", "-cii42Werror-prone-option-usage"};

        (void) cli(args.size(), const_cast<char**>(args.data()));

        CHECK(warn == "error-prone-option-usage");
        CHECK(i == 42);
        CHECK(c == 'i');
    }
}

TEST_CASE("cli_parser handles bool options correclt", "[cli_parser][api]") {
    bool opt = false;
    auto cli = cli_parser::make(
           "bool"_opt->*opt
    )();
    auto args = std::array{"a.out", "--bool", "keep"};

    auto ret = cli(args.size(), const_cast<char**>(args.data()));

    CHECK(opt);
    CHECK(ret.size() == 2);
    CHECK_THAT(ret, Catch::VectorContains(std::string_view{"keep"}));
}

TEST_CASE("auto-help is generated properly", "[cli_parser][auto-help][api]") {
    bool b = false;
    int i = 42;
    char undocumented = 'C';
    auto cli = cli_parser::make(
           "b|bool"_opt[
                  "Sets the boolean option [false]"_hlp
           ]->*b,
           "i|int"_opt[
                  "Sets the integer option [42]"_hlp
           ]->*i,
           "undoc"_opt->*undocumented
    )();
    auto args = std::array{"a.out", "--help", "--bool"};

    std::vector<std::string_view> ret;
    CHECK_NOTHROW(ret = cli(args.size(), const_cast<char**>(args.data())));
}

