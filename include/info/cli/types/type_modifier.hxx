/*
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Contains a checker for the set of type modifiers which slightly change the
 * behavior of parsing.
 * For example repeat<T> for integral types (for others it doesn't change anything)
 * allows an implicit increment to occur on each encountered option.
 *
 * Specializations for custom type modifiers are expected for type_modifier_<T>.
 */
#pragma once

#include <type_traits>

#include <info/cli/extra/repeat.hxx>
#include <info/cli/macros.hxx>

namespace info::cli {
    /**
     * \brief Type modifier check and reference normalization
     *
     * Takes a given type and through the value member returns whether the given
     * type is is a type modifier wrapping some kind of type. If it is, it gives
     * \c true, otherwise \c false.
     *
     * Its second purpose is that to allow undisturbed access to the variable the
     * user wants cli_parser to actually fill, even if it is wrapped in a type
     * modifier. For this, the operator() function is used, which always returns
     * the lvalue reference to the end-user's variable, regardless of in what form
     * it is presented to us.
     *
     * To define your own type modifiers this is one of the things you need
     * to specialize, see the implementation of repeat for reference and \ref Tutorial_4
     * for more information about what you need to specialize to create a type
     * modifier.
     *
     * \tparam T The type to check for modifierness(, even if that's not a word)
     */
    template<class T>
    struct INFO_CLI_API type_modifier_ : std::false_type {
        /**
         * \brief Returns an lvalue reference to the callback variable
         *
         * The type_modifier_ functors are used to strip the type modifiers
         * themselves from the given callbacks, by returning an lvalue reference
         * to the actual variable the user provided on the cli_parser constructor.
         *
         * Takes the bare, unmodified callback as given by the user and
         * returns it as an lvalue reference.
         *
         * \param x The user-given lvalue reference to their variable
         * \return The given lvalue reference to the user's variable
         */
        auto&
        operator()(T& x) const noexcept {
            return x;
        }
    };

    /**
     * \copybrief type_modifier_
     *
     * This is a specialized case of the type_modifier_ type trait. This is an
     * implementation for the InfoCLI provided repeat type modifier, thus is
     * evaluated to \c true.
     *
     * \tparam T The type embedded in the repeat type modifier
     */
    template<class T>
    struct INFO_CLI_API type_modifier_<repeat<T>> : std::true_type {
        /**
         * \brief Returns an lvalue reference to the user-given callback variable
         *
         * The type_modifier_ functors are used to strip the type modifiers
         * themselves from the given callbacks, by returning an lvalue reference
         * to the actual variable the user provided on the cli_parser constructor.
         *
         * Takes a forwarding reference to the given repeat type modifier instance
         * and then returns an lvalue reference by extracting it from the repeat
         * instance. This way the repeat can be a temporary, as it does not exist
         * during the parsing anymore, just during the construction.
         *
         * \tparam Ft Type for the forwarding reference
         * \param x The repeat instance to extract the lvalue reference from
         *
         * \return The lvalue reference extracted from the repeat instance
         */
        template<class Ft>
        auto&
        operator()(Ft&& x) const noexcept {
            static_assert(std::is_same_v<std::decay_t<Ft>, std::decay_t<repeat<T>>>,
                          "Quote the LaTeX badness 1000. Don't try to be sneaky.");
            return x.value;
        }
    };

    /**
     * \brief Convenience variable for checking if type is a type modifier
     *
     * A constexpr variable with the boolean value of the appropriate type_modifier_
     * template class's value static member. This can be used to decide about
     * a given type if it is a type modifier wrapping another type.
     *
     * \tparam T The type to check for being a type modifier
     */
    template<class T>
    constexpr const static auto type_modifier = type_modifier_<T>::value;
}
