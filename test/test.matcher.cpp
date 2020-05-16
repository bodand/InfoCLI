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

// test'd
#include <info/cli/matcher.hpp>
using namespace info::cli::impl;

TEST_CASE("matcher test cases", "[matcher][impl]") {
    SECTION("matchers compare equal with same string-type") {
        matcher a{BOOST_HANA_STRING("asd")};
        matcher b{BOOST_HANA_STRING("asd")};

        CHECK(a == b);
        CHECK_FALSE(a != b);
    }

    SECTION("matchers compare unequal with different string-type") {
        matcher a{BOOST_HANA_STRING("impl")};
        matcher b{BOOST_HANA_STRING("api")};

        CHECK(a != b);
        CHECK_FALSE(a == b);
    }

    SECTION("empty matcher does not match anything") {
        matcher m{boost::hana::string_c<>};

        CHECK_FALSE(m(""));
        CHECK_FALSE(m("anything"));
        CHECK_FALSE(m("-anything"));
        CHECK_FALSE(m("--anything"));
    }

    SECTION("one char option matchers match POSIX style opts") {
        matcher m{BOOST_HANA_STRING("a")};

        CHECK(m("-a"));
        CHECK_FALSE(m("--a"));
        CHECK_FALSE(m("a"));
        CHECK_FALSE(m("-b"));
        CHECK_FALSE(m("--bingo"));
    }

    SECTION("multi char option matchers match GNU style opts") {
        matcher m{BOOST_HANA_STRING("int")};

        CHECK(m("--int"));
        CHECK_FALSE(m("-int"));
        CHECK_FALSE(m("int"));
        CHECK_FALSE(m("-a"));
        CHECK_FALSE(m("--asd"));
    }

    SECTION("multi-matchers match multiple opts of possibly different styles") {
        matcher m{BOOST_HANA_STRING("int|i")};

        CHECK(m("--int"));
        CHECK(m("-i"));
        CHECK_FALSE(m("--i"));
        CHECK_FALSE(m("-int"));
    }
}
