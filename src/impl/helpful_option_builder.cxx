/*
 * Copyright (c) 2020 bodand
 * Licensed under the BSD 3-Clause license
 *
 * Implements the zeroth step in the creation of options: option_builder_0
 */

#include <info/cli/option.hxx>

info::_cli::helpful_option_builder::helpful_option_builder(const info::_cli::option_builder& bld)
     : helpful_option_builder("", bld) {
}

info::_cli::helpful_option_builder::helpful_option_builder(std::string_view help,
                                                           const info::_cli::option_builder& bld)
     : help(help),
       names(bld.names.begin(), bld.names.end()) {
}
