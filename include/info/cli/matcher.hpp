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

#pragma once

// stdlib
#include <unordered_set>
#include <string_view>

// Boost.Hana
#include <boost/hana/string.hpp>
#include <boost/hana/transform.hpp>
#include <boost/hana/for_each.hpp>
#include <boost/hana/plus.hpp>

// info::utils
#include <info/_macros.hpp>

// project
#include "split.hpp"

#ifndef INFO_CLI_SHORT_OPT_DASH // todo document this
#  define INFO_CLI_SHORT_OPT_DASH "-"
#endif

#ifndef INFO_CLI_LONG_OPT_DASH // todo document this
#  define INFO_CLI_LONG_OPT_DASH "--"
#endif

namespace info::cli::impl {
  namespace hana = boost::hana;

  template<class Str>
  struct matcher {
      constexpr explicit matcher(Str str) {
          auto matchers = hana::transform(
                 split(str),
                 [](auto x) {
                   if (hana::length(x) == hana::size_c<1>) {
                       return std::string{INFO_CLI_SHORT_OPT_DASH} + x.c_str();
                   }
                   return std::string{INFO_CLI_LONG_OPT_DASH} + x.c_str();
                 }
          );
          hana::for_each(matchers, [this](auto&& x) {
            _matchers.emplace(std::move(x));
          });
      }

      bool operator()(std::string_view str) {
          return _matchers.find(std::string{str}) != _matchers.end();
      }

  private:
      std::unordered_set<std::string> _matchers{};
  };

  template<class T, class U>
  INFO_CONSTEVAL std::false_type operator==(const matcher<T>&, const matcher<U>&) {
      return {};
  }

  template<class T>
  INFO_CONSTEVAL std::true_type operator==(const matcher<T>&, const matcher<T>&) {
      return {};
  }

  template<class T, class U>
  INFO_CONSTEVAL std::true_type operator!=(const matcher<T>&, const matcher<U>&) {
      return {};
  }

  template<class T>
  INFO_CONSTEVAL std::false_type operator!=(const matcher<T>&, const matcher<T>&) {
      return {};
  }
}
