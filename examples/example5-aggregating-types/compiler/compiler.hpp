//&!Copyright
//
// Created by bodand on 2020-06-03.
//

#pragma once

// stdlib
#include <vector>
#include <string_view>
#include <fstream>

struct compiler {
    explicit compiler(std::vector<std::string_view> files)
           : _files{std::move(files)} {}

    inline void compile(std::string name, const std::vector<std::string>& warnings,
                 int optlvl) {
        std::ofstream out{name};
        out << "Compiled files: ";
        for (auto file : _files) {
            out << file << " ";
        }
        out << std::endl;
        out << "Warnings used: ";
        for (auto warn : warnings) {
            out << warn << " ";
        }
        out << std::endl;
        out << "Optimization level used: " << optlvl << std::endl;
    }

private:
    std::vector<std::string_view> _files;
};
