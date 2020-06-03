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

// stdlib
#include <string>
#include <exception>
#include <iostream>

// info::cli
#include <info/cli.hpp>
using namespace info::cli::literals;

// project
#include "compiler/compiler.hpp"

int main(int argc, char** argv) {
    std::string exec_name;
    int optlvl;
    std::vector<std::string> warns;

    auto cli = info::cli::cli_parser::make(
           "o|output"_opt["The output executable's name"_hlp]->*exec_name,
           "O"_opt["The optimization's level"_hlp]->*optlvl,
           "W"_opt["Warnings to be checked for during compilation"_hlp]->*warns
    )();

    std::vector<std::string_view> args;
    try {
        args = cli(argc, argv);
    } catch(const std::exception& ex) {
        std::cout << "encountered error while parsing input arguments: " << ex.what() << std::endl;
        return -1;
    }

    compiler comp{args};
    comp.compile(exec_name, warns, optlvl);

    return 0;
}
