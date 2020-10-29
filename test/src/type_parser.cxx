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

#include <catch2/catch.hpp>

#include <cstring>

// test'd
#include <info/cli/types/type_parser.hxx>
using namespace info::cli;

// custom type used to test custom parsers
struct extended_parser_type {
    int i;
};

template<>
struct info::cli::type_parser<extended_parser_type> {
    info::expected<extended_parser_type, info::cli::parser_opcode>
    operator()(std::string_view str) const noexcept {
        if (str == "fail")
            return INFO_UNEXPECTED{parser_opcode::terminate};
        return extended_parser_type{74};
    }
};

struct strm_op_type {
    int i;
};

std::istream&
operator>>(std::istream& is, strm_op_type& b) {
    int i;
    is >> i;
    b.i = i;
    return is;
}

TEMPLATE_TEST_CASE("type_parser test cases for built-in specialized types - integer",
                   "[type_parser][api][int]",
                   short,
                   int,
                   long,
                   long long,
                   unsigned short,
                   unsigned,
                   unsigned long,
                   unsigned long long) {
    type_parser<TestType> parser;
    const char* last = nullptr;

    SECTION("type_parser fails for non-digit input") {
        const char str[] = "asd";
        CHECK_FALSE(parser(str, last));
        CHECK(last == str);
    }

    SECTION("type_parser parses pure digit inputs successfully") {
        const char str[] = "125";
        auto ret = parser(str, last);

        REQUIRE(ret);
        CHECK(*ret == 125);
        CHECK(last == str + std::strlen(str));
    }

    SECTION("type_parser parses non-pure input successfully") {
        const char str[] = "45m";
        auto ret = parser(str, last);

        REQUIRE(ret);
        CHECK(*ret == 45);
        CHECK(last == str + std::strlen(str) - 1);
    }
}

TEMPLATE_TEST_CASE("type_parser test cases for built-in specialized types - floating point",
                   "[type_parser][api][float]",
                   float,
                   double,
                   long double) {
    type_parser<TestType> parser;
    const char* last = nullptr;

    SECTION("type_parser fails for non-digit input") {
        const char str[] = "asd";
        CHECK_FALSE(parser(str, last));
        CHECK(last == str);
    }

    SECTION("type_parser parses pure digit inputs successfully") {
        const char str[] = "125.54";
        auto ret = parser(str, last);

        REQUIRE(ret);
        CHECK_THAT(*ret,
                   Catch::WithinRel(
                          static_cast<std::conditional_t<std::is_same_v<TestType, long double>,
                                                         double,
                                                         TestType>>(125.54)));
        CHECK(last == str + std::strlen(str));
    }

    SECTION("type_parser parses non-pure input successfully") {
        const char str[] = "489.521m";
        auto ret = parser(str, last);

        REQUIRE(ret);
        CHECK_THAT(*ret,
                   Catch::WithinRel(
                          static_cast<std::conditional_t<std::is_same_v<TestType, long double>,
                                                         double,
                                                         TestType>>(489.521)));
        CHECK(last == str + std::strlen(str) - 1);
    }
}

TEST_CASE("type_parser test cases for custom types", "[type_parser][api][user-defined]") {
    SECTION("type_parser with operator>>") {
        type_parser<strm_op_type> parser;
        const char* last;

        const char str[] = "42";
        auto ret = parser(str, last);
        REQUIRE(ret);
        CHECK(ret->i == 42);
        CHECK(last == str + std::strlen(str));
    }

    SECTION("type_parser with specialization") {
        type_parser<extended_parser_type> parser;

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
