/*
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Implements the type_parser<>-s provided as part of the library
 */

#include <info/cli/types/type_parser.hxx>

#include <cassert>

info::expected<std::string, info::cli::parser_opcode>
info::cli::type_parser<std::string>::operator()(std::string_view str,
                                                const char*& last) const noexcept {
    last = str.data() + str.size();
    return std::string(str.data(), str.size());
}

info::expected<std::string_view, info::cli::parser_opcode>
info::cli::type_parser<std::string_view>::operator()(std::string_view str,
                                                     const char*& last) const noexcept {
    last = str.data() + str.size();
    return std::string_view(str.data(), str.size());
}

#define INTEGRAL_PARSER_IMPL(T)                                               \
    info::expected<T, info::cli::parser_opcode>                               \
    info::cli::type_parser<T>::operator()(std::string_view str,               \
                                          const char*& last) const noexcept { \
        assert(!str.empty());                                                 \
                                                                              \
        T i;                                                                  \
        auto [p, ex] = std::from_chars(str.data(),                            \
                                       str.data() + str.size(),               \
                                       i);                                    \
        last = p;                                                             \
                                                                              \
        if (ex == std::errc{})                                                \
            return i;                                                         \
                                                                              \
        return INFO_UNEXPECTED{parser_opcode::terminate};                     \
    }


INTEGRAL_PARSER_IMPL(short)
INTEGRAL_PARSER_IMPL(unsigned short)
INTEGRAL_PARSER_IMPL(int)
INTEGRAL_PARSER_IMPL(unsigned)
INTEGRAL_PARSER_IMPL(long)
INTEGRAL_PARSER_IMPL(unsigned long)
INTEGRAL_PARSER_IMPL(long long)
INTEGRAL_PARSER_IMPL(long unsigned long)

info::expected<char, info::cli::parser_opcode>
info::cli::type_parser<char>::operator()(std::string_view str,
                                         const char*& last) const noexcept {
    assert(str.size() == 1);

    last = str.data() + 1;
    return str[0];
}

info::expected<unsigned char, info::cli::parser_opcode>
info::cli::type_parser<unsigned char>::operator()(std::string_view str,
                                                  const char*& last) const noexcept {
    assert(str.size() == 1);

    last = str.data() + 1;
    return static_cast<unsigned char>(str[0]);
}

// ah, yes copy-paste oriented programming
info::expected<float, info::cli::parser_opcode>
info::cli::type_parser<float>::operator()(std::string_view str,
                                          const char*& last) const noexcept {
    assert(!str.empty());

    auto val = std::strtof(str.data(), const_cast<char**>(&last));
    if (str.data() == last)// couldn't parse shit
        return INFO_UNEXPECTED{parser_opcode::terminate};
    return val;
}

info::expected<double, info::cli::parser_opcode>
info::cli::type_parser<double>::operator()(std::string_view str,
                                           const char*& last) const noexcept {
    assert(!str.empty());

    auto val = std::strtod(str.data(), const_cast<char**>(&last));
    if (str.data() == last)// couldn't parse shit
        return INFO_UNEXPECTED{parser_opcode::terminate};
    return val;
}

info::expected<long double, info::cli::parser_opcode>
info::cli::type_parser<long double>::operator()(std::string_view str,
                                                const char*& last) const noexcept {
    assert(!str.empty());

    auto val = std::strtold(str.data(), const_cast<char**>(&last));
    if (str.data() == last)// couldn't parse shit
        return INFO_UNEXPECTED{parser_opcode::terminate};
    return val;
}

info::expected<bool, info::cli::parser_opcode>
info::cli::type_parser<bool>::operator()(std::string_view str,
                                         const char*& last) const noexcept {
    if (str.empty())
        return false;

    std::string lc{str};
    std::transform(lc.begin(), lc.end(), lc.begin(), [](auto ch) {
        return static_cast<char>(::tolower(ch));
    });
    last = str.data() + str.size();
    return !(str == "false"
             || str == "off"
             || str == "0");
}
