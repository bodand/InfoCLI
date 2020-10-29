/*
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Utility class for allowing repeated callbacks to functor callbacks.
 */
#pragma once

namespace info::cli {
    /**
     * \brief A type modifier that allows an integral type to sum its appearances
     *
     * A type modifier that modifies integral types such that when they are present
     * multiple types, whey are added together instead of overwriting earlier
     * values.
     *
     * For other types, this behaves as provided by the _cli::aggregator_ type's
     * operator() member function for the encompassed \c T type.
     *
     * \tparam T The type to modify
     */
    template<class T>
    struct repeat {
        T& value;///< The reference to the original variable to modify during parsing
    };

    /**
     * A deduction guide for repeat construction
     */
    template<class T>
    repeat(T&) -> repeat<T>;
}
