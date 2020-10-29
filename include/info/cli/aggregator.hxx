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

#include <info/cli/macros.hxx>
#include <info/cli/extra/repeat.hxx>

namespace info::_cli {
    template<class T>
    struct INFO_CLI_API aggregator_ : std::false_type {
        using type = T;
    };

    template<class T>
    struct INFO_CLI_API aggregator_<cli::repeat<T>> : std::true_type {
        using type = T;

        template<class U>
        auto /* !! CAUTION !! \
               this function contrary to all other aggregator_<T> types does NOT
               take T, but U where T is cli::repeat<U>. This is because
               we somehow need to change passed (to cli_parser) rvalue reference
               to repeat<U>&& to the within contained lvalue reference to U& */
        operator()(T& rep, U&& elem) const {
            static_assert(std::is_assignable_v<T&, decltype(std::forward<U>(elem))>,
                          "Value of aggregating type is not constructable with given type");
            if constexpr (std::is_integral_v<T>) {
                rep += std::forward<U>(elem);
            } else {
                if constexpr (auto agg = aggregator_<T>{}) {
                    agg(rep, std::forward<U>(elem));
                } else {
                    rep = std::forward<U>(elem);
                }
            }
        }
    };

    template<class T>
    struct INFO_CLI_API aggregator_<std::vector<T>> : std::true_type {
        using type = T;

        template<class U>
        auto
        operator()(std::vector<T>& cont, U&& elem) const {
            static_assert(std::is_constructible_v<T, decltype(std::forward<U>(elem))>,
                          "Value of aggregating type is not constructable with given type");
            cont.emplace_back(std::forward<U>(elem));
        }
    };

    template<class T>
    struct INFO_CLI_API aggregator_<std::list<T>> : std::true_type {
        using type = T;

        template<class U>
        auto
        operator()(std::list<T>& cont, U&& elem) const {
            static_assert(std::is_constructible_v<T, decltype(std::forward<U>(elem))>,
                          "Value of aggregating type is not constructable with given type");
            cont.emplace_back(std::forward<U>(elem));
        }
    };

    template<class T>
    struct INFO_CLI_API aggregator_<std::stack<T>> : std::true_type {
        using type = T;

        template<class U>
        auto
        operator()(std::stack<T>& cont, U&& elem) const {
            static_assert(std::is_constructible_v<T, decltype(std::forward<U>(elem))>,
                          "Value of aggregating type is not constructable with given type");
            cont.emplace(std::forward<U>(elem));
        }
    };

    template<class T>
    struct INFO_CLI_API aggregator_<std::queue<T>> : std::true_type {
        using type = T;

        template<class U>
        auto
        operator()(std::queue<T>& cont, U&& elem) const {
            static_assert(std::is_constructible_v<T, decltype(std::forward<U>(elem))>,
                          "Value of aggregating type is not constructable with given type");
            cont.emplace(std::forward<U>(elem));
        }
    };

    template<class T>
    struct INFO_CLI_API aggregator_<std::deque<T>> : std::true_type {
        using type = T;

        template<class U>
        auto
        operator()(std::deque<T>& cont, U&& elem) const {
            static_assert(std::is_constructible_v<T, decltype(std::forward<U>(elem))>,
                          "Value of aggregating type is not constructable with given type");
            cont.emplace_back(std::forward<U>(elem));
        }
    };

    template<class T>
    INFO_CLI_LOCAL constexpr static auto aggregator = aggregator_<T>{};

    template<class T>
    using aggregator_type = typename aggregator_<T>::type;
}
