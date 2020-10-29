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

#include <info/cli/extra/repeat.hxx>
#include <info/cli/macros.hxx>

namespace info::_cli {
    /**
     * \brief Aggregates aggregating types
     *
     * When given an aggregating type, the \c value member resolves to true
     * and the \c operator() member function is defined, otherwise \c value
     * contains false.
     *
     * In either case, the \c type member typedef contains the type that's going
     * to get aggregated, or the given type if type is not aggregating.
     *
     * \tparam T The type to aggregate on
     */
    template<class T>
    struct INFO_CLI_API aggregator_ : std::false_type {
        using type = T;///< The type aggregated on
    };

    /**
     * \copybrief aggregator_
     *
     * Defines the way to properly handle a type with a repeat type modifier:
     * if it is an integral type, increment it by the value found, otherwise
     * recursively call the aggregator for the parameter type of the type
     * modifier.
     *
     * \tparam T The modified type
     */
    template<class T>
    struct INFO_CLI_API aggregator_<cli::repeat<T>> : std::true_type {
        using type = T;///< The modified type

        /**
         * \brief Performs aggregation on a modified type's extracted lvalue reference
         *
         * Takes an lvalue reference to the type wrapped by the type modifier
         * which is then aggregated on.
         *
         * \tparam U The type of the value to be aggregated into the lvalue reference
         *
         * \param rep The lvalue reference to the wrapped type to modify
         * \param elem The forwarding reference to aggregate into \c rep
         *
         * \return void (auto)
         */
        template<class U>
        auto /* !! CAUTION !! \
               this function contrary to all other aggregator_<T> types does NOT
               take T, but U where T is cli::repeat<U>. This is because
               we somehow need to change passed (to cli_parser) rvalue reference
               to repeat<U>&& to the within contained lvalue reference to U& */
        operator()(T& rep, U&& elem) const {
            static_assert(std::is_assignable_v<T&, decltype(std::forward<U>(elem))>,
                          "Value of aggregating type is not constructable with given type");
            if constexpr (std::is_integral_v<T>) {
                rep += std::forward<U>(elem);
            } else {
                if constexpr (auto agg = aggregator_<T>{}) {
                    agg(rep, std::forward<U>(elem));
                } else {
                    rep = std::forward<U>(elem);
                }
            }
        }
    };

    /**
     * \copybrief aggregator_
     *
     * Aggregator that aggregates vectors of type T.
     * Vectors are aggregated using their emplace_back member functions.
     *
     * \tparam T The contained type of the vector
     */
    template<class T>
    struct INFO_CLI_API aggregator_<std::vector<T>> : std::true_type {
        using type = T;///< The contained type

        /**
         * \brief Aggregates the given vector
         *
         * Aggregates a new element into the vector by emplacing the forwarded
         * element at the end.
         *
         * \tparam U Type of the element to be added
         *
         * \param cont The vector to add into
         * \param elem The elem to be moved into the vector
         *
         * \return void (auto)
         */
        template<class U>
        auto
        operator()(std::vector<T>& cont, U&& elem) const {
            static_assert(std::is_constructible_v<T, decltype(std::forward<U>(elem))>,
                          "Value of aggregating type is not constructable with given type");
            cont.emplace_back(std::forward<U>(elem));
        }
    };

    /**
     * \copybrief aggregator_
     *
     * Aggregator that aggregates lists of type T.
     * Lists are aggregated using their emplace_back member functions.
     *
     * \tparam T The contained type of the list
     */
    template<class T>
    struct INFO_CLI_API aggregator_<std::list<T>> : std::true_type {
        using type = T;///< The contained type

        /**
         * \brief Aggregates the given list
         *
         * Aggregates a new element into the list by emplacing the forwarded
         * element at the end.
         *
         * \tparam U Type of the element to be added
         *
         * \param cont The list to add into
         * \param elem The elem to be moved into the list
         *
         * \return void (auto)
         */
        template<class U>
        auto
        operator()(std::list<T>& cont, U&& elem) const {
            static_assert(std::is_constructible_v<T, decltype(std::forward<U>(elem))>,
                          "Value of aggregating type is not constructable with given type");
            cont.emplace_back(std::forward<U>(elem));
        }
    };

    /**
     * \copybrief aggregator_
     *
     * Aggregator that aggregates stacks of type T.
     * Stacks are aggregated using their emplace member functions and
     * new elements will be pushed onto the stack.
     *
     * \tparam T The contained type of the stack
     */
    template<class T>
    struct INFO_CLI_API aggregator_<std::stack<T>> : std::true_type {
        using type = T;///< The contained type

        /**
         * \brief Aggregates the given stack
         *
         * Aggregates a new element into the stack by emplacing the forwarded
         * element.
         *
         * \tparam U Type of the element to be added
         *
         * \param cont The stack to push onto
         * \param elem The elem to be moved onto the stack
         *
         * \return void (auto)
         */
        template<class U>
        auto
        operator()(std::stack<T>& cont, U&& elem) const {
            static_assert(std::is_constructible_v<T, decltype(std::forward<U>(elem))>,
                          "Value of aggregating type is not constructable with given type");
            cont.emplace(std::forward<U>(elem));
        }
    };

    /**
     * \copybrief aggregator_
     *
     * Aggregator that aggregates queues of type T.
     * Queues are aggregated using their emplace member functions,
     * when new elements are added to the queue.
     *
     * \tparam T The contained type of the queue
     */
    template<class T>
    struct INFO_CLI_API aggregator_<std::queue<T>> : std::true_type {
        using type = T;///< The contained type

        /**
         * \brief Aggregates the given queue
         *
         * Aggregates a new element into the queue by emplacing the forwarded
         * element at the end.
         *
         * \tparam U Type of the element to be added
         *
         * \param cont The queue to add into
         * \param elem The elem to be moved into the queue
         *
         * \return void (auto)
         */
        template<class U>
        auto
        operator()(std::queue<T>& cont, U&& elem) const {
            static_assert(std::is_constructible_v<T, decltype(std::forward<U>(elem))>,
                          "Value of aggregating type is not constructable with given type");
            cont.emplace(std::forward<U>(elem));
        }
    };

    /**
     * \copybrief aggregator_
     *
     * Aggregator that aggregates deques of type T.
     * Deques are aggregated using their emplace_back member functions, thus
     * they behave as if they were a simple queue in the hands of InfoCLI.
     *
     * \tparam T The contained type of the deque
     */
    template<class T>
    struct INFO_CLI_API aggregator_<std::deque<T>> : std::true_type {
        using type = T;///< The contained type

        /**
         * \brief Aggregates the given deque
         *
         * Aggregates a new element into the deque by emplacing the forwarded
         * element at the end.
         *
         * \tparam U Type of the element to be added
         *
         * \param cont The deque to add into
         * \param elem The elem to be moved into the deque
         *
         * \return void (auto)
         */
        template<class U>
        auto
        operator()(std::deque<T>& cont, U&& elem) const {
            static_assert(std::is_constructible_v<T, decltype(std::forward<U>(elem))>,
                          "Value of aggregating type is not constructable with given type");
            cont.emplace_back(std::forward<U>(elem));
        }
    };

    /**
     * \brief Instance of an aggregator for type \c T
     *
     * \tparam T The type to perform the aggregating functions on
     */
    template<class T>
    INFO_CLI_LOCAL constexpr static auto aggregator = aggregator_<T>{};

    /**
     * \brief Meta-function to get the type the aggregator type is aggregating on
     *
     * Returns the type on which the aggregating type \c T aggregates, or if the
     * given type is not aggregating it returns the given type.
     *
     * \tparam T The type to inspect
     *
     * \return The type on which the aggregating type aggregates, or the
     *          type itself
     */
    template<class T>
    using aggregator_type = typename aggregator_<T>::type;
}
