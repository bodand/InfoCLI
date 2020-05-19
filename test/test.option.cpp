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

#include <catch2/catch.hpp>

// stdlib
#include <iostream>

// Boost.Hana
#define BOOST_HANA_CONFIG_ENABLE_STRING_UDL
#include <boost/hana.hpp>
using namespace boost;
using namespace hana::literals;

// project
#include "type_equality.hpp"

// test'd
#include <info/cli/option.hpp>
using namespace info::cli;

struct gen_func {
    template<class T>
    void operator()(T) {}

    bool operator==(const gen_func& rhs) const {
        return i == rhs.i;
    }

    int i;
};

TEST_CASE("option test cases", "[option][api]") {
    SECTION("_opt udl works") {
        option_str<char, 'a', 'b'> opt{};
        auto opt2 = "ab"_opt;

        CHECK(type_c<decltype(opt)> == type_c<decltype(opt2)>);
    }

    SECTION("_opt udl creates map with expected keys") {
        int i = 42;
        auto matcher = "int|i"_opt->*i;

        CHECK(type_c<decltype(hana::length(matcher))> == type_c<hana::size_t<2>>);
        CHECK(type_c<decltype(matcher[0_c][0_c])> == type_c<decltype("--int"_s)&>);
        CHECK(type_c<decltype(matcher[1_c][0_c])> == type_c<decltype("-i"_s)&>);
    }

    SECTION("option creates expected map with reference") {
        int i = 5;
        auto matcher = "int"_opt->*i;

        CHECK(type_c<decltype(hana::length(matcher))> == type_c<hana::size_t<1>>);
        CHECK(type_c<decltype(matcher[0_c][0_c])> == type_c<decltype("--int"_s)&>);
    }

    SECTION("option creates expected map with typed functor") {
        struct func {
            void operator()(int) {}

            bool operator==(const func& rhs) const {
                return i == rhs.i;
            }

            int i;
        };

        auto fun = func{42};
        auto matcher = "int"_opt->*fun;
        CHECK(type_c<decltype(hana::length(matcher))> == type_c<hana::size_t<1>>);
        CHECK(type_c<decltype(matcher[0_c][0_c])> == type_c<decltype("--int"_s)&>);
        // CHECK(type_c<decltype(matcher[0_c][1_c])> == type_c<hana::basic_type<int>&>);
        CHECK(type_c<decltype(matcher[0_c][2_c])> == type_c<func&>);
    }

    SECTION("options creates expected map with generic-lambda-style functor") {
        auto fun = gen_func{42};

        auto matcher = "int"_opt->*fun;
        CHECK(type_c<decltype(hana::length(matcher))> == type_c<hana::size_t<1>>);
        // CHECK(type_c<decltype(matcher[0_c][1_c])> == type_c<hana::basic_type<std::string_view>&>);
        CHECK(type_c<decltype(matcher[0_c][2_c])> == type_c<gen_func&>);
    }

    SECTION("option creates their help appropriately") {
        auto matcher = "int"_opt["help string"]->*[](int) {};
        (void) matcher;

        CHECK(help<hana::tuple<hana::string<'-', '-', 'i', 'n', 't'>>>.msg == "help string");
    }
}
