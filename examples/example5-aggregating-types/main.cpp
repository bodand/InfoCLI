//&!Copyright
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
