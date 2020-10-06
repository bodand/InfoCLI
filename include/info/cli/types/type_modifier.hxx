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
    template<class>
    struct INFO_CLI_API type_modifier_ : std::false_type { };

    template<class T>
    struct INFO_CLI_API type_modifier_<repeat<T>> : std::true_type { };

    template<class T>
    constexpr const static auto type_modifier = type_modifier_<T>::value;
}
