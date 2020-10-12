/** InfoCLI project
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Tests for parsing packed short options like -abc
 */

#include <array>
#include <string_view>
#include <vector>
using namespace std::literals;

#include <catch2/catch.hpp>

#include <info/cli/cli_parser.hxx>
#include <info/cli/option.hxx>
using namespace info::cli::udl;

TEST_CASE("cli_parser parses packed valueless boolean flags correctly",
          "[cli_parser][short_options][packed]") {
    bool a = false, b = false, c = false;
    info::cli::cli_parser cli{
           'a'_opt >>= a,
           'b'_opt >>= b,
           'c'_opt >>= c};
    auto args = std::array{"text", "-abc", "asd"};

    auto rem = cli(args.size(), const_cast<char**>(args.data()));

    CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
    CHECK(a);
    CHECK(b);
    CHECK(c);
}

TEST_CASE("cli_parser parses packed options with values correctly",
          "[cli_parser][short_options][packed]") {
    bool b = true;
    int i = 0;
    char c = '\0';
    info::cli::cli_parser cli{
           'b'_opt >>= b,
           'i'_opt >>= i,
           'c'_opt >>= c};
    auto args = std::array{"text", "-cii42b0", "asd"};

    auto rem = cli(args.size(), const_cast<char**>(args.data()));

    CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
    CHECK_FALSE(b);
    CHECK(i == 42);
    CHECK(c == 'i');
}

TEST_CASE("cli_parser parses packed options where the pack's element allows nothing",
          "[cli_parser][short_options][packed]") {
    bool b = true;
    int i = 0;
    char c = '\0';
    info::cli::cli_parser cli{
           'b'_opt >>= b,
           '.'_opt >>= i, // this is a heinous idea -.2
           'c'_opt >>= c};
    auto args = std::array{"text", "-cib.42", "asd"};

    auto rem = cli(args.size(), const_cast<char**>(args.data()));

    CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
    CHECK(b);
    CHECK(i == 42);
    CHECK(c == 'i');
}
