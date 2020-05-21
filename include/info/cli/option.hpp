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
#include <string_view>

// Boost.Hana
#include <boost/hana/tuple.hpp>
#include <boost/hana/string.hpp>
#include <boost/hana/map.hpp>
#include <boost/hana/functional/compose.hpp>
#include <boost/hana/equal.hpp>
#include <boost/hana/integral_constant.hpp>
#include <boost/hana/length.hpp>
#include <boost/hana/not.hpp>
#include <boost/hana/filter.hpp>

// project
#include "meta.hpp"
#include "dissector.hpp"
#include "meta_split.hpp"

namespace info::cli {
  namespace impl {
    template<class MStr, class HStr>
    struct callback_impl {
        template<class S>
        using add_short_dash = decltype(boost::hana::string_c<'-'> + S{});
        template<class S>
        using add_long_dash = decltype(boost::hana::string_c<'-', '-'> + S{});

        template<class S>
        using len1 = std::integral_constant<bool, meta::csize<S>::value == 1>;
        template<class S>
        using not_len1 = std::integral_constant<bool, meta::csize<S>::value != 1>;

        constexpr static auto strs = meta::map_if<
               meta::map_if<
                      decltype(impl::meta_split(MStr{})),
                      not_len1,
                      add_long_dash
               >,
               len1,
               add_short_dash
        >{};

        template<class T, class Action, class... Strs>
        constexpr static auto mk_matcher(Action act, boost::hana::tuple<Strs...>) {
            return boost::hana::make_tuple(
                   boost::hana::make_tuple(
                          Strs{},
                          HStr{},
                          boost::hana::type_c<T>,
                          act
                   )...
            );
        }

        template<class T,
               typename = std::enable_if_t<
                      !impl::is_typed_callback<T>
                      && !std::is_invocable_v<T, std::string_view>
               >
        >
        // variable callbacks with a bare reference
        INFO_CONSTEVAL auto operator->*(T& ref) const {
            auto action = [&ref](const T& val) {
              ref = val;
            };

            return mk_matcher<T>(action, strs);
        }

        template<class Fun,
               typename = std::enable_if_t<
                      impl::is_typed_callback<std::remove_reference_t<Fun>>
                      && !std::is_invocable_v<Fun, std::string_view>
               >
        >
        // functor callback with a specified callback-type != string_view
        // hence we also do manual formatting
        INFO_CONSTEVAL auto operator->*(Fun&& fun) const {
            using PassType = impl::dissect<std::remove_reference_t<Fun>>;
            static_assert(meta::size<PassType>::value == 1,
                          "The arity of the passed callback is not 1");

            return mk_matcher<meta::head<PassType>>(std::forward<Fun>(fun), strs);
        }

        template<class Fun,
               typename = std::enable_if_t<
                      !impl::is_typed_callback<Fun>
                      && std::is_invocable_v<Fun, std::string_view>
               >,
               typename = int // just to create overloads and not a redefinition
        >
        // generic functors => we can't perform manual formatting; or
        // string requesting functors => we needn't perform any formatting
        // hence we just pass the applicable string
        INFO_CONSTEVAL auto operator->*(Fun fun) const {
            static_assert(std::is_invocable_v<Fun, std::string_view>,
                          "Passed functors must be callable with std::string_view");

            return mk_matcher<std::string_view>(std::forward<Fun>(fun), strs);
        }
    };
  }

  template<class MStr, class HStr>
  struct helpful_option_str : impl::callback_impl<
         MStr,
         HStr
  > {
  };

  template<char... cs>
  struct option_str : impl::callback_impl<
         boost::hana::string<cs...>,
         boost::hana::string<>
  > {
      template<class StrT>
      INFO_CONSTEVAL auto operator[](StrT) const {
          return helpful_option_str<boost::hana::string<cs...>, StrT>{};
      }
  };

  inline namespace literals {
    template<class CharT, CharT... c>
    constexpr option_str<c...> operator ""_opt() {
        return {};
    }

    template<class CharT, CharT... c>
    constexpr boost::hana::string<c...> operator ""_hlp() {
        return {};
    }
  }
}
