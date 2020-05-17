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
#include "type_hash.hpp"
#include "type_parser.hpp"
#include "split.hpp"

namespace info::cli {
  namespace impl {
    struct typed_callback {
        type_hash _hash;
        info::functor<void(std::string_view)> _func;

        constexpr bool is(impl::type_hash hash) {
            return _hash == hash;
        }

        void operator()(std::string_view str) {
            _func(str);
        }
    };
  }

  template<class... Matchers>
  struct cli_parser {
      cli_parser(Matchers... ms) {
          boost::hana::for_each(
                 boost::hana::make_tuple(ms...),
                 [&](auto tpl) {
                   boost::hana::unpack( // this unpack-fuse-unpack combo exists
                                        // because Matchers are tuples thus
                                        // we have 1-tuples holding
                                        // tuples, so the first unpack gets the
                                        // tuple from the 1-tuple, then fuse
                                        // turns the returned 2-tuple into
                                        // the key hana::string and the signature
                                        // 2-tuple which will get unpacked into
                                        // the hana::type and the function callback
                          tpl,
                          boost::hana::fuse([&](auto key, auto sig) {
                            _val[key.c_str()] = boost::hana::unpack(
                                   sig,
                                   [](auto T, auto func) {
                                     return impl::typed_callback{
                                            impl::mk_type_hash(T),
                                            [=](std::string_view arg) {
                                              func(*(impl::wrapped_type_parser(T)(arg)));
                                            }
                                     };
                                   });
                          })
                   );
                 }
          );
      };

      std::vector<std::string_view>
      operator()(int argc, char** argv) const {
          std::vector<std::string_view> ret;
          ret.emplace_back(argv[0]); // toss program name where I don't see it

          for (int i = 1; i < argc; ++i) {
              // todo
          }

          return {};
      }

  private:
      std::unordered_map<std::string, impl::typed_callback> _val;
  };
}
