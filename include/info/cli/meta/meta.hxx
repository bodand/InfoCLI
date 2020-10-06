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

#pragma once

// stdlib
#include <deque>
#include <list>
#include <queue>
#include <stack>
#include <type_traits>
#include <vector>

#include <info/cli/aggregator.hxx>
#include <info/cli/macros.hxx>

namespace info::cli::meta {
    template<class... Args>
    struct INFO_CLI_LOCAL tlist {
    };

    // head --------------------------------------------------------------------
    template<class>
    struct head_;

    template<template<class...> class L,
             class T, class... Args>
    struct head_<L<T, Args...>> {
        using type = T;
    };

    template<class L>
    using head = typename head_<L>::type;

    // is_aggregating ----------------------------------------------------------
    template<class T>
    struct INFO_CLI_LOCAL is_aggregating_ : std::bool_constant<_cli::aggregator_<T>::value> {
        using type = T;
    };

    template<class T>
    INFO_CLI_LOCAL constexpr static bool is_aggregating = is_aggregating_<T>::value;

    template<class T>
    using aggregating_type = typename is_aggregating_<T>::type;
}
