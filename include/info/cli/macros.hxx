/** InfoCLI project
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Defines different macros that provide compiler differences eg. "__builtin_unreachable",
 * and microsoft ABI dllexport things.
 */
#pragma once

#ifdef _MSC_VER
#    define INFO_CLI_DLLIMPORT __declspec(dllimport)
#    define INFO_CLI_DLLEXPORT __declspec(dllexport)
#else
#    define INFO_CLI_DLLIMPORT __attribute__((dllimport))
#    define INFO_CLI_DLLEXPORT __attribute__((dllexport))
#endif

// dll im-/export
#if !INFO_CLI_BUILD_STATIC
#    if defined(_WIN32) || defined(__CYGWIN__)
#        if INFO_CLI_EXPORTS
#            define INFO_CLI_API INFO_CLI_DLLEXPORT
#        else
#            define INFO_CLI_API INFO_CLI_DLLIMPORT
#        endif
#        define INFO_CLI_LOCAL
#    else
#        if __has_attribute(visibility)
#            define INFO_CLI_API __attribute__((visibility("default")))
#            define INFO_CLI_LOCAL __attribute__((visibility("hidden")))
#        else
#            define INFO_CLI_API
#            define INFO_CLI_LOCAL
#        endif
#    endif
#else
#    define INFO_CLI_API
#    define INFO_CLI_LOCAL
#endif

// uncreachable path
#ifdef _MSC_VER
#    define INFO_CLI_NOT_HAPPENING __assume(false)
#else
#    define INFO_CLI_NOT_HAPPENING __builtin_unreachable()
#endif

// un/likely
#ifdef _MSC_VER
#    define INFO_CLI_LIKELY(...) (__VA_ARGS__)
#    define INFO_CLI_UNLIKELY(...) (__VA_ARGS__)
#else
#    define INFO_CLI_LIKELY(...) (__builtin_expect(!!(__VA_ARGS__), 1))
#    define INFO_CLI_UNLIKELY(...) (__builtin_expect(!!(__VA_ARGS__), 0))
#endif

#if defined(__GNUC__) && !defined(__clang__)
#    define INFO_CLI_PURE __attribute__((pure))
#    define INFO_CLI_CONST __attribute__((const))
#else
#    define INFO_CLI_PURE
#    define INFO_CLI_CONST
#endif
