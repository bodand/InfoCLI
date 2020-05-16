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

// project
#include "type_equality.hpp"

// test'd
#include <info/cli/dissector.hpp>
using namespace info::cli::impl;

void fun(int) {}

TEST_CASE("is_typed_callback tests", "[is_callback][impl][meta]") {

    SECTION("is_callback returns false for non-callable types") {
        struct foo {
        };

        CHECK_FALSE(is_typed_callback<int>);
        CHECK_FALSE(is_typed_callback<foo>);
    }

    SECTION("is_callback returns true for function types") {
        CHECK(is_typed_callback<decltype(fun)>);
    }

    SECTION("is_callback returns true for functor types") {
        struct foo {
            void operator()(int) {}
        };

        CHECK(is_typed_callback<foo>);
    }

    SECTION("is_callback returns true for lambdas") {
        auto lambda = [](int) {};
        CHECK(is_typed_callback<decltype(lambda)>);
    }

    SECTION("is_callback returns false for generic lambdas") {
        auto lambda = [](auto) {};
        CHECK_FALSE(is_typed_callback<decltype(lambda)>);
    }
}

TEST_CASE("dissector tests", "[dissector][impl][meta]") {
    struct nullary {
        void operator()() {}
    };

    struct unary {
        void operator()(int) {}
    };

    struct binary {
        void operator()(int, int) {}
    };

    SECTION("dissector returns empty list correctly") {
        CHECK(type_c<dissect<nullary>> == type_c<tlist<>>);
    }

    SECTION("dissector returns one element list correctly") {
        CHECK(type_c<dissect<unary>> == type_c<tlist<int>>);
    }

    SECTION("dissector returns multi-element list correctly") {
        CHECK(type_c<dissect<binary>> == type_c<tlist<int, int>>);
    }
}
