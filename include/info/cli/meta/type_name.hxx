/*
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Implements a function that returns a name of a type on the big 3 compilers.
 * Credit's due to The Programmer's Hangout Discord Server's #cpp channel
 * where I got the inspiration from.
 */
#pragma once

#include <string_view>

namespace info::cli::meta {
    /**
     * \brief A rather crude attempt at human readable type-names
     *
     * Returns a mostly readable name for a type.
     *
     * \warning This is highly experimental and probably will get replaced,
     * by something more customizable and that provides better results.
     *
     * \tparam T The type whose name we want.
     * \return The name of the type, as the compiler sees it, but not mangled
     */
    template<typename T>
    constexpr auto
    type_name() {
        std::string_view name;
        std::size_t pfx_len = 0, sfx_len = 0;
#ifdef __clang__
        name = __PRETTY_FUNCTION__;
        pfx_len = sizeof "auto info::cli::meta::type_name() [T = " - 1;
        sfx_len = sizeof "]" - 1;
#elif defined(__GNUC__)
        name = __PRETTY_FUNCTION__;
        pfx_len = sizeof "constexpr auto info::cli::meta::type_name() [with T = " - 1;
        sfx_len = sizeof "]" - 1;
#elif defined(_MSC_VER)
        name = __FUNCSIG__;
        pfx_len = sizeof "auto __cdecl info::cli::meta::type_name<" - 1;
        sfx_len = sizeof ">(void)" - 1;
#endif
        name.remove_prefix(pfx_len);
        name.remove_suffix(sfx_len);
        return name;
    }

    template<>
    constexpr auto
    type_name<std::string>() {// something like this is the future
        return "string";      // but natch not specialized function templates
    }

    template<>
    constexpr auto
    type_name<std::string_view>() {
        return "string";
    }
}
