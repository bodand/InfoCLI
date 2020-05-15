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

// test'd
#include <info/cli/val_callback.hpp>
using namespace info::cli::impl;

TEST_CASE("val_callback tests", "[val_callback][impl]") {
    SECTION("val_callback construction does not modify anything") {
        int i = 42;

        auto f = val_callback{i};
        (void)f;

        CHECK(i == 42);
    }

    SECTION("call to val_callback changes value") {
        int i = 42;
        auto f = val_callback{i};

        f(46);

        CHECK(i == 46);
    }

    SECTION("call to val_callback can be made with any assignable type") {
        int i = 42;
        auto f = val_callback{i};

        f(46.5f);

        CHECK(i == 46);
    }

    SECTION("call to val_callback is noexcept when possible") {
        int i = 42;
        auto fun = val_callback{i};

        CHECK(noexcept(fun(2)));
    }

    SECTION("call to val_callback isn't noexcept when not possible") {
        struct foo {
            foo& operator=(foo) {
                return *this;
            }
        };
        foo f1, f2;
        auto fun = val_callback{f1};

        CHECK_FALSE(noexcept(fun(f2)));
    }
}
