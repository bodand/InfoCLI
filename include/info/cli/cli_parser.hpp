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
#include <unordered_map>
#include <string_view>
#include <string>
#include <utility>
#include <functional>

// Boost.Hana
#include <boost/hana/tuple.hpp>
#include <boost/hana/fuse.hpp>
#include <boost/hana/unpack.hpp>
#include <boost/hana/for_each.hpp>
#include <boost/hana/at_key.hpp>

// info::utils
#include <info/_macros.hpp>
#include <info/functor.hpp>

// parallel map
#include "phmap.hpp"

// project
#include "type_data.hpp"
#include "error_reporter.hpp"
#include "type_parser.hpp"
#include "help_generator.hpp"
#include "split.hpp"
#include "meta.hpp"

#ifndef INFO_CLI_HELP_END_EXECUTION
#  define INFO_CLI_HELP_END_EXECUTION true
#endif

namespace info::cli {
  namespace impl {
    struct typed_callback {
        typed_callback();

        template<class Functor>
        typed_callback(rt_type_data data, Functor&& func)
               : _data{data},
                 _func{std::forward<Functor>(func)} {}

        impl::rt_type_data _data;
        info::functor<void(std::string_view)> _func;
    };
  }

  class cli_parser {
      [[gnu::always_inline]]
      auto mk_help_callback() const {
          return [this](std::string_view arg) {
            auto parsed = type_parser<bool>{}(arg);

            if (parsed
                && *parsed) {
                _help.print();
                if constexpr (INFO_CLI_HELP_END_EXECUTION) {
                    std::exit(1);
                }
            } else {
                _error(parsed.error());
            }
          };
      }

      template<class TT, class FuncT>
      [[gnu::always_inline]]
      static auto mk_callback_impl(std::string_view key, std::string_view help,
                                   TT T, FuncT func) {
          return [
                 key{std::move(key)},
                 help{std::move(help)},
                 T{std::move(T)},
                 func{std::move(func)}
          ](auto& val, auto& help_) {
            help_.register_(key, help);

            val.try_emplace(key.data(), impl::typed_callback{
                   impl::rt_type_data::make(T),
                   [T, func](std::string_view arg) {
                     auto parsed = impl::wrapped_type_parser(T)(arg);

                     if (parsed) {
                         func(*parsed);
                     } else {
                         _error(parsed.error());
                     }
                   }
            });
          };
      }

      template<class KeyT, class HelpT, class TT, class FuncT>
      [[gnu::always_inline]]
      static auto mk_callback(KeyT key, HelpT help, TT T, FuncT func) {
          return mk_callback_impl(key.c_str(), help.c_str(), T, func);
      }

      using map_type = phmap::flat_hash_map<std::string_view, impl::typed_callback>;

      template<std::size_t N>
      cli_parser(const std::array<std::function<void(map_type&, impl::help_generator&)>, N>&& regs) {
          static_assert(N > 0,
                        "The zero length matcher registry set is ill-formed");

          for (auto&& reg : regs) {
              reg(_val, _help);
          }

          if (_help.meaningful) {
              _val.try_emplace("--help", impl::typed_callback{
                     impl::rt_type_data::make(boost::hana::type_c<bool>),
                     mk_help_callback()
              });
              _val.try_emplace("-h", impl::typed_callback{
                     impl::rt_type_data::make(boost::hana::type_c<bool>),
                     mk_help_callback()
              });
          }

          _has_help = _val.find("--help") != _val.end();
      }

      template<std::size_t N>
      INFO_CONSTEVAL static auto mk_maker(
             std::array<std::function<void(map_type&, impl::help_generator&)>, N> arr
      ) {
          return [arr{std::move(arr)}]() {
            return cli_parser(std::move(arr));
          };
      }

  public:
      template<class... Matchers>
      [[gnu::flatten]]
      INFO_CONSTEVAL static auto
      make(Matchers... ms) {
          static_assert(sizeof...(Matchers) > 0,
                        "The zero length matcher set is ill-formed");

          std::array<std::function<void(map_type&, impl::help_generator&)>,
                 (0 + ... + meta::size<Matchers>::value)
          > arr;
          std::size_t i = 0;

          (boost::hana::for_each(
                 ms,
                 boost::hana::fuse([&](auto key, auto help, auto T, auto func)
                                          __attribute__((flatten)) {
                   arr[i++] = mk_callback(key, help, T, func);
                 })
          ), ...);

          return mk_maker(std::move(arr));
      }

      INFO_NODISCARD_JUST//("Return value is the remaining non-option values in the input args")
      std::vector<std::string_view>
      operator()(int argc, char** argv);

  private:
      void finish_all(int argc, char** argv,
                      std::vector<std::string_view>& ret,
                      int i) const;

      void handle_long_opt(std::string_view arg,
                           int argc, char** argv,
                           int& i);

      void handle_short_opt(std::string_view arg,
                            int argc, char** argv,
                            int i);

      static error_reporter<> _error;
      map_type _val;
      impl::help_generator _help;
      bool _has_help;
  };
}
