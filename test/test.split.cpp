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

#include <boost/hana/string.hpp>
namespace hana = boost::hana;
using namespace hana::literals;

// project
#include "type_equality.hpp"

// test'd
#include <info/cli/meta_split.hpp>
#include <info/cli/split.hpp>
using namespace info::cli::impl;

TEST_CASE("split test cases", "[split][impl]") {
    SECTION("split returns correctly split string") {
        auto str = std::string{"--option=value"};
        auto ret = split(str);

        CHECK(ret[0] == "--option");
        CHECK(ret[1] == "value");
    }

    SECTION("split returns empty part if delim is at end") {
        auto str = std::string{"--option="};
        auto ret = split(str);

        CHECK(ret[0] == "--option");
        CHECK(ret[1] == "");
    }

    SECTION("split returns input string as first ret if delim is not present") {
        auto str = std::string{"--option"};
        auto ret = split(str);

        CHECK(ret[0] == "--option");
        CHECK(ret[1] == "");
    }
}

TEST_CASE("meta_split test cases", "[meta_split][impl][meta]") {
    SECTION("meta_split returns empty tuple for empty string") {
        auto str = BOOST_HANA_STRING("");
        auto ret = meta_split(str);

        CHECK(type_c<decltype(ret)> == type_c<hana::tuple<>>);
    }

    SECTION("meta_split returns one element tuple for non-meta_splittable string") {
        auto str = BOOST_HANA_STRING("text");
        auto ret = meta_split(str);

        CHECK(type_c<std::remove_reference_t<decltype(ret[0_c])>> == type_c<decltype(str)>);
    }

    SECTION("meta_split splits string with one | in it") {
        auto str = BOOST_HANA_STRING("te|xt");
        auto ret = meta_split(str);
        auto exp = hana::make_tuple(
               BOOST_HANA_STRING("te"),
               BOOST_HANA_STRING("xt")
        );

        CHECK(type_c<decltype(ret)> == type_c<decltype(exp)>);
    }

    SECTION("meta_split splits string with multiple | in it") {
        auto str = BOOST_HANA_STRING("te|xt|te|xt|t");
        auto ret = meta_split(str);
        auto exp = hana::make_tuple(
               BOOST_HANA_STRING("te"),
               BOOST_HANA_STRING("xt"),
               BOOST_HANA_STRING("te"),
               BOOST_HANA_STRING("xt"),
               BOOST_HANA_STRING("t")
        );

        CHECK(type_c<decltype(ret)> == type_c<decltype(exp)>);
    }
}
