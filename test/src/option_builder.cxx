/*
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Tests for the different stages of the option building DSL
 */

#include <catch2/catch.hpp>

#include <info/cli/option.hxx>
using namespace info::cli::udl;
using namespace std::literals;

TEST_CASE("_opt creates an object that contains the name given to it",
          "[dsl][option]") {
    std::vector<std::string> exp{"fancy-option"};

    auto opt = "fancy-option"_opt;

    CHECK(std::equal(opt.names.begin(), opt.names.end(), exp.begin()));
}

TEST_CASE("_opt creates an object that contains the name given to it with chars",
          "[dsl][option]") {
    std::vector<std::string> exp{"f"};

    auto opt = 'f'_opt;

    CHECK(std::equal(opt.names.begin(), opt.names.end(), exp.begin()));
}

TEST_CASE("step 0 builder can be prepended with slashes",
          "[dsl][option]") {
    std::vector<std::string> exp{"f", "fancy-option"};

    auto opt = 'f' / "fancy-option"_opt;

    CHECK(std::equal(opt.names.begin(), opt.names.end(), exp.begin()));
}

TEST_CASE("step 0 builder can be appended with slashes",
          "[dsl][option]") {
    auto opt = 'f'_opt / "fancy-option" / 'F';

    CHECK_THAT(std::vector(opt.names.begin(), opt.names.end()),
               Catch::Equals(std::vector{"f"s, "fancy-option"s, "F"s}));
}

TEST_CASE("step 0 builders can be joined",
          "[dsl][option]") {
    std::vector<std::string> exp{"f", "fancy-option", "fop", "fo"};

    auto opt0 = "f"_opt / "fancy-option";
    auto opt1 = "fop" / "fo"_opt;
    auto opt = opt0 / opt1;

    CHECK(std::equal(opt.names.begin(), opt.names.end(), exp.begin()));
}

TEST_CASE("Help text can be added to get to step 1",
          "[dsl][option]") {
    auto opt = 'h' / "help"_opt >= "help text";

    CHECK(opt.help == "help text");
}

TEST_CASE("Callback reference can be added to builder",
          "[dsl][option]") {
    int i{};

    SECTION("from step 0 builder") {
        auto opt = "verbose"_opt >>= i;

        CHECK("Compiled");
    }

    SECTION("from step 1 builder") {
        auto opt = "verbose"_opt >= "Sets the verbosity level" >>= i;

        CHECK("Compiled");
    }
}
//
//TEST_CASE("Callback lambda can be added to builder",
//          "[dsl][option]") {
//
//    SECTION("from step 0 builder") {
//        int i{};
//        auto opt = "verbose"_opt >>= [&i](std::string_view) {
//            ++i;
//        };
//
//        CHECK("Compiled");
//    }
//
//    SECTION("from step 1 builder") {
//        int i{};
//        auto opt = "verbose"_opt >= "Sets the verbosity level" >>= [&i](std::string_view) {
//            ++i;
//        };
//
//        CHECK("Compiled");
//    }
//}
