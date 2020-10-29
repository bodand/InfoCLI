// BSD 3-Clause License
//
// Copyright (c) 2020, bodand
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

// stdlib
#include <deque>
#include <list>
#include <queue>
#include <stack>
#include <type_traits>
#include <vector>

#include <info/cli/aggregator.hxx>
#include <info/cli/macros.hxx>
#include <info/cli/types/type_modifier.hxx>

/**
 * \brief TMP helper classes for InfoCLI
 *
 * A set of utilities to ease template metaprogramming inside InfoCLI.
 * Not heavily utilized, as InfoCLI does not really on that much template
 * trickery but are good to have.
 *
 * Interface is mostly stable, but the namespace is considered an implementation
 * detail.
 */
namespace info::cli::meta {
    /**
     * \brief A typelist
     *
     * A type that can hold a list of types
     *
     * \tparam Args... The types in the list
     */
    template<class... Args>
    struct INFO_CLI_LOCAL tlist {
    };

    // head --------------------------------------------------------------------
    /**
     * \brief Bad usage of the \c meta::head meta-function
     *
     * This is what happens if the typelist-like entity given to \c meta::head
     * is in fact not a typelist-like entity, or it does not contain any elements.
     */
    template<class>
    struct head_;

    /**
     * \brief Takes any typelist-like entity and returns the first element
     *        it holds
     *
     * Given any typelist-like type, it takes the first argument in the list of
     * the type-list and returns that type.
     *
     * \tparam L The typelist-like type that can store types
     * \tparam T The type we are going to extract
     * \tparam Args... The remaining types in the type-list
     */
    template<template<class...> class L,
             class T,
             class... Args>
    struct head_<L<T, Args...>> {
        using type = T;///< The return type
    };

    /**
     * \brief Meta-function returning the first type in a typelist
     *
     * A meta-function that takes any type that looks like a typelist, and
     * returns the first element in it. It fails miserably if the type is
     * not a typelist-like type, or it does not contain any types.
     *
     * \tparam L The typelist to return the first type from
     *
     * \return The first type in the given list
     */
    template<class L>
    using head = typename head_<L>::type;

    // is_aggregating ----------------------------------------------------------
    /**
     * \brief Class to query aggregating-related properties of types
     *
     * For any given type it returns in the \c value member, whether the
     * given value is aggregating or not as a boolean, of course.
     *
     * Whether it is, it'll return the type the aggregating type is aggregating
     * on, otherwise just the given type using the \c type member.
     *
     * \tparam T The type to query for aggregating-ness properties
     */
    template<class T>
    struct INFO_CLI_LOCAL is_aggregating_ : std::bool_constant<_cli::aggregator_<T>::value> {
        using type = T;///< The return type
    };

    /**
     * \brief A meta-function that returns whether a given type is aggregating
     *
     * \return Whether the given type is aggregating
     */
    template<class T>
    INFO_CLI_LOCAL constexpr static bool is_aggregating = is_aggregating_<T>::value;

    /**
     * \brief A meta-function that returns the type a given aggregating type
     *         aggregates on
     *
     * Takes an aggregating type and returns the type the aggregating type
     * aggregates on. For example \c std::vector<int> is an aggregating type
     * which aggregates on the \c int type.
     *
     * \return The type on which the aggregating type aggregates
     */
    template<class T>
    using aggregating_type = typename is_aggregating_<T>::type;

    // expected_type -----------------------------------------------------------
    /**
     * \brief Returns the type to be associated with the given type by InfoRTTI
     *
     * Returns a type for any given type that is to be used when constructing
     * the InfoRTTI entry for a given option.
     *
     * Normally, this is the result of the aggregating_type meta-functoin, since
     * we do not want the type-info about the aggregating types themselves, as
     * they are not supported as an element to be parsed. (Can be done, but
     * you'll really love not doing it.)
     *
     * Type modifiers, however, change the game, as by default the aggregating_type
     * meta-function strips these modifiers, but InfoRTTI needs to know about them
     * as they may modify parsing behavior, thus the expected_type meta-function.
     *
     * \tparam T The type to get the InfoRTTI compatible version of
     */
    template<class T>
    struct INFO_CLI_LOCAL expected_type_ {
        using type = aggregating_type<T>;///< The return type
    };

    /**
     * \copybrief expected_type_
     *
     * Implementation classes of the expected_type meta-function. This
     * is a specialization for a type modifier, thus it returns the provided
     * type, with the modifier in-tact.
     *
     * \tparam T The type engulfed by the repeat type modifier
     */
    template<class T>
    struct INFO_CLI_LOCAL expected_type_<cli::repeat<T>> {
        using type = cli::repeat<T>;///< The return type
    };

    /**
     * \brief A meta-function that returns the type that's to be used by
     *         InfoRTTI during runtime
     *
     * Takes a type and returns the type to be used in the InfoRTTI runtime
     * attribute lookup. This is because some modifiers may modify certain
     * attributes of their type, about which InfoRTTI needs to know during
     * runtime to act appropriately.
     *
     * By default, this type is given by the aggregating_type meta-function,
     * except when modifiers enter the scene.
     *
     * \return The type which is used in InfoRTTI
     */
    template<class T>
    using expected_type = typename expected_type_<T>::type;

    // referenced_type -----------------------------------------------------------
    /**
     * \brief Returns the lvalue reference type which is of the type the user
     *         expected back into the given variable
     *
     * Returns an lvalue reference for any given T, as this is only modified
     * in cases of type modifiers.
     *
     * \tparam T The type to get the corrrect lvalue reference type of
     */
    template<class T>
    struct INFO_CLI_LOCAL referenced_type_ {
        using type = std::add_lvalue_reference_t<T>;///< The return type
    };

    /**
     * \copybrief referenced_type_
     *
     * Strips the given type modifier, and then forms an lvalue reference to the
     * type found as such.
     *
     * \tparam T The type engulfed by the type modifier (\c cli::repeat)
     */
    template<class T>
    struct INFO_CLI_LOCAL referenced_type_<cli::repeat<T>> {
        using type = std::add_lvalue_reference_t<T>;
    };

    /**
     * \brief A meta-function that returns the lvalue reference type that's
     *         actually given by the user
     *
     * Takes a type and always returns the lvalue reference to which the final
     * result of the option call is to be passed. This way we can optionally
     * remove type modifiers which change the found type and the actually wanted
     * lvalue type.
     *
     * \return The lvalue type the user's input referenced
     */
    template<class T>
    using referenced_type = typename referenced_type_<T>::type;
}
