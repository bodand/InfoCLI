/** InfoCLI project
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Tests for parsing unpacked short options with possibly value concated at their
 * ends
 */

#include <array>
#include <string_view>
#include <vector>
using namespace std::literals;

#include <catch2/catch.hpp>

#include <info/cli/cli_parser.hxx>
#include <info/cli/option.hxx>
using namespace info::cli::udl;

TEST_CASE("boolean flag (unbounded, printable, default) parses correctly",
          "[cli_parser][short_options][unpacked]") {
    bool b;
    info::cli::cli_parser cli{
           'b'_opt >= "Some fancy help for option -b" >>= b};

    SECTION("without value") {
        b = false;
        auto args = std::array{"text", "-b", "asd"};

        auto rem = cli(args.size(), const_cast<char**>(args.data()));

        CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
        CHECK(b);
    }

    SECTION("with value") {
        SECTION("numeric true 42") {
            b = false;
            auto args = std::array{"text", "-b42", "asd"};

            auto rem = cli(args.size(), const_cast<char**>(args.data()));

            CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
            CHECK(b);
        }

        SECTION("numeric false 0") {
            b = true;
            auto args = std::array{"text", "-b0", "asd"};

            auto rem = cli(args.size(), const_cast<char**>(args.data()));

            CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
            CHECK_FALSE(b);
        }

        SECTION("string true asd") {
            b = false;
            auto args = std::array{"text",
                                   "-basd",
                                   "WANTED 'asd'. Illegally leaving post: $.1"};

            auto rem = cli(args.size(), const_cast<char**>(args.data()));

            CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv,//
                                                      "WANTED 'asd'. Illegally leaving post: $.1"sv}));
            CHECK(b);
        }

        SECTION("string false off") {
            b = true;
            auto args = std::array{"text", "-boff", "asd"};

            auto rem = cli(args.size(), const_cast<char**>(args.data()));

            CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
            CHECK_FALSE(b);
        }

        SECTION("string false false") {
            b = true;
            auto args = std::array{"text", "-bfalse", "asd"};

            auto rem = cli(args.size(), const_cast<char**>(args.data()));

            CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
            CHECK_FALSE(b);
        }
    }
}

TEST_CASE("character option (len 1, printable, no-default) parses correctly",
          "[cli_parser][short_options][unpacked]") {
    char c;
    info::cli::cli_parser cli{
           'c'_opt >= "Some other fancy help for option -c" >>= c};

    SECTION("-c with value concated to it") {
        c = '\0';
        auto args = std::array{"text",
                               "-cf",
                               "asd"};

        auto rem = cli(args.size(), const_cast<char**>(args.data()));

        CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
        CHECK(c == 'f');
    }

    SECTION("-c with value following it") {
        c = '\0';
        auto args = std::array{"text",
                               "-c",
                               "f",
                               "asd"};

        auto rem = cli(args.size(), const_cast<char**>(args.data()));

        CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
        CHECK(c == 'f');
    }

    SECTION("-c with string after it") {
        c = '\0';
        auto args = std::array{"text",
                               "-c",
                               "fling",// ling is dropped
                               "asd"};

        auto rem = cli(args.size(), const_cast<char**>(args.data()));

        CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
        CHECK(c == 'f');
    }
}

TEST_CASE("integer option (unbounded, digit, no-default) parses correctly",
          "[cli_parser][short_options][unpacked]") {
    int i;
    info::cli::cli_parser cli{
           'i'_opt >= "Somewhat fancy help for option -i" >>= i};

    SECTION("-i with single digit concated to it") {
        i = 0;
        auto args = std::array{"text",
                               "-i9",
                               "asd"};

        auto rem = cli(args.size(), const_cast<char**>(args.data()));

        CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
        CHECK(i == 9);
    }

    SECTION("-i with digits concated to it") {
        i = 0;
        auto args = std::array{"text",
                               "-i42",
                               "asd"};

        auto rem = cli(args.size(), const_cast<char**>(args.data()));

        CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
        CHECK(i == 42);
    }

    SECTION("-i with value following it") {
        i = 0;
        auto args = std::array{"text",
                               "-i",
                               "56",
                               "asd"};

        auto rem = cli(args.size(), const_cast<char**>(args.data()));

        CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
        CHECK(i == 56);
    }

    SECTION("-i with string after it") {
        i = 0;
        auto args = std::array{"text",
                               "-i",
                               "65km",// km
                               "asd"};

        auto rem = cli(args.size(), const_cast<char**>(args.data()));

        CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
        CHECK(i == 65);
    }
}

TEST_CASE("string option (unbounded, printable, no-default) parses correctly",
          "[cli_parser][short_options][unpacked]") {
    std::string str;
    info::cli::cli_parser cli{
           's'_opt >= "Rather unhelpful help for option -s" >>= str};

    SECTION("-s with value concated to it") {
        str = "";
        auto args = std::array{"text",
                               "-safety",
                               "asd"};

        auto rem = cli(args.size(), const_cast<char**>(args.data()));

        CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
        CHECK(str == "afety");
    }

    SECTION("-s with value following it") {
        str = "";
        auto args = std::array{"text",
                               "-s",
                               "safety",
                               "asd"};

        auto rem = cli(args.size(), const_cast<char**>(args.data()));

        CHECK_THAT(rem, Catch::Equals(std::vector{"text"sv, "asd"sv}));
        CHECK(str == "safety");
    }
}
