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
// Created by bodand on 2020-05-21.
//


// stdlib
#include <iostream>
#include <string>
#include <cstdlib>
#include <filesystem>
namespace fs = std::filesystem;

// InfoCLI
#include <info/cli.hpp>
using namespace info::cli::literals;

int main(int argc, char** argv) {
    std::string inp;
    std::string outp;

    // set-up parser
    info::cli::cli_parser cli{
           "i|input-file"_opt["The file to copy from"_hlp]->*inp,
           "o|output-file"_opt["The file to copy to"_hlp]->*outp,

           "v|version"_opt["Prints the version of the utility"_hlp]->*[](bool) {
             std::cout << '3' << std::endl;
             std::exit(1);
           }
    };

    // perform parsing
    std::vector<std::string_view> rem;
    try {
        rem = cli(argc, argv);
    } catch (std::exception& ex) {
        std::cerr << "Unable to parse provided arguments: " << ex.what();
        return -1;
    }

    // check required values to have been set
    if (inp.empty()) {
        std::cerr << "The input file must be set.";
        return 1;
    }
    if (outp.empty()) {
        std::cerr << "The output file must be set.";
        return 2;
    }

    // perform copy
    try {
        fs::copy(inp, outp);
    } catch (std::exception& ex) {
        std::cerr << "Error occurred while copying: " << ex.what();
        return -2;
    }
}
