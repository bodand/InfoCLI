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

// project
#include "meta.hpp"
#include "dissector.hpp"
#include "matcher.hpp"

namespace info::cli {
  template<class T>
  struct help_t {
      constexpr void operator()(std::string_view txt) const {
          msg = txt;
      }

      mutable std::string_view msg;
  };

  template<class T>
  INFO_CONSTINIT static help_t<T> help;

  template<class CharT, CharT... str>
  struct option_str {
      template<class T,
             typename = std::enable_if_t<
                    !impl::is_typed_callback<T>
                    && !std::is_invocable_v<T, std::string_view>
             >
      >
      // variable callbacks with a bare reference
      INFO_CONSTEVAL auto operator->*(T& ref) const {
          return boost::hana::make_tuple(
                 impl::matcher(boost::hana::string_c<str...>),
                 [&ref](const T& arg) {
                     ref = arg;
                 },
                 boost::hana::type_c<T>
          );
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

          return boost::hana::make_tuple(
                 impl::matcher(boost::hana::string_c<str...>),
                 std::forward<Fun>(fun),
                 boost::hana::type_c<
                        meta::head<PassType>
                 >
          );
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
      INFO_CONSTEVAL auto operator->*(Fun&& fun) const {
          static_assert(std::is_invocable_v<Fun, std::string_view>,
                        "Passed generic lambdas must be callable with std::string_view");

          return boost::hana::make_tuple(
                 impl::matcher(boost::hana::string_c<str...>),
                 std::forward<Fun>(fun),
                 boost::hana::type_c<std::string_view>
          );
      }

      INFO_CONSTEVAL auto operator[](std::string_view txt) const {
          help<boost::hana::string<str...>>(txt);
          return *this;
      }
  };

  template<class CharT, CharT... cs>
  constexpr option_str<CharT, cs...> operator ""_opt() {
      return {};
  }
}
