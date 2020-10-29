/*
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Tests for generic option-style independent behavior of cli_parser
 */

#include <algorithm>
#include <array>
#include <numeric>
#include <string_view>
#include <vector>
using namespace std::literals;

#include <catch2/catch.hpp>

#include <info/cli/cli_parser.hxx>
#include <info/cli/exc/bad_option_value.hxx>
#include <info/cli/exc/no_such_option.hxx>
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

#define EXCEPTION_MESSAGE_MATCHES_STRINGS(ExT, ...)                   \
    Catch::Predicate<ExT>([](const ExT& ex) {                         \
        std::string msg = ex.what();                                  \
        std::vector<std::string> exps{__VA_ARGS__};                   \
        auto res = new bool[exps.size()];                             \
                                                                      \
        std::transform(exps.begin(), exps.end(), res, [&](auto str) { \
            return msg.end() != std::search(msg.begin(), /**/         \
                                            msg.end(),                \
                                            str.begin(),              \
                                            str.end());               \
        });                                                           \
        bool ret = std::accumulate(res,                               \
                                   res + exps.size(),                 \
                                   true,                              \
                                   [](auto val, auto x) {             \
                                       return val && x;               \
                                   });                                \
        delete[] res;                                                 \
        return ret;                                                   \
    })

TEST_CASE("cli_parser throws if an option was not provided with required value",
          "[cli_parser][error][bad_option_value]") {
    int i = 0;
    info::cli::cli_parser cli{
           'i'_opt / "long-i" >>= i};

    SECTION("unpacked options") {
        auto args = std::array{"-i"};
        CHECK_THROWS_MATCHES(cli(args.size(), const_cast<char**>(args.data())),
                             info::cli::bad_option_value,
                             EXCEPTION_MESSAGE_MATCHES_STRINGS(info::cli::bad_option_value,
                                                               "error: unintelligible value",
                                                               "option 'i' ",
                                                               "<none given>"));
    }

    SECTION("packed options") {
        auto args = std::array{"-i."};// packed-like options, rather
        CHECK_THROWS_MATCHES(cli(args.size(), const_cast<char**>(args.data())),
                             info::cli::bad_option_value,
                             EXCEPTION_MESSAGE_MATCHES_STRINGS(info::cli::bad_option_value,
                                                               "error: unintelligible value",
                                                               "option 'i' ",
                                                               "<none given>"));
    }

    SECTION("long options") {
        auto args = std::array{"--long-i"};
        CHECK_THROWS_MATCHES(cli(args.size(), const_cast<char**>(args.data())),
                             info::cli::bad_option_value,
                             EXCEPTION_MESSAGE_MATCHES_STRINGS(info::cli::bad_option_value,
                                                               "error: unintelligible value",
                                                               "option 'long-i' ",
                                                               "<none given>"));
    }
}

TEST_CASE("cli_parser throws if option is not defined",
          "[cli_parser][error][no_such_option]") {
    info::cli::cli_parser cli{};

    SECTION("unpacked options") {
        auto args = std::array{"-i"};
        CHECK_THROWS_MATCHES(cli(args.size(), const_cast<char**>(args.data())),
                             info::cli::no_such_option,
                             EXCEPTION_MESSAGE_MATCHES_STRINGS(info::cli::no_such_option,
                                                               "error: unknown option",
                                                               "line: 'i'"));
    }

    SECTION("packed options") {
        auto args = std::array{"-icup"};
        CHECK_THROWS_MATCHES(cli(args.size(), const_cast<char**>(args.data())),
                             info::cli::no_such_option,
                             EXCEPTION_MESSAGE_MATCHES_STRINGS(info::cli::no_such_option,
                                                               "error: unknown option",
                                                               "line: 'icup'"));
    }

    SECTION("long options") {
        auto args = std::array{"--methamphetamine"};
        CHECK_THROWS_MATCHES(cli(args.size(), const_cast<char**>(args.data())),
                             info::cli::no_such_option,
                             EXCEPTION_MESSAGE_MATCHES_STRINGS(info::cli::no_such_option,
                                                               "error: unknown option",
                                                               "line: 'methamphetamine'"));
    }

    SECTION("GNU-style long options") {
        auto args = std::array{"--tree=arbor"};
        CHECK_THROWS_MATCHES(cli(args.size(), const_cast<char**>(args.data())),
                             info::cli::no_such_option,
                             EXCEPTION_MESSAGE_MATCHES_STRINGS(info::cli::no_such_option,
                                                               "error: unknown option",
                                                               "line: 'tree=arbor'"));
    }
}
