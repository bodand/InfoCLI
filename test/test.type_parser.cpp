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

#include <catch2/catch.hpp>

// special
#include <info/cli/specialize.hpp>

struct foo {
    int i;
};

template<>
struct info::cli::type_parser<foo> {
    info::expected<foo, std::string_view>
    operator()(std::string_view str) const noexcept {
        if (str == "fail")
            return INFO_UNEXPECTED{std::string_view{"failure requested"}};
        return foo{74};
    }
};

// test'd
#include <info/cli/type_parser.hpp>
using namespace info::cli;

struct bar {
    int i;
};

std::istream& operator>>(std::istream& is, bar& b) {
    int i;
    is >> i;
    b.i = i;
    return is;
}

TEMPLATE_TEST_CASE("type_parser test cases for built-in specialized types - integer",
                   "[type_parser][api][int]",
                   short, int, long, unsigned short, unsigned, unsigned long) {
    type_parser<TestType> parser;

    SECTION("type_parser fails for empty input") {
        CHECK_FALSE(parser(""));
    }

    SECTION("type_parser fails for non-digit input") {
        CHECK_FALSE(parser("asd"));
    }

    SECTION("type_parser parses pure digit inputs successfully") {
        auto ret = parser("125");

        REQUIRE(ret);
        CHECK(*ret == 125);
    }

    SECTION("type_parser parses non-pure input successfully") {
        auto ret = parser("45m");

        REQUIRE(ret);
        CHECK(*ret == 45);
    }
}

TEMPLATE_TEST_CASE("type_parser test cases for built-in specialized types - floating point",
                   "[type_parser][api][float]",
                   float, double, long double) {
    type_parser<TestType> parser;

    SECTION("type_parser fails for empty input") {
        CHECK_FALSE(parser(""));
    }

    SECTION("type_parser fails for non-digit input") {
        CHECK_FALSE(parser("asd"));
    }

    SECTION("type_parser parses pure digit inputs successfully") {
        auto ret = parser("125.54");

        REQUIRE(ret);
        CHECK_THAT(*ret, Catch::WithinRel(
               static_cast<std::conditional_t<std::is_same_v<TestType, long double>, double, TestType>>(125.54)));
    }

    SECTION("type_parser parses non-pure input successfully") {
        auto ret = parser("489.521m");

        REQUIRE(ret);
        CHECK_THAT(*ret, Catch::WithinRel(
               static_cast<std::conditional_t<std::is_same_v<TestType, long double>, double, TestType>>(489.521)));
    }
}

TEST_CASE("type_parser test cases for custom types", "[type_parser][api][user-defined]") {
    SECTION("type_parser with operator>>") {
        type_parser<bar> parser;
        auto ret = parser("42");
        REQUIRE(ret);
        CHECK(ret->i == 42);
    }

    SECTION("type_parser with specialization") {
        type_parser<foo> parser;

        SECTION("fails when expected to") {
            CHECK_FALSE(parser("fail"));
        }

        SECTION("succeeds when expected to") {
            auto ret = parser("whatever");
            REQUIRE(ret);
            CHECK(ret->i == 74);
        }
    }
}
