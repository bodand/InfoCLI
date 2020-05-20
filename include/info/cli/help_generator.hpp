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
// Created by bodand on 2020-05-20.
//

#pragma once

// stdlib
#include <unordered_map>
#include <string>
#include <vector>

// Boost.Hana
#include <boost/hana/length.hpp>
#include <boost/hana/is_empty.hpp>
#include <boost/hana/string.hpp>

namespace info::cli::impl {
  struct help_generator {
      void print() const noexcept;

      template<class OptStr, class HlpStr>
      void register_(OptStr opt, HlpStr hlp) noexcept {
          if (boost::hana::is_empty(hlp)) return; // undocumented opts are ignored

          meaningful = true;
          _helps[hlp.c_str()].add_opt(opt.c_str());
      }

      void set_name(std::string_view name);

      bool meaningful{false};
  private:
      struct help_str {
          void add_opt(std::string_view) noexcept;

          std::string print() const noexcept;

          std::string _opts{};
      };

      std::string _name{};
      std::unordered_map<std::string, help_str> _helps{};
  };
}
