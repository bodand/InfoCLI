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
#include <iostream>

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
#include "help_generator.hpp"
#include "split.hpp"

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

  struct cli_parser {
      template<class... Matchers>
      cli_parser(Matchers... ms) {
          (boost::hana::for_each(
                 ms,
                 boost::hana::fuse([&](auto key, auto help, auto T, auto func) {
                   _help.register_(key, help);

                   _val.try_emplace(key.c_str(), impl::typed_callback{
                          impl::rt_type_data::make(T),
                          [=](std::string_view arg) {
                            auto parsed = impl::wrapped_type_parser(T)(arg);

                            if (parsed) {
                                func(*parsed);
                            } else {
                                _error(parsed.error());
                            }
                          }
                   });
                 })
          ), ...);

          if (_help.meaningful) {
              _val.try_emplace("--help", impl::typed_callback{
                     impl::rt_type_data::make(boost::hana::type_c<bool>),
                     [&](std::string_view arg) {
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
                     }
              });
              _val.try_emplace("-h", impl::typed_callback{
                     impl::rt_type_data::make(boost::hana::type_c<bool>),
                     [&](std::string_view arg) {
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
                     }
              });
          }
      }

      INFO_NODISCARD("Return value is the remaining non-option values in the input args")
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
      std::unordered_map<std::string, impl::typed_callback> _val;
      impl::help_generator _help;
  };
}
