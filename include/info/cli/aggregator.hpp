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
// Created by bodand on 2020-06-03.
//

#pragma once

// stdlib
#include <vector>
#include <list>
#include <stack>
#include <deque>
#include <queue>

namespace info::cli::impl {
  template<class T>
  struct aggregator_;

  template<class T>
  struct aggregator_<std::vector<T>> {
      template<class U>
      auto operator()(std::vector<T>& cont, const U& elem) const {
          static_assert(std::is_constructible_v<T, const U&>,
                        "Value of aggregating type is not constructable with given type");
          cont.emplace_back(elem);
      }
  };

  template<class T>
  struct aggregator_<std::list<T>> {
      template<class U>
      auto operator()(std::list<T>& cont, const U& elem) const {
          static_assert(std::is_constructible_v<T, const U&>,
                        "Value of aggregating type is not constructable with given type");
          cont.emplace_back(elem);
      }
  };

  template<class T>
  struct aggregator_<std::stack<T>> {
      template<class U>
      auto operator()(std::stack<T>& cont, const U& elem) const {
          static_assert(std::is_constructible_v<T, const U&>,
                        "Value of aggregating type is not constructable with given type");
          cont.emplace(elem);
      }
  };

  template<class T>
  struct aggregator_<std::queue<T>> {
      template<class U>
      auto operator()(std::stack<T>& cont, const U& elem) const {
          static_assert(std::is_constructible_v<T, const U&>,
                        "Value of aggregating type is not constructable with given type");
          cont.emplace(elem);
      }
  };

  template<class T>
  struct aggregator_<std::deque<T>> {
      template<class U>
      auto operator()(std::stack<T>& cont, const U& elem) const {
          static_assert(std::is_constructible_v<T, const U&>,
                        "Value of aggregating type is not constructable with given type");
          cont.emplace(elem);
      }
  };

  template<class T>
  constexpr static auto aggregator = aggregator_<T>{};
}
