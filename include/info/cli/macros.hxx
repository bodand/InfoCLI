/*
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Defines different macros that provide compiler differences eg. "__builtin_unreachable",
 * and microsoft ABI dllexport things.
 */
#pragma once

/**
 * \brief Compiler independent import attribute for shared libraries
 *
 * A macro that expands to an attribute that compiler depenedently always
 * means that the specified symbol is to be imported from the shared library.
 */
#ifdef _MSC_VER
#    define INFO_CLI_DLLIMPORT __declspec(dllimport)
#else
#    define INFO_CLI_DLLIMPORT __attribute__((dllimport))
#endif

/**
 * \brief Compiler independent export attribute for shared libraries
 *
 * A macro that expands to an attribute that compiler depenedently always
 * means that the specified symbol is to be exported by the shared library.
 */
#ifdef _MSC_VER
#    define INFO_CLI_DLLEXPORT __declspec(dllexport)
#else
#    define INFO_CLI_DLLEXPORT __attribute__((dllexport))
#endif

// dll im-/export
#if !INFO_CLI_BUILD_STATIC \
       && !defined(INFO_CLI_DOXYGEN_RUN)
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
/**
 * \brief Macro used to denote entities that are part of the public API
 *
 * A macro that expands to either nothing, when building a static library,
 * or a compiler dependent attribute that is used to mean that the entitiy
 * is part of the public API, and should be exported from a DLL, or its visibility
 * should be "default" on ELF targets.
 */
#    define INFO_CLI_API

/**
 * \brief Macro used to denote entities that are NOT part of the public API
 *
 * A macro that expands to either nothing, when building a static library,
 * or a compiler dependent attribute that is used to mean that the entitiy
 * is NOT part of the public API, and should not be exported from a DLL, or
 * its visibility should be "hidden" on ELF targets.
 */
#    define INFO_CLI_LOCAL
#endif

/**
 * \brief Macro for marking impossible execution paths
 *
 * A macro expanding the to the correct built-in of the compiler to let it know
 * that that branch is never to be taken.
 */
#ifdef _MSC_VER
#    define INFO_CLI_NOT_HAPPENING __assume(false)
#else
#    define INFO_CLI_NOT_HAPPENING __builtin_unreachable()
#endif

/**
 * \brief Branch probability macro. Used if something is likely to be true
 *
 * A macro that is used in branching constructs to mean that the true branch is
 * the one that's way more likely to happen. If the compiler does not support
 * it, it just expands to its arguments.
 */
#ifdef _MSC_VER
#    define INFO_CLI_LIKELY(...) (__VA_ARGS__)
#else
#    define INFO_CLI_LIKELY(...) (__builtin_expect(!!(__VA_ARGS__), 1))
#endif

/**
 * \brief Branch probability macro. Used if something is likely to be false
 *
 * A macro that is used in branching constructs to mean that the false branch is
 * the one that's way more likely to happen. If the compiler does not support
 * it, it just expands to its arguments.
 */
#ifdef _MSC_VER
#    define INFO_CLI_UNLIKELY(...) (__VA_ARGS__)
#else
#    define INFO_CLI_UNLIKELY(...) (__builtin_expect(!!(__VA_ARGS__), 0))
#endif

/**
 * \brief Function purity macro. The function does not produce side-effects
 *
 * A macro that expands into an attribute (if supported) that tells the compiler
 * that said function is pure in a sense that it does not modify global state
 * of the program, other than its return value.
 */
#ifdef __GNUG__
#    define INFO_CLI_PURE __attribute__((pure))
#else
#    define INFO_CLI_PURE
#endif

/**
 * \brief Function purity macro. The function is deterministic on its arguments
 *
 * A macro that expands into an attribute (if supported) that tells the compiler
 * that said function is pure in a sense that it does not modify global state
 * of the program, other than its return value, AND does not read from any
 * other parts of the program that are not its arguments, which means it's
 * deterministic.
 */
#ifdef __GNUG__
#    define INFO_CLI_CONST __attribute__((const))
#else
#    define INFO_CLI_CONST
#endif
