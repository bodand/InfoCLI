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
#include <boost/hana/equal.hpp>

// Boost.MP11
#include <boost/mp11/list.hpp>
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/utility.hpp>

// info::utils
#include <info/_macros.hpp>

// project
#include "meta.hpp"

namespace info::cli::impl {
  namespace hana = boost::hana;
  using namespace boost::mp11;

  class split_ {
      template<class T>
      struct stringify_;

      template<
             class T,
             T... Args
      >
      struct stringify_<hana::tuple<hana::integral_constant<T, Args>...>> {
          using type = hana::string<Args...>;
      };

      template<class T>
      using stringify = typename stringify_<T>::type;

      template<class S>
      using tuplify = decltype(hana::to_tuple(S{}));

      template<class Ch, class Tpl>
      struct f_ {
          using type = mp_if_c<
                 Ch{} == hana::char_c<'|'>,
                 mp_push_front<Tpl, hana::string<>>,
                 mp_append<
                        hana::tuple<
                               stringify<mp_push_front<
                                      tuplify<mp_eval_if<
                                             mp_empty<Tpl>,
                                             hana::string<>,
                                             mp_front,
                                             Tpl
                                      >>,
                                      Ch
                               >>
                        >,
                        meta::tail<Tpl>
                 >
          >;
      };

      template<class Ch, class Tpl>
      using f = typename f_<Ch, Tpl>::type;

  public:
      template<class Str>
      INFO_CONSTEVAL auto operator()(Str) const {
          return mp_reverse_fold<tuplify<Str>, hana::tuple<>, f>{};
      }
  };

  INFO_CONSTINIT const static split_ meta_split{};
}
