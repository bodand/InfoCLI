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

#pragma once

// stdlib
#include <type_traits>

// project
#include <info/cli/macros.hxx>
#include <info/cli/meta/meta.hxx>

namespace info::cli::meta {
    template<class T, class>
    struct INFO_CLI_LOCAL dissector : std::false_type {
        using type = tlist<T>;
    };

    template<class F>
    class INFO_CLI_LOCAL dissector<
           F,
           std::void_t<decltype(&F::operator())> > : public std::true_type {

        template<class R, class C, class... ArgsT>
        static constexpr tlist<ArgsT...>
        dis(R (C::*)(ArgsT...)) {
            return {};
        }

        template<class R, class C, class... ArgsT>
        static constexpr tlist<ArgsT...>
        dis(R (C::*)(ArgsT...) const) {
            return {};
        }

    public:
        constexpr const static auto ptr = &F::operator();

        using type = decltype(dissector::dis(ptr));
    };

    template<class R, class... Args>
    struct INFO_CLI_LOCAL dissector<R(Args...), void> : std::true_type {
        using return_type = R;
        using type = tlist<Args...>;
    };

    template<class T>
    INFO_CLI_LOCAL constexpr const static auto is_typed_callback = dissector<T, void>::value;

    template<class T>
    using dissect = typename dissector<T, void>::type;
}