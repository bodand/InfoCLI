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

// Boost.Hana
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

    SECTION("option creates expected tuple with reference") {
        int i = 5;
        auto matcher = "int"_opt->*i;
        auto tuple = hana::make_tuple(
               hana::string_c<'i', 'n', 't'>,
               impl::val_callback<int>{i},
               hana::type_c<int>
        );

        CHECK(std::string_view{matcher[0_c].c_str()} == tuple[0_c].c_str());
        CHECK(matcher[1_c] == tuple[1_c]);
        // ^ two val_callbacks compare equal if they share the same ref
        CHECK(type_c<decltype(matcher[2_c])> == type_c<decltype(tuple[2_c])>);
    }

    SECTION("option creates expected tuple with typed functor") {
        struct func {
            void operator()(int) {}

            bool operator==(const func& rhs) const {
                return i == rhs.i;
            }

            int i;
        };

        auto fun = func{42};
        auto matcher = "int"_opt->*fun;
        auto tuple = hana::make_tuple(
               hana::string_c<'i', 'n', 't'>,
               func{42},
               hana::type_c<int>
        );

        CHECK(std::string_view{matcher[0_c].c_str()} == tuple[0_c].c_str());
        CHECK(matcher[1_c] == tuple[1_c]);
        CHECK(type_c<decltype(matcher[2_c])> == type_c<decltype(tuple[2_c])>);
    }

    SECTION("options creates expected tuple with generic-lambda-style functor") {
        auto fun = gen_func{42};

        auto matcher = "int"_opt->*fun;
        auto tuple = hana::make_tuple(
               hana::string_c<'i', 'n', 't'>,
               gen_func{42},
               hana::type_c<std::string_view>
        );

        CHECK(std::string_view{matcher[0_c].c_str()} == tuple[0_c].c_str());
        CHECK(matcher[1_c] == tuple[1_c]);
        CHECK(type_c<decltype(matcher[2_c])> == type_c<decltype(tuple[2_c])>);
    }

    SECTION("option creates their help appropriately") {
        auto matcher = "int"_opt["help string"]->*[](int) {};
        (void)matcher;

        CHECK(help<hana::string<'i', 'n', 't'>>.msg == "help string");
    }
}
