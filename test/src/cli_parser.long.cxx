/*
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Tests for long option parsing behavior of cli_parser
 */

#include <array>
#include <string_view>
#include <vector>
using namespace std::literals;

#include <catch2/catch.hpp>

#include <info/cli/cli_parser.hxx>
#include <info/cli/option.hxx>
using namespace info::cli::udl;

// we do not differentiate types for GNU-style long options
// as they are required to be provided like --opt=value
// therefore even bools are to be provided
// a non-provided bool is not regarded as a GNU-style option
TEST_CASE("cli_parser parses GNU-style long options",
          "[cli_parser][long][GNU]") {
    int i;
    info::cli::cli_parser cli{
           "option"_opt >= "asd" >>= i};

    auto args = std::array{"text",
                           "--option=42",
                           "asd"};
    auto rem = cli(args.size(), const_cast<char**>(args.data()));

    CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
    CHECK(i == 42);
}

TEST_CASE("defaultable non-gnu long option",
          "[cli_parser][long][non-gnu]") {
    bool b = false;
    info::cli::cli_parser cli{
           "option"_opt >>= b};
    auto args = std::array{"text",
                           "--option",
                           "asd"};

    auto rem = cli(args.size(), const_cast<char**>(args.data()));

    CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
    CHECK(b);
}

TEST_CASE("non-defaultable non-gnu long option",
          "[cli_parser][long][non-gnu]") {
    std::string str;
    info::cli::cli_parser cli{
           "option"_opt >>= str};
    auto args = std::array{"text",
                           "--option",
                           "asd"};

    auto rem = cli(args.size(), const_cast<char**>(args.data()));

    CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv}));
    CHECK(str == "asd");
}
