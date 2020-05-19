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
#include <string>
#include <string_view>
#include <sstream>
#include <charconv>
#include <cstdlib>
#include <algorithm>

// info::utils
#include <info/_macros.hpp>
#include <info/expected.hpp>

// Boost.Hana
#define BOOST_HANA_CONFIG_ENABLE_STRING_UDL
#include <boost/hana/type.hpp>
#include <boost/hana/string.hpp>

namespace info::cli {
  using namespace boost::hana::literals;
  template<class>
  struct type_parser;

  namespace impl {
    template<class T>
    INFO_CONSTEVAL type_parser<T> wrapped_type_parser(boost::hana::basic_type<T>) {
        return {};
    }
  }

  template<class T>
  struct type_parser {
      // default - low-performance parser
      info::expected<T, std::string_view>
      operator()(std::string_view str) {
          T tmp;
          std::istringstream ss{str.data()};
          if (ss >> tmp)
              return tmp;
          return INFO_UNEXPECTED{std::string_view{"Default parsing failed with istringstream"}};
      }
  };
}
