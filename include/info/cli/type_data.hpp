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
// Created by bodand on 2020-05-18.
//

#pragma once

// stdlib
#include <string_view>
#include <type_traits>

// Boost.Hana
#include <boost/hana/type.hpp>

namespace info::cli {
  namespace impl {
    template<class T>
    struct is_non_digit_arithmetic : std::is_arithmetic<T> {
    };

    template<>
    struct is_non_digit_arithmetic<char> : std::false_type {
    };
    template<>
    struct is_non_digit_arithmetic<signed char> : std::false_type {
    };
    template<>
    struct is_non_digit_arithmetic<unsigned char> : std::false_type {
    };
    template<>
    struct is_non_digit_arithmetic<wchar_t> : std::false_type {
    };
#ifdef __cpp_char8_t
    template<>
    struct is_non_digit_arithmetic<char8_t> : std::false_type {};
#endif
    template<>
    struct is_non_digit_arithmetic<char16_t> : std::false_type {
    };
    template<>
    struct is_non_digit_arithmetic<char32_t> : std::false_type {
    };
  }

  enum class parse_type {
      alpha,
      alphanumeric,
      numeric
  };

  template<class T>
  struct type_data {
      constexpr static bool allow_nothing = false;
      constexpr static std::string_view default_value = "";
      constexpr static int length = -1;
      constexpr static parse_type expected_type =
             impl::is_non_digit_arithmetic<T>::value ? parse_type::numeric
                                                     : parse_type::alphanumeric;
  };

  template<>
  struct type_data<bool> {
      constexpr static bool allow_nothing = true;
      constexpr static std::string_view default_value = "1";
      constexpr static int length = -1;
      constexpr static parse_type expected_type = parse_type::alphanumeric;
  };

  namespace impl {
    struct rt_type_data {
        bool allow_nothing;
        std::string_view default_val;
        int length;
        parse_type expected_type;

        template<class T>
        static rt_type_data make(boost::hana::basic_type<T>) {
            return {type_data<T>{}};
        };

    private:
        template<class T>
        rt_type_data(type_data<T> td)
               : allow_nothing{td.allow_nothing},
                 default_val{td.default_value},
                 length{td.length},
                 expected_type{td.expected_type} {}
    };
  }
}

