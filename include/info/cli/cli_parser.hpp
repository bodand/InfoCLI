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
#include <boost/hana/flatten.hpp>
#include <boost/hana/tuple.hpp>
#include <boost/hana/fuse.hpp>
#include <boost/hana/unpack.hpp>
#include <boost/hana/for_each.hpp>

// info::utils
#include <info/_macros.hpp>
#include <info/functor.hpp>

// project
#include "meta.hpp"
#include "type_parser.hpp"

namespace info::cli {
  template<class... Matchers>
  struct cli_parser {
      cli_parser(Matchers... ms) {
          boost::hana::for_each(
                 boost::hana::make_tuple(ms...),
                 [&](auto tpl) {
                   boost::hana::unpack(tpl, boost::hana::fuse([&](auto Key, auto Sig) {
                     _val[Key.c_str()] = boost::hana::unpack(Sig, [](auto T, auto func) {
                       return [=](std::string_view arg) {
                         func(*(impl::wrapped_type_parser(T)(arg)));
                       };
                     });
                   }));
                 }
          );
      };

      std::vector<std::string_view>
      operator()(int argc, char** argv) const {
      }

  private:
      std::unordered_map<std::string, info::functor<void(std::string_view)>> _val;
  };
}
