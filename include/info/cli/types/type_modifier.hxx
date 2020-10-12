/** InfoCLI project
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
    template<class T>
    struct INFO_CLI_API type_modifier_ : std::false_type {
        auto&
        operator()(T& x) const noexcept {
            return x;
        }
    };

    template<class T>
    struct INFO_CLI_API type_modifier_<repeat<T>> : std::true_type {
        template<class Ft>
        auto&
        operator()(Ft&& x) const noexcept {
            static_assert(std::is_same_v<std::decay_t<Ft>, std::decay_t<repeat<T>>>,
                          "Quote the LaTeX badness 1000. Don't try to be sneaky.");
            return x.value;
        }
    };

    template<class T>
    constexpr const static auto type_modifier = type_modifier_<T>::value;
}
