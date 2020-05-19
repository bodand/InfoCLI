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
// Created by bodand on 2020-05-19.
//

#include <info/cli/type_parser.hpp>

namespace info::cli {
  info::expected<std::string, std::string_view>
  type_parser<std::string>::operator()(std::string_view str) {
      return std::string{str};
  }

  info::expected<std::string_view, std::string_view>
  type_parser<std::string_view>::operator()(std::string_view str) {
      return str;
  }

#define INTEGRAL_PARSER_IMPL(T)                                                \
  info::expected<T, std::string_view>                                          \
  type_parser<T>::operator()(std::string_view str) const noexcept {            \
      if (str.empty()) {                                                       \
          return INFO_UNEXPECTED{std::string_view{                             \
                 "empty value for " #T " typed option"                         \
          }};                                                                  \
      }                                                                        \
      T i;                                                                     \
      auto[p, ex] = std::from_chars(                                           \
             str.data(),                                                       \
             str.data() + str.size(),                                          \
             i                                                                 \
      );                                                                       \
      if (ex == std::errc{})                                                   \
          return i;                                                            \
      return INFO_UNEXPECTED{std::string_view{                                 \
             "could not parse " #T                                             \
      }};                                                                      \
  }

  INTEGRAL_PARSER_IMPL(short)

  INTEGRAL_PARSER_IMPL(unsigned short)

  info::expected<int, std::string_view>
  type_parser<int>::operator()(std::string_view str) const noexcept {
      if (str.empty()) {
          return unexpected{std::string_view{"empty value for " "int" " typed option"}};
      }
      int i;
      auto[p, ex]=std::from_chars(str.data(), str.data() + str.size(), i);
      if (ex == std::errc{})
          return i;
      return unexpected{std::string_view{"could not parse " "int"}};
  }

  INTEGRAL_PARSER_IMPL(unsigned)

  INTEGRAL_PARSER_IMPL(long)

  INTEGRAL_PARSER_IMPL(unsigned long)

  INTEGRAL_PARSER_IMPL(long long)

  INTEGRAL_PARSER_IMPL(unsigned long long)

#undef INTEGRAL_PARSER_IMPL

  info::expected<char, std::string_view>
  type_parser<char>::operator()(std::string_view str) const noexcept {
      if (str.size() >= 1)
          return str[0];
      return INFO_UNEXPECTED{
             std::string_view{"empty value for char typed option"}
      };
  }

  info::expected<unsigned char, std::string_view>
  type_parser<unsigned char>::operator()(std::string_view str) const noexcept {
      if (str.size() >= 1)
          return static_cast<unsigned char>(str[0]);
      return INFO_UNEXPECTED{
             std::string_view{"empty value for unsigned char typed option"}
      };
  }

  info::expected<float, std::string_view>
  type_parser<float>::operator()(std::string_view str) const noexcept {
      char* ptr;
      float val = std::strtof(str.data(), &ptr);
      if (str.data() == ptr) // couldn't parse shit
          return INFO_UNEXPECTED{
                 std::string_view{"could not parse float"}
          };
      return val;
  }

  info::expected<double, std::string_view>
  type_parser<double>::operator()(std::string_view str) const noexcept {
      char* ptr;
      double val = std::strtod(str.data(), &ptr);
      if (str.data() == ptr) // couldn't parse shit
          return INFO_UNEXPECTED{
                 std::string_view{"could not parse double"}
          };
      return val;
  }

  info::expected<long double, std::string_view>
  type_parser<long double>::operator()(std::string_view str) const noexcept {
      char* ptr;
      long double val = std::strtold(str.data(), &ptr);
      if (str.data() == ptr) // couldn't parse shit
          return INFO_UNEXPECTED{
                 std::string_view{"could not parse long double"}
          };
      return val;
  }

  info::expected<bool, std::string_view>
  type_parser<bool>::operator()(std::string_view str) const noexcept {
      if (str.empty())
          return false;
      std::string lc{str};
      std::transform(lc.begin(), lc.end(), lc.begin(), ::tolower);
      return !(str == "false"
               || str == "off"
               || str == "0");
  }
}

