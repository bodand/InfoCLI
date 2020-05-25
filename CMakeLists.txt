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

if (MSVC)
    message(FATAL_ERROR "MSVC is not supported. Be a better person and use Clang or GCC. Thank you.")
endif ()

## CMake shenanigans
cmake_minimum_required(VERSION 3.14)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
set(CMAKE_CXX_STANDARD 17)

include(GNUInstallDirs)
include(Utils)

## Project options
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(INFO_CLI_MAIN On)
else ()
    set(INFO_CLI_MAIN Off)
endif ()

option(INFO_CLI_BUILD_TESTS "Build the the InfoCLI test suite [Off as dependency]" ${INFO_CLI_MAIN})
option(INFO_CLI_BUILD_EXAMPLES "Build the the InfoCLI usage examples [Off as dependency]" ${INFO_CLI_MAIN})
option(INFO_CLI_BUILD_BENCHMARKS "Build the the InfoCLI benchmark [Off]" Off)
option(INFO_CLI_BUILD_SHARED "Build the the InfoCLI as a shared library [On]" On)
NameOption(${INFO_CLI_BUILD_SHARED} "SHARED;STATIC" INFO_CLI_BUILD_TYPE)

## Project
project(InfoCLI
        VERSION 0.1.0
        LANGUAGES CXX)

## Project settings
set(INFO_PROJECT_NAME CLI)
set(INFO_PROJECT_NAMESPACE info::cli)

## Debug flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")

## Project level macros
set(${INFO_PROJECT_NAME}_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(${INFO_PROJECT_NAME}_HEADER_PATH "${${INFO_PROJECT_NAME}_INCLUDE_DIR}/info/cli")
set(${INFO_PROJECT_NAME}_TARGET icli)

## Initialize target
add_library(${${INFO_PROJECT_NAME}_TARGET} ${INFO_CLI_BUILD_TYPE}
            src/parse_exception.cpp
            src/split.cpp
            src/cli_parser.cpp
            src/type_parser.cpp
            src/help_generator.cpp)
add_library(${INFO_PROJECT_NAMESPACE} ALIAS ${${INFO_PROJECT_NAME}_TARGET})

## Require C++17
set_target_properties(${${INFO_PROJECT_NAME}_TARGET}
                      PROPERTIES
                      CXX_STANDARD 17)

# Check for required features and
#  applicable warnings
include(FeatureTests)

# Add warnings
target_compile_options(${${INFO_PROJECT_NAME}_TARGET}
                       PRIVATE
                       ${${INFO_PROJECT_NAME}_WARNINGS})

# Dependencies
include(Dependencies)

## Get dependencies
target_link_libraries(${${INFO_PROJECT_NAME}_TARGET}
                      PUBLIC info::utils
                      PUBLIC ${Hana_LINK_AS}
                      PUBLIC ${boost_mp11_LINK_AS}
                      PRIVATE fmt::fmt
                      )

## Set target properties
target_include_directories(${${INFO_PROJECT_NAME}_TARGET}
                           PUBLIC
                           $<INSTALL_INTERFACE:include>
                           $<BUILD_INTERFACE:${${INFO_PROJECT_NAME}_INCLUDE_DIR}>
                           )

## Optionally enable tests
if (INFO_CLI_BUILD_TESTS)
    message(STATUS "[InfoCLI] Building tests")
    add_subdirectory(test)
endif ()

## Optionally enable examples
if (INFO_CLI_BUILD_EXAMPLES)
    message(STATUS "[InfoCLI] Building examples")
    add_subdirectory(examples)
endif ()

## Optionally enable benchmark
if (INFO_CLI_BUILD_BENCHMARKS)
    message(STATUS "[InfoCLI] Building benchmarks - are you sure you need this?")
    add_subdirectory(benchmark)
endif ()
