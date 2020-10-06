/** InfoCLI project
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Utility class for allowing repeated callbacks to functor callbacks.
 */
#pragma once

namespace info::cli {
    template<class T>
    struct repeat {
        T& value;
    };

    template<class T>
    repeat(T&) -> repeat<T>;
}
