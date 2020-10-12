/** InfoCLI project
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Tests for generic option-style independent behavior of cli_parser
 */

#include <array>
#include <string_view>
#include <vector>
using namespace std::literals;

#include <catch2/catch.hpp>

#include <info/cli/cli_parser.hxx>
#include <info/cli/option.hxx>
using namespace info::cli::udl;

TEST_CASE("cli_parser takes init-list of options",
          "[cli_parser][construction]") {
    int i;
    info::cli::cli_parser cli{
           'W' / 'f'_opt >= "asd" >>= i,
           'x'_opt >>= i//
    };

    INFO("Constructed properly");
    CHECK(true);
}

TEST_CASE("cli_parser creates correct amount of actual options from the DSL",
          "[cli_parser][construction][dsl]") {
    int i, j, k;

    SECTION("when options are simplex") {
        info::cli::cli_parser cli{
               'a'_opt >>= i,
               'b'_opt >>= j,
               'c'_opt >>= k//
        };

        CHECK(cli.size() == 3);
    }

    SECTION("when options are multiplex") {
        info::cli::cli_parser cli{
               'a'_opt / "option-a" >>= i,
               'b'_opt / "option-b" >>= j,
               'c'_opt / "option-c" >>= k//
        };

        CHECK(cli.size() == 6);
    }
}

TEST_CASE("aggregating types work properly",
          "[cli_parser][aggregating]") {
    std::vector<std::string> agg;

    info::cli::cli_parser cli{
           'o'_opt >>= agg};
    auto args = std::array{"text",
                           "-othis",
                           "-ois",
                           "-ofancy",
                           "asd"};

    auto rem = cli(args.size(), const_cast<char**>(args.data()));

    CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
    CHECK_THAT(agg, Catch::Equals(std::vector{"this"s, "is"s, "fancy"s}));
}

TEST_CASE("functor callbacks work with non-string types",
          "[cli_parser][functor][non-string]") {
    int i = 0;
    info::cli::cli_parser cli{
           'o'_opt >>= [&i](int x) {
               i = x;
           }};
    auto args = std::array{"text",
                           "-o3",
                           "asd"};

    auto rem = cli(args.size(), const_cast<char**>(args.data()));

    CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
    CHECK(i == 3);
}

TEST_CASE("functor callbacks work with string types",
          "[cli_parser][functor][non-string]") {
    std::string str;
    info::cli::cli_parser cli{
           's'_opt >>= [&str](std::string_view x) {
               str = x;
           }};
    auto args = std::array{"text",
                           "-safety",
                           "asd"};

    auto rem = cli(args.size(), const_cast<char**>(args.data()));

    CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
    CHECK(str == "afety");
}

TEST_CASE("repeat callback values allow incrementing valueless options",
          "[cli_parser][value_callback][repeat]") {
    int i = 0;
    info::cli::cli_parser cli{
           'v'_opt >>= info::cli::repeat{i}};
    auto args = std::array{"-v",
                           "-vvv"};

    auto rem = cli(args.size(), const_cast<char**>(args.data()));

    CHECK(i == 4);
}

TEST_CASE("cli_parser handles - correctly",
          "[cli_parser][meta_options]") {
    int i = 0;
    info::cli::cli_parser cli{
           'i'_opt >>= i};
    auto args = std::array{"text",
                           "-i3",
                           "asd",
                           "-",
                           "-i2",
                           "asd"};


    auto rem = cli(args.size(), const_cast<char**>(args.data()));

    CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv, "-"sv, "asd"sv}));
    CHECK(i == 2);
}

TEST_CASE("cli_parser handles -- correctly",
          "[cli_parser][meta_options]") {
    int i = 0;
    info::cli::cli_parser cli{
           'i'_opt >>= i};
    auto args = std::array{"text",
                           "-i3",
                           "asd",
                           "--",
                           "-i2",
                           "asd"};


    auto rem = cli(args.size(), const_cast<char**>(args.data()));

    CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv, "-i2"sv, "asd"sv}));
    CHECK(i == 3);
}
