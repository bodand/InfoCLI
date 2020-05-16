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

// Boost.Hana
#include <boost/hana/string.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/integral_constant.hpp>
#include <boost/hana/contains.hpp>
#include <boost/hana/not_equal.hpp>
#include <boost/hana/plus.hpp>
#include <boost/hana/minus.hpp>
#include <boost/hana/take_back.hpp>
#include <boost/hana/take_while.hpp>
#include <boost/hana/length.hpp>
#include <boost/hana/prepend.hpp>

// info::utils
#include <info/_macros.hpp>

namespace info::cli::impl {
  namespace hana = boost::hana;

  struct split_ {
      template<class Str>
      INFO_CONSTEVAL auto operator()(Str str) const {
          return split_::split(str);
      }

  private:
      template<class T>
      struct stringify_;

      template<
             class T,
             T... Args
      >
      struct stringify_<hana::tuple<hana::integral_constant<T, Args>...>> {
          using type = hana::string<Args...>;
      };

      template<class Str_>
      INFO_CONSTEVAL static auto tuplify(Str_ str) {
          return hana::to_tuple(str);
      }

      template<class Tpl>
      INFO_CONSTEVAL static auto stringify(Tpl) {
          return typename stringify_<Tpl>::type{};
      }

      template<class Str_, class Pred>
      INFO_CONSTEVAL static auto str_take_while(Str_&& str, Pred&& pred) {
          return stringify(hana::take_while(tuplify(std::forward<Str_>(str)), std::forward<Pred>(pred)));
      }

      template<class Str_, class N>
      INFO_CONSTEVAL static auto str_take_back(Str_&& str, N&& count) {
          return stringify(hana::take_back(tuplify(std::forward<Str_>(str)), std::forward<N>(count)));
      }

      template<class Str_,
             typename = std::enable_if_t<hana::contains(Str_{}, hana::char_c<'|'>)>>
      INFO_CONSTEVAL static auto split(Str_ str) {
          // fixme: this is horrendous: recursion
          auto head = str_take_while(str, hana::not_equal.to(hana::char_c<'|'>));
          auto tail = str_take_back(str, hana::length(str) - (hana::length(head) + hana::size_c<1>));

          return hana::prepend(split_::split(tail), head);
      }

      template<class Str_,
             typename = std::enable_if_t<!hana::contains(Str_{}, hana::char_c<'|'>)>,
             typename = int>
      INFO_CONSTEVAL static auto split(Str_ str) {
          return hana::make_tuple(str);
      }

      INFO_CONSTEVAL static auto split(hana::string<>) {
          return hana::tuple{};
      }
  };

  INFO_CONSTINIT const static split_ meta_split{};
}
