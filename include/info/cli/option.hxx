/*
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

/**
 * \brief Implementation details of the InfoCLI library
 *
 * The subsidiary namespace making up several implementation
 * details of InfoCLI. Everything contained in this namespace,
 * except aggregator_, is not considered part of the API and may change
 * massively between patch versions. Relying on it is dicing with death.
 */
namespace info::_cli {
    /// \cond DOXYGEN_IGNORE_THIS
    // doxygen parsed these as variable declarations??
    struct INFO_CLI_API option_builder;
    struct INFO_CLI_API helpful_option_builder;
    /// \endcond
}

namespace info::cli {
    /**
     * \brief The struct used do define options in the DSL.
     *
     * The basic building block of the DSL. This is passed to the cli_parser to
     * create the whole command line interface for the program, along with its
     * kin.
     *
     * \warning
     * This should not usually be constructed by hand, but using the DSL and the
     * two builder types: _cli::option_builder, and _cli::helpful_option_builder.
     * The constructor is not provided with any convenience overloads for manual
     * construction, and piecing together the required elements would make the
     * whole thing look really verbose, so just use the DSL.
     *
     * The type itself is almost just a POD, difference is that it has a constructor.
     * The singular reason this exists is because using a tuple instead would be
     * really annoying, and we somehow need to pass structured data to the
     * cli_parser constructor and bundle values that form each option together.
     */
    struct INFO_CLI_API option {
        /**
         * \brief Type of the callback used for options
         *
         * A function type which is expected when dealing with the callbacks
         * for when an option is encountered.
         */
        using callback_type = bool(std::string_view, const char*&);

        /**
         * \brief Creates an option object which defines an cli option
         *
         * Constructs a given option object which defines uniquely a option accepted
         * on the command line.
         * The vector of names (the name and aliases) are moved from the given
         * vector, to avoid unnecessary copies of strings.
         *
         * \param help The description of the option used by Auto-Help. May be the empty string.
         * \param callback A function of callback_type which is used to perform the callback
         * \param names The name and the aliases of the option. The first value is considered the name
         * \param type The InfoRTTI type info for the type associated with the created option
         */
        option(std::string help,
               std::function<callback_type> callback,
               std::vector<std::string>&& names,
               rt_type_data type);

        std::vector<std::string> names;       ///< The name and the aliases for this option
        rt_type_data type;                    ///< InfoRTTI type info for the type associated with this option
        std::string help;                     ///< The Auto-Help description
        std::function<callback_type> callback;///< The callback used when the option is encountered
    };

    /**
     * \brief Namespace for the user defined literals of InfoCLI
     *
     * A namespace that can be `using namespace`'d to pull all user defined
     * literals defined by InfoCLI into the global scope for use.
     *
     * InfoCLI UDLs are used by the option definition DSL to ease the creation
     * of cli::option objects to pass the cli_parser constructor.
     */
    inline namespace udl {
        /**
         * \brief Creates an option_builder object from the name given by
         *        the literal string
         *
         * Creates a new option_builder object by taking the literal string
         * and constructs the option_builder with it.
         *
         * \param str The literal string for the name of the object
         * \param size The size of the string
         *
         * \return The created option_builder object for following use in the DSL
         */
        INFO_CLI_API INFO_CLI_PURE
               _cli::option_builder
               operator""_opt(const char* str, std::size_t size);

        /**
         * \brief Creates an option_builder with the name of a single character
         *        aka a short option
         *
         * Takes the character literal and creates an option_builder for it,
         * where the option will be considered a short option.
         *
         * \note
         * If you plan on being strictly POSIX compliant, using options created
         * like this will help you immensely, as POSIX only acknowledges options
         * of one character length.
         *
         * \param ch The character literal to create the option as
         *
         * \return The option_builder used to further construct the option
         */
        INFO_CLI_API INFO_CLI_CONST
               _cli::option_builder
               operator""_opt(char ch);
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
        /**
         * \brief Constructs a helpful_option_builder by advancing the option_builder
         *         without a help description
         *
         * An option_builder can advance to the next step of option creation even
         * if the user didn't provide a help description. In this case, this
         * constructor is called, the option_builder's members are used to
         * construct what we know, and the help is left empty.
         *
         * \attention
         * Implementors! This constructor is used to allow the option_builder to
         * seamlessly delegate the implementation of the operator>>= member function.
         *
         * \param bld The option_builder to advance to the next step without a help
         *             description.
         */
        explicit helpful_option_builder(const option_builder& bld);
        /**
         * \brief Takes the names from the option_builder and advances the build
         *         procedure with the help description.
         *
         * If during the usage of the DSL, the user provides a help description
         * the option_builder is advanced to the next step using this constructor
         * creating the helpful_option_builder which contains the previous
         * builder's information with the additional help description.
         *
         * \param help The help description for the built option
         * \param bld The previous step's builder, which contains the option's name and aliases
         */
        helpful_option_builder(std::string_view help, const option_builder& bld);

        /**
         * \brief Creates the option from the builder by defining the callback
         *
         * The callback defining function, when the callback is a simple variable
         * reference. In this case the function will perform an assignment
         * to the reference returned by type_modifier by the means described
         * by the aggregator_ instance for the given type, or if it cannot
         * be aggregated, by the operator= of that type.
         *
         * Type modifiers may be used given to this callback, and they are handled
         * as described by an appropriate type_modifier and aggregator_ combination.
         *
         * \tparam T The type of the callback
         * \param ref A forwarding reference to the callback value. If this is
         *             an rvalue reference, the value shall be a type modifier
         *             wrapping the actual callback value.
         * \return The option constructed by the stored name, aliases, help, and
         *          the newly provided callback.
         */
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

            auto& rf = cli::type_modifier_<T>{}(ref);

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

        /**
         * \copybrief operator>>=(T&&)
         *
         * The universal functor callback handling function. If the type of
         * the parameter of the given functor is not a string-like type, this
         * function figures out how to handle the situation.
         *
         * First, the created lambda, calls the type_parser of the expected type.
         * If this succeeded, we check if type is aggregating or not:
         * if it is we create a default constructed object of that type, then
         * aggregate the parsed value into it, then call the functor with this
         * object; otherwise, we just call the functor with the value
         * we got from the parser.
         *
         * \tparam Fn The type of the functor
         * \param fun The functor, perfectly forwarded into the callback handling
         *             lambda, given to the created object.
         * \return The option constructed by the stored name, aliases, help, and
         *          the newly provided callback.
         */
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

        /**
         * \copybrief operator>>=(T&&)
         *
         * A specialized callback function, is where the functor takes a string-like
         * object as a parameter, ie. it takes an \c std::string or \c std::string_view
         * In this case, we can circumvent the complex templating machinery and scaffolding
         * and allow us to not even call the type_parser, or let the aggregator_ do
         * our bidding, because we can simply pass the string we found on the command
         * line, as is.
         *
         * Since it may be useful for the users, the library provides specializations
         * for standard string-like type of type_parser, but we can simply inline
         * it manually, since there is literally no chance they fail, therefore,
         * the whole error handling mechanism is also useless.
         *
         * \tparam Fn A functor type taking a string-like parameter,
         *             which thus can be called with a \c std::string_view
         * \param fun The functor, perfectly forwarded into the callback
         *             lambda given to the option instance
         * \return The option constructed by the stored name, aliases, help, and
         *          the newly provided callback.
         */
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
                        last = str.data() + str.size();
                        fn(str);
                        return true;
                    },
                    std::move(names),
                    cli::rt_type_data(cli::type_data<ret>())};
        }

        std::string help;              ///< The help description for Auto-Help. May be the empty string
        std::vector<std::string> names;///< The name and aliases of the created option
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
       option_builder -> helpful_option_builder -> option
                      `--->------->-------->----´
       \endverbatim
     */
    struct INFO_CLI_API option_builder {
        /**
         * \brief Constructs an option_builder from a name to an option
         *
         * Takes a name for the option which is then started building by the
         * constructed object.
         *
         * \param name The name of the option which is now being built
         */
        option_builder(std::string_view name);

        /**
         * \brief Adds help description to the option being built
         *
         * Adds the help description to the currently built option. After
         * this step no aliases can be added to the option.
         *
         * The description used here will be used by Auto-Help to generate the
         * help option.
         *
         * \param help The description to use in the help option generated
         *              by Auto-Help
         *
         * \return An object that is used to create the finished option
         */
        helpful_option_builder operator>=(std::string_view help);

        /**
         * \brief Create the option object without help description
         *
         * This operator creates the finished option object without a help
         * description. In this case, the option will not be documented
         * by Auto-Help, and if none of the options have a description Auto-Help
         * will not generate a help command.
         *
         * \attention Implementors! This function just delegates work to the
         * same functions of helpful_option_builder by constructing one
         * inside the function. This is so we don't have to copy-paste the
         * same code over from that class. Do every modification of behavior
         * over there.
         *
         * \tparam Callback The type of the callback to use.
         * \param callback The callback to use for this option. Perfectly forwarded
         *                  to the class actually implementing the option creation.
         * \return The built option, containing the correct callback, without
         *          any help description defined.
         */
        template<class Callback>
        cli::option
        operator>>=(Callback&& callback) {
            // delegate all the work to the helpful
            return (helpful_option_builder(*this)).operator>>=(std::forward<Callback>(callback));
        }

        // deque: need access to push_front
        std::deque<std::string> names;///< The names that are aliased for the current option
    };

    /**
     * \brief Adds an alias to the option being built by the option_builder
     *
     * Takes the option builder and adds the contents of the name parameters to
     * it as an alias to the currently contained name and aliases.
     *
     * \param name The alias to add to the built option
     * \param bld The option_builder to add the alias to
     *
     * \return An option builder instance that has all old and the new aliases
     */
    INFO_CLI_API option_builder operator/(std::string_view name, option_builder bld);
    /// \copydoc operator/(std::string_view,option_builder)
    INFO_CLI_API option_builder operator/(char name, option_builder bld);
    /// \copydoc operator/(std::string_view,option_builder)
    INFO_CLI_API option_builder operator/(option_builder bld, std::string_view name);
    /// \copydoc operator/(std::string_view,option_builder)
    INFO_CLI_API option_builder operator/(option_builder bld, char name);
    /**
     * \brief Joins together two sets of option aliases and registers them under
     * the option of the first option
     *
     * Takes the options being built by the first option_builder and its aliases
     * and adds to it the second one's option and aliases, essentially integrating
     * the second option into the first one.
     *
     * \param bld1 The primer option to add the aliases to
     * \param bld2 The seconder option whose aliases and option will be added to the primer option
     * \return An option_builder instance containing all the aliases to the primer option
     */
    INFO_CLI_API option_builder operator/(option_builder bld1, const option_builder& bld2);
}
