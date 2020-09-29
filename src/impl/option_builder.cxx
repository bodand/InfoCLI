/** InfoCLI project
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Implements the zeroth step in the creation of options: option_builder_0
 */

#include <info/cli/option.hxx>

#include <algorithm>
#include <iterator>
#include <utility>

info::_cli::option_builder::option_builder(std::string_view name)
     : names{name.data()} {
}

info::_cli::helpful_option_builder
info::_cli::option_builder::operator>=(std::string_view help) {
    return {help, *this};
}

info::_cli::option_builder /* clang-format off */
info::_cli::operator/(info::_cli::option_builder bld, std::string_view name) {
                           /* clang-format on */
    bld.names.emplace_back(name.data());
    return bld;
}

info::_cli::option_builder /* clang-format off */
info::_cli::operator/(std::string_view name, info::_cli::option_builder bld) {
                           /* clang-format on */
    bld.names.emplace_front(name.data());
    return bld;
}

info::_cli::option_builder /* clang-format off */
info::_cli::operator/(info::_cli::option_builder bld1,
                      const info::_cli::option_builder& bld2) {
                           /* clang-format on */
    std::copy(bld2.names.begin(), bld2.names.end(), std::back_inserter(bld1.names));
    return bld1;
}

info::_cli::option_builder
info::_cli::operator/(char name, info::_cli::option_builder bld) {
    return std::string(1, name) / std::move(bld);
}

info::_cli::option_builder
info::_cli::operator/(info::_cli::option_builder bld, char name) {
    return std::move(bld) / std::string(1, name);
}
