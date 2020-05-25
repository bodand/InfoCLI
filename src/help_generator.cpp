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

#include <sstream>

// {fmt}
#include <fmt/format.h>

#include <info/cli/help_generator.hpp>

void info::cli::impl::help_generator::help_str::add_opt(std::string_view str) noexcept {
    (_opts += ' ') += str;
}

std::string
info::cli::impl::help_generator::help_str::print() const noexcept {
    if (__builtin_expect(_opts.size() > 24, 0)) {
        return fmt::format("  {}\n                        ", _opts);
    } else {
        return fmt::format("  {:<22}", _opts);
    }
}

void
info::cli::impl::help_generator::print() const noexcept {
    fmt::print("Usage: {} [options]\n\n", _name);
    fmt::print("Possible options:\n");
    for (auto&&[help, opts] : _helps) {
        fmt::print("  {} {}\n", opts.print(), help);
    }
}

void info::cli::impl::help_generator::set_name(std::string_view name) {
    _name = name;
}

void info::cli::impl::help_generator::register_(std::string_view opt,
                                                std::string_view hlp) noexcept {
    if (hlp.empty()) return; // undocumented opts are ignored

    meaningful = true;
    _helps[hlp.data()].add_opt(opt.data());
}
