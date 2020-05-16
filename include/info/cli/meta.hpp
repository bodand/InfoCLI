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
// Created by bodand on 2020-05-16.
//

#pragma once

namespace info::cli::meta {
  template<class... Args>
  struct tlist {
  };

  template<auto V>
  struct cval {
      constexpr const static auto value = V;
  };

  // size ---------------------------------------------------------------------
  template<class>
  struct size_;

  template<
         template<class...> class L, class... Args
  >
  struct size_<L<Args...>> {
      using type = cval<sizeof...(Args)>;
  };

  template<class T>
  using size = typename size_<T>::type;

  // head ---------------------------------------------------------------------
  template<class>
  struct head_;

  template<
         template<class...> class L, class T, class... Args
  >
  struct head_<L<T, Args...>> {
      using type = T;
  };

  template<class T>
  using head = typename head_<T>::type;

  // nest ---------------------------------------------------------------------
  template<class, template<class, class> class>
  struct nest_;

  template<
         template<class, class> class T,
         template<class, class, class> class L,
         class Args1, class Args2, class Args3
  > // I don't think this needs to be universal
  struct nest_<L<Args1, Args2, Args3>, T> {
      using type = T<Args1, T<Args2, Args3>>;
  };

  template<class L, template<class, class> class T>
  using nest = typename nest_<L, T>::type;
}
