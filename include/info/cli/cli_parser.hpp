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
#include <vector>
#include <string_view>
#include <string>

// Boost.Hana
#include <boost/hana/tuple.hpp>
#include <boost/hana/fuse.hpp>
#include <boost/hana/unpack.hpp>
#include <boost/hana/for_each.hpp>
#include <boost/hana/at_key.hpp>

// info::utils
#include <info/_macros.hpp>
#include <info/functor.hpp>

// project
#include "type_data.hpp"
#include "error_reporter.hpp"
#include "type_parser.hpp"
#include "split.hpp"

namespace info::cli {
  namespace impl {
    struct typed_callback {
        typed_callback()
               : _data{impl::rt_type_data::make(hana::type_c<void>)},
                 _func{[](auto) {}} {}

        template<class Functor>
        typed_callback(rt_type_data data, Functor&& func)
               : _data{data},
                 _func{std::forward<Functor>(func)} {}

        impl::rt_type_data _data;
        info::functor<void(std::string_view)> _func;
    };
  }

  template<class... Matchers>
  struct cli_parser {
      cli_parser(Matchers... ms) {
          (boost::hana::for_each(
                 ms,
                 boost::hana::fuse([&](auto key, auto sig) {
                   _val[key.c_str()] = boost::hana::unpack(
                          sig,
                          [](auto T, auto func) {
                            return impl::typed_callback{
                                   impl::rt_type_data::make(T),
                                   [=](std::string_view arg) {
                                     auto parsed = impl::wrapped_type_parser(T)(arg);

                                     if (parsed) {
                                         func(*parsed);
                                     } else {
                                         _error(parsed.error());
                                     }
                                   }
                            };
                          }
                   );
                 })
          ), ...);
      };

      std::vector<std::string_view>
      operator()(int argc, char** argv) {
          std::vector<std::string_view> ret;
          ret.emplace_back(argv[0]); // toss program name where I don't see it

          for (int i = 1; i < argc; ++i) {
              std::string_view arg{argv[i]};

              // filter end of options
              if (arg == "--") {
                  finish_all(argc, argv, ret, i);
                  return ret;
              }

              // filter empty input
              if (arg.empty()) {
                  ret.emplace_back("");
                  continue;
              }

              // filter non options
              if (arg[0] != '-' && arg.size() > 1) {
                  ret.emplace_back(std::move(arg));
                  continue;
              }

              if (arg[1] == '-') { // --<...>
                  handle_long_opt(arg, argc, argv, ret, i);
              } else { // -<...>
                  handle_short_opt(arg, argc, argv, ret, i);
              }
          }

          return ret;
      }

  private:

      void finish_all(int argc, char** argv,
                      std::vector<std::string_view>& ret,
                      int i) const {
          for (int j = i + 1; j < argc; ++j) {
              ret.emplace_back(argv[j]);
          }
      }

      void handle_long_opt(std::string_view arg,
                           int argc, char** argv,
                           std::vector<std::string_view>& ret,
                           int& i) {
          auto[opt, val] = impl::split(arg);

          // "--opt=val"
          if (!val.empty()) {
              auto iter = _val.find(opt);
              if (iter == _val.end()) {
                  _error("Unexpected option found: " + opt + ". Use --help for usage.");
                  return;
              }
              auto&[__, fn] = iter->second;
              fn(val);
              return;
          }

          // "--opt="
          if (arg.back() == '=') {
              auto iter = _val.find(opt);
              auto&[__, fn] = iter->second;
              fn("");
              return;
          }

          // "--opt" => opt = arg
          if (auto iter = _val.find(opt);
                 iter != _val.end()) {
              auto&[data, fn] = iter->second;

              if (data.allow_nothing) { // "--opt"
                  fn(data.default_val);
                  return;
              }

              if (i + 1 < argc) { // "--opt" "val"
                  ++i;
                  fn(argv[i]);
                  return;
              }
              _error("Expected value after encountering option: " +
                     opt + ". But found end of input. Use --help for usage.");
              return;
          }
          _error("Unexpected option found: " + opt + ". Use --help for usage.");
      }

      void handle_short_opt(std::string_view arg,
                            int argc, char** argv,
                            std::vector<std::string_view>& ret,
                            int i) const {
          // todo
      }

      inline static error_reporter<> _error;
      std::unordered_map<std::string, impl::typed_callback> _val;
  };
}
