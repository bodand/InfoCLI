/** InfoCLI project
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Defines the struct for an option that is used to define cli options.
 * Fancy UDL included.
 */
#pragma once

#include <deque>
#include <functional>
#include <string>
#include <type_traits>
#include <vector>

#include <info/cli/aggregator.hxx>
#include <info/cli/meta/dissector.hxx>
#include <info/cli/types/type_data.hxx>
#include <info/cli/types/type_modifier.hxx>
#include <info/cli/types/type_parser.hxx>

namespace info::_cli {
    struct INFO_CLI_API option_builder;
    struct INFO_CLI_API helpful_option_builder;
}

namespace info::cli {
    struct INFO_CLI_API option {
        option(std::string help,
               std::function<bool(std::string_view, const char*&)> callback,
               std::vector<std::string>&& names,
               rt_type_data type);
        option(std::function<bool(std::string_view, const char*&)> callback,
               std::vector<std::string>&& names,
               rt_type_data type);

        std::vector<std::string> names;
        rt_type_data type;
        std::string help;
        std::function<bool(std::string_view, const char*&)> callback;
    };

    inline namespace udl {
        INFO_CLI_API
        _cli::option_builder operator""_opt(const char* str, std::size_t size);
        INFO_CLI_API
        _cli::option_builder operator""_opt(char ch);
    }
}

namespace info::_cli {
    /**
     * \brief An option-builder that has a help message an now only needs
     * the callback to finish the option.
     *
     * An option builder that has stepped up from the name defining part,
     * and also has a help-text. The only way from here is the callback with >>=.
     */
    struct INFO_CLI_API helpful_option_builder {
        explicit helpful_option_builder(const option_builder& bld);
        helpful_option_builder(std::string_view help, const option_builder& bld);

        template<class T,// clang-format off
                 typename std::enable_if<
                    !cli::meta::is_typed_callback<std::decay_t<T>>
                    && !std::is_invocable_v<T, std::string_view>
                 >::type* = nullptr
        >// clang-format on
        cli::option
        operator>>=(T&& ref) {// Reference to variable
            // The type without reference stuffs
            using DecayedType = std::decay_t<T>;
            // The type to which we have a reference to; usually DecayedType, but
            // modifiers may change it to something else
            // eg. DecayedType = repeat<T> => ReferencedType = T
            using ReferencedType = cli::meta::referenced_type<DecayedType>;
            // The type that's to be used for the type_data. In most cases
            // it is ParsedType, except for modifiers where it is DecayedType,
            // because type_data tracks changes with modifiers, so it needs to
            // know about them
            using ExpectedType = cli::meta::expected_type<DecayedType>;
            // The type to parse from a string. This changes for stdlib containers
            // and modifiers for example, otherwise is just DecayedType
            using ParsedType = aggregator_type<DecayedType>;

            auto& rf = [&ref]() -> ReferencedType& {
                if constexpr (cli::type_modifier<T>) {
                    return ref;
                } else {
                    return ref.value;
                }
            }();

            return {help, [&rf](std::string_view str, const char*& last) {
                        auto val = cli::type_parser<ParsedType>{}(str, last);
                        if (!val) {
                            return val.error() == cli::parser_opcode::ignore;
                        }

                        if constexpr (auto agg = _cli::aggregator<DecayedType>) {
                            if constexpr (std::is_move_constructible_v<ReferencedType>) {
                                agg(rf, std::move(*val));
                            } else {
                                agg(rf, *val);
                            }
                        } else {
                            if constexpr (std::is_move_assignable_v<ReferencedType>) {
                                rf = std::move(*val);
                            } else {
                                rf = *val;
                            }
                        }
                        return true;
                    },
                    std::move(names),
                    cli::rt_type_data(cli::type_data<ExpectedType>{})};
        }

        template<class Fn,// clang-format off
                         typename std::enable_if<
                             cli::meta::is_typed_callback<std::decay_t<Fn>>
                             && !std::is_invocable_v<Fn, std::string_view>
                        >::type* = nullptr
                >// clang-format on
        cli::option
        operator>>=(Fn&& fun) {// callback with non-string type
            using T = cli::meta::head<cli::meta::dissect<Fn>>;
            using typ = aggregator_type<T>;

            return {help, [fn = std::forward<Fn>(fun)](std::string_view str, const char*& last) -> bool {
                        auto val = cli::type_parser<typ>{}(str, last);
                        if (!val) {
                            return val.error() == cli::parser_opcode::ignore;
                        }

                        if constexpr (auto agg = _cli::aggregator<T>) {
                            T ref;
                            if constexpr (std::is_move_constructible_v<typ>) {
                                agg(ref, std::move(*val));
                            } else {
                                agg(ref, *val);
                            }
                            fn(ref);
                        } else {
                            if constexpr (std::is_move_constructible_v<T>) {
                                fn(std::move(*val));
                            } else {
                                fn(*val);
                            }
                        }
                        return true;
                    },
                    std::move(names),
                    cli::rt_type_data(cli::type_data<T>())};
        }

        template<class Fn,// clang-format off
                         typename std::enable_if<
                             cli::meta::is_typed_callback<std::decay_t<Fn>>
                             && std::is_invocable_v<Fn, std::string_view>
                        >::type* = nullptr
                >// clang-format on
        cli::option
        operator>>=(Fn&& fun) {// callback with string type
            using ret = cli::meta::head<cli::meta::dissect<Fn>>;

            return {help, [fn = std::forward<Fn>(fun)](std::string_view str, const char*& last) -> bool {
                        auto val = cli::type_parser<ret>{}(str, last);
                        if (INFO_CLI_UNLIKELY(!val)) {
                            return val.error() == cli::parser_opcode::ignore;
                        }

                        fn(std::move(*val));
                        return true;
                    },
                    std::move(names),
                    cli::rt_type_data(cli::type_data<ret>())};
        }

        std::string help;
        std::vector<std::string> names;
    };

    /**
     * \brief Builder object used by the DSL to create options.
     *
     * A builder used to create options either directly or taking a detour
     * and adding help with a helpful_option_builder.
     *
     * This shows how the ways the DSL can be used to create an option, from
     * an option_builder.
     * \verbatim
     * option_builder -> helpful_option_builder -> option
     *                `--->------->-------->----´
     * \endverbatim
     */
    struct INFO_CLI_API option_builder {
        option_builder(std::string_view name);
        helpful_option_builder operator>=(std::string_view help);

        template<class Callback>
        cli::option
        operator>>=(Callback&& callback) {
            // delegate all the work to the helpful
            return (helpful_option_builder(*this)).operator>>=(std::forward<Callback>(callback));
        }

        // deque: need access to push_front
        std::deque<std::string> names;
    };

    INFO_CLI_API option_builder operator/(std::string_view name, option_builder bld);
    INFO_CLI_API option_builder operator/(char name, option_builder bld);
    INFO_CLI_API option_builder operator/(option_builder bld, std::string_view name);
    INFO_CLI_API option_builder operator/(option_builder bld, char name);
    INFO_CLI_API option_builder operator/(option_builder bld1, const option_builder& bld2);
}
