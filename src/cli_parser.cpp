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

#include <vector>

#include <info/cli/cli_parser.hpp>

void info::cli::cli_parser::finish_all(int argc,
                                       char** argv,
                                       std::vector<std::string_view>& ret, int i) const {
    for (int j = i + 1; j < argc; ++j) {
        ret.emplace_back(argv[j]);
    }
}

void info::cli::cli_parser::handle_long_opt(std::string_view arg, int argc, char** argv, int& i) {
    auto[opt, val] = impl::split(arg);

    // "--opt=val"
    if (!val.empty()) {
        auto iter = _val.find(opt);
        if (iter == _val.end()) {
            _error("Unexpected option found: " + opt +
                   (_has_help ? ". Use --help for usage." : "."));
            return;
        }
        auto&[__, fn] = iter->second;
        fn(val);
        return;
    }

    // "--opt="
    if (arg.back() == '=') {
        auto iter = _val.find(opt);
        auto&[__, fn] = iter->second;
        fn("");
        return;
    }

    // "--opt" => opt = arg
    if (auto iter = _val.find(opt);
           iter != _val.end()) {
        auto&[data, fn] = iter->second;

        if (data.allow_nothing) { // "--opt"
            fn(data.default_val);
            return;
        }

        if (i + 1 < argc) { // "--opt" "val"
            ++i;
            fn(argv[i]);
            return;
        }
        _error("Expected value after encountering option: " +
               opt + ". But found end of input." +
               (_has_help ? " Use --help for usage." : ""));
        return;
    }
    _error("Unexpected option found: " + opt +
           (_has_help ? ". Use --help for usage." : "."));
}

void info::cli::cli_parser::handle_short_opt(std::string_view arg,
                                             int argc, char** argv, int i) {
    unsigned n = 2;
    for (std::string opt{arg.substr(0, 2)};
         n <= arg.size();
         opt = '-' + std::string{arg.substr(n++, 1)}) {

        if (auto it = _val.find(opt);
               it != _val.end()) {
            auto&[data, fn] = it->second;

            if (data.allow_nothing) { // -ab -> -a
                fn(data.default_val);
                continue;
            }

            if (n == arg.size()) { // "-a" "val"
                if (i + 1 < argc) {
                    ++i;
                    fn(argv[i]);
                    break;
                }
                _error("Expected value after encountering option: " +
                       opt + ". But found end of input." +
                       (_has_help ? " Use --help for usage." : ""));
            }

            if (data.expected_type == parse_type::numeric) { // -a12b -> -a 12
                unsigned copy_num = 0;
                while (std::isdigit(arg[n + copy_num])
                       && arg.size() > copy_num) {
                    ++copy_num;
                }

                if (copy_num == 0)
                    _error("Expected numeric value for option: " + opt);

                std::string val(copy_num, '0');
                std::copy_n(std::next(arg.begin(), n), copy_num, val.begin());
                n += copy_num;

                fn(val);
                continue;
            } else { // characters are allowed
                if (data.length < 0) { // -Werror -> "-W" "error"
                    fn(arg.substr(n));
                    break;
                }
                if (arg.size() - n >= (unsigned) data.length) { // do we have enough for that?
                    fn(arg.substr(n, (unsigned) data.length)); // -aqb -> -a q -b; where q is data.length long
                    n += static_cast<unsigned>(data.length);
                    continue;
                }
                _error("Inadequate length for value of option: "
                       + opt + " with given value:" + arg.substr(n).data());
            }
        }
        _error("Unexpected option found: " + opt +
               (_has_help ? ". Use --help for usage." : "."));
    }
}

info::cli::error_reporter<> info::cli::cli_parser::_error{};

std::vector<std::string_view>
info::cli::cli_parser::operator()(int argc, char** argv) {
    std::vector<std::string_view> ret;
    _help.set_name(argv[0]);
    ret.emplace_back(argv[0]); // toss program name where I don't see it

    for (int i = 1; i < argc; ++i) {
        std::string_view arg{argv[i]};

        // filter end of options
        if (__builtin_expect(arg == "--", 0)) {
            finish_all(argc, argv, ret, i);
            return ret;
        }

        // filter empty input
        if (__builtin_expect(arg.empty(), 0)) {
            ret.emplace_back("");
            continue;
        }

        // filter non options
        if (arg[0] != '-' && arg.size() > 1) {
            ret.emplace_back(std::move(arg));
            continue;
        }

        if (arg[1] == '-') { // --<...>
            handle_long_opt(arg, argc, argv, i);
        } else { // -<...>
            handle_short_opt(arg, argc, argv, i);
        }
    }

    return ret;
}

info::cli::impl::typed_callback::typed_callback()
       : _data{impl::rt_type_data::make(boost::hana::type_c<void>)},
         _func{[](auto) {}} {}

