## BSD 3-Clause License
#
# Copyright (c) 2020, bodand
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## Catch CMake support ##
find_package(Catch2 CONFIG REQUIRED)
include(Catch)

## Project ##
project(InfoCLI_Tests
        CXX
        )

add_executable(cli_test
               src/dissector.cxx
               src/main.cxx
               src/option_builder.cxx
               src/type_parser.cxx
               src/cli_parser.cxx
               src/cli_parser.long.cxx
               src/cli_parser.unpacked.cxx
               src/cli_parser.packed.cxx
               )

target_link_libraries(cli_test
                      PUBLIC info::cli
                      PUBLIC Catch2::Catch2
                      )

target_include_directories(cli_test PUBLIC
                           ${CMAKE_CURRENT_SOURCE_DIR}/include
                           )

target_compile_options(cli_test
                       PUBLIC ${CLI_WARNINGS})

set_target_properties(cli_test PROPERTIES
                      CXX_STANDARD 14)

add_custom_command(TARGET cli_test POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:info::cli>" "$<TARGET_FILE_DIR:cli_test>"
                   )

catch_discover_tests(cli_test)
