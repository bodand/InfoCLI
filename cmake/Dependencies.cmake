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

## Download GetDependency ##
if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GetDependency.cmake")
    message("[${PROJECT_NAME}] Downloading GetDependency")
    file(DOWNLOAD
         "https://raw.githubusercontent.com/bodand/GetDependency/master/cmake/GetDependency.cmake"
         "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GetDependency.cmake"
         )
endif ()
include(GetDependency)

message(CHECK_START "[${PROJECT_NAME}] Setting up dependencies")
list(APPEND CMAKE_MESSAGE_INDENT "  ")
if (INFO_CLI_BUILD_BENCHMARKS)
    set(_DEP_COUNT 6)
else ()
    if (INFO_CLI_BUILD_TESTS)
        set(_DEP_COUNT 3)
    else ()
        set(_DEP_COUNT 2)
    endif ()
endif ()

## {fmt} ##
message(CHECK_START "[${PROJECT_NAME}] '{fmt}' (1/${_DEP_COUNT})")
GetDependency(fmt
              REPOSITORY_URL https://github.com/fmtlib/fmt.git
              VERSION 7.0.3
              REMOTE_ONLY
              )
message(CHECK_PASS "found")

## InfoUtils ##
message(CHECK_START "[${PROJECT_NAME}] 'InfoUtils' (2/${_DEP_COUNT})")
GetDependency(InfoUtils
              REPOSITORY_URL https://github.com/bodand/InfoUtils.git
              VERSION v1.6.0
              )
message(CHECK_PASS "found")

## Catch2 ##
if (INFO_CLI_BUILD_TEST)
    message(CHECK_START "[${PROJECT_NAME}] 'Catch2' (3/${_DEP_COUNT})")
    GetDependency(Catch2
                  REPOSITORY_URL https://github.com/catchorg/Catch2.git
                  VERSION v2.13.1
                  REMOTE_ONLY
                  )
    message(CHECK_PASS "found")
endif()

# Benchmark dependencies
if (INFO_CLI_BUILD_BENCHMARKS)
    ## Boost.ProgramOptions
    # Boost's answer to CLI parsing. A Boost-style, heavyweight library
    # Either depends on other parts of Boost, or just doesn't provide
    # a CMake project as-is, so we need to pull in the whole thing
    # fun times
    message(CHECK_START "[${PROJECT_NAME}] 'Boost' (4/${_DEP_COUNT})")
    find_package(Boost COMPONENTS program_options)
    if (NOT Boost_FOUND)
        list(APPEND INFO_CLI_MISSING_DEPS "Boost")
        message(CHECK_FAIL "failed")
    else ()
        message(CHECK_PASS "found")
    endif ()

    ## cxxopts
    # CLI parser with a similar interface to Boost.ProgramOptions, but is
    #  - header-only
    #  - not part of Boost
    #  - much lighter-weight
    #  - according to my previous benchmarks; way faster than Boost.ProgramOptions
    message(CHECK_START "[${PROJECT_NAME}] 'cxxopts' (5/${_DEP_COUNT})")
    GetDependency(cxxopts
                  REPOSITORY_URL https://github.com/jarro2783/cxxopts.git
                  VERSION v2.2.1
                  )
    message(CHECK_PASS "found")
endif ()

list(POP_BACK CMAKE_MESSAGE_INDENT)
if ("x${INFO_CLI_MISSING_DEPS}x" STREQUAL "xx")
    message(CHECK_PASS "completed")
else ()
    message(CHECK_FAIL "failed")
    message(FATAL_ERROR "[${PROJECT_NAME}] Missing dependencies that could not have been found: ${INFO_CLI_MISSING_DEPS}")
endif ()
