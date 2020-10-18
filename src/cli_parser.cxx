/** InfoCLI project
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Implements the cli_parser class's member functions
 *  => contains all the parsing logic
 */

#include <cassert>
#include <cstring>

#include <info/cli/cli_parser.hxx>
#include <info/cli/exc/bad_option_value.hxx>
#include <info/cli/exc/callback_error.hxx>
#include <info/cli/exc/no_such_option.hxx>
#include <info/lambda.hpp>

void
info::cli::cli_parser::short_option(char* arg, int argc, char** argv, int& i) {
    if (arg[0] != '\0'
        && arg[1] == '\0') {// short unpacked option 'x' (option stripped)
        return unpacked_shorts(arg, argc, argv, i);
    }
    packed_shorts(arg, argc, argv, i);
}

std::vector<std::string_view>
info::cli::cli_parser::operator()(int argc, char** argv) {
    std::vector<std::string_view> operands;
    operands.reserve(static_cast<unsigned>(argc));

    for (int i = 0; i != argc; ++i) {
        // handle long options "--"
        bool long_opt = std::memcmp("--", argv[i], sizeof("--") - 1) == 0;
        if (long_opt) {
            if (argv[i][2] == '\0') {// "--"
                for (int j = i + 1; j != argc; ++j) {
                    operands.emplace_back(argv[j]);
                }
                return operands;
            }
            long_option(argc, argv, i);
            continue;
        }

        // handle short options "-"
        bool short_opt = argv[i][0] == '-';
        if (short_opt) {
            if (argv[i][1] == '\0') {// "-"
                operands.emplace_back("-");
            } else {
                short_option(strip_option(argv[i]), argc, argv, i);
            }
            continue;
        }

        // else
        operands.emplace_back(argv[i]);
    }

    return operands;
}

char*
info::cli::cli_parser::strip_option(char* opt, bool lng) {
    char* ret = opt;

    ret += *ret == '-';
    if (lng) {
        ret += *ret == '-';
    }

    return ret;
}

info::cli::cli_parser::cli_parser(std::initializer_list<option> opts) {
    for (auto& [names, type, help, call] : opts) {
        auto [it, _] = _helps.emplace(help);
        _callbacks.emplace_back(call);
        auto pos = _callbacks.size() - 1;
        for (const auto& name : names) {
            _options.emplace(name,
                             option_info{type,
                                         pos,
                                         std::string_view{it->data(), it->size()}});
        }
    }
}

std::size_t
info::cli::cli_parser::size() const noexcept {
    return _options.size();
}

void
info::cli::cli_parser::unpacked_shorts(char* arg, int argc, char** argv, int& i) {
    const char* last = nullptr;
    std::string_view inopt{arg};

    auto it = _options.find({inopt.data(), inopt.size()});
    if (it == _options.end()) {
        throw no_such_option(arg);
    }

    auto& [data, idx, __] = it->second;
    const auto& fn = _callbacks[idx];
    assert((bool) fn);

    if (!data.allow_nothing) {
        if (i + 1 == argc) {
            throw bad_option_value(std::string(arg, 1), data.type_name, "<none given>");
        }
        ++i;

        auto str = data.length != -1 ? std::string_view(argv[i], static_cast<std::size_t>(data.length))
                                     : std::string_view(argv[i]);

        if (!fn(str, last)) {// last ignored
            throw callback_error(std::string(arg, 1), argv[i]);
        }
    } else if (INFO_CLI_UNLIKELY(!fn(data.default_val, last))) {// last ignored
        throw callback_error(std::string(arg, 1), data.default_val.data());
    }
}

void
info::cli::cli_parser::packed_shorts(char* arg, int argc, char** argv, int& i) {
    const char* last = nullptr;
    std::string_view inopt(arg);

    auto it = _options.find(std::string(1, inopt[0]));
    if (it == _options.end()) {
        throw no_such_option(arg);
    }
    auto& [data, idx, __] = it->second;
    auto& fn = _callbacks[idx];
    assert((bool) fn);

    if (auto it_ = _options.find(std::string(1, inopt[1]));
        (it_ != _options.end()) && data.allow_nothing) {
        // if the following is an option and we accept nothing we
        // do not consume it and call the callback with the default

        if (INFO_CLI_UNLIKELY(!fn(data.default_val, last))) {// last ignored
            throw callback_error(std::string(arg, 1), inopt.substr(1).data());
        }

        short_option(arg + 1, argc, argv, i);
    } else if (parse_type_accepts(data.expected_type)(inopt[1])) {
        // munch from input
        if (!fn(inopt.substr(1, static_cast<std::size_t>(data.length)), last)) {
            throw callback_error(std::string(arg, 1), inopt.substr(1).data());
        }

        if (last == inopt.data() + inopt.size()) {// the end is here
            return;
        }

        short_option(const_cast<char*>(last), argc, argv, i);
    } else if (data.allow_nothing) {
        if (INFO_CLI_UNLIKELY(!fn(data.default_val, last))) {// last ignored
            throw callback_error(std::string(arg, 1), data.default_val.data());
        }
    } else {
        throw bad_option_value(std::string(arg, 1), data.type_name, "<none given>");
    }
}

void
info::cli::cli_parser::long_option(int argc, char** argv, int& i) {
    std::string_view inopt{strip_option(argv[i], true)};
    const char* last = nullptr;

    if (auto pos = std::find(inopt.begin(), inopt.end(), '=');
        pos != inopt.end()) {// GNU-style long options
        auto dist = static_cast<std::size_t>(std::distance(inopt.begin(), pos));
        auto opt = inopt.substr(0, dist);
        auto val = inopt.substr(dist + 1);

        auto it = _options.find(std::string(opt.data(), opt.size()));
        if (it == _options.end()) {
            throw no_such_option(strip_option(const_cast<char*>(opt.data()), true));
        }
        auto& [_, idx, __] = it->second;
        auto& fn = _callbacks[idx];
        assert((bool) fn);

        if (!fn(val, last)) {// last ignored
            throw callback_error(strip_option(const_cast<char*>(opt.data()), true), val.data());
        }
        return;
    }

    auto it = _options.find(std::string(inopt.data(), inopt.size()));
    if (it == _options.end()) {
        throw no_such_option(strip_option(const_cast<char*>(inopt.data()), true));
    }
    auto& [data, idx, __] = it->second;
    auto& fn = _callbacks[idx];
    assert((bool) fn);

    if (!data.allow_nothing) {
        if (i + 1 == argc) {
            throw bad_option_value(strip_option(argv[i], true), data.type_name, "<none given>");
        }
        ++i;

        auto str = data.length != -1 ? std::string_view(argv[i], static_cast<std::size_t>(data.length))
                                     : std::string_view(argv[i]);

        if (!fn(str, last)) {// last ignored
            throw callback_error(argv[i - 1], argv[i]);
        }
    } else if (INFO_CLI_UNLIKELY(!fn(data.default_val, last))) {// last ignored
        throw callback_error(argv[i], data.default_val.data());
    }
}
