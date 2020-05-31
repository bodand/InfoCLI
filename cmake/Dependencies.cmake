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


include(Dependency)

## List of normal dependencies

# Metaprogramming library
GetDependency(
        Hana
        REPOSITORY_URL https://github.com/ldionne/hana.git
        VERSION v1.6.0
        FALLBACK Boost
)

# Metaprogramming library on the type-level
GetDependency(
        boost_mp11
        REPOSITORY_URL https://github.com/boostorg/mp11.git
        VERSION boost-1.73.0
        FALLBACK Boost
)

# {fmt}
GetDependency(
        fmt
        REPOSITORY_URL https://github.com/fmtlib/fmt.git
        VERSION 6.2.1
)

# Info* project utilities
set(INFO_UTILS_BUILD_TESTS Off)
# InfoUtils needs to disable tests when not main project
GetDependency(
        InfoUtils
        REPOSITORY_URL https://github.com/isbodand/InfoUtils.git
        VERSION v1.5.0
)

# Test dependencies
if (INFO_CLI_BUILD_TESTS)
    # Catch2 testing framework
    GetDependency(
            Catch2
            REPOSITORY_URL https://github.com/catchorg/Catch2.git
            VERSION v2.12.1
    )
    # this is most likely implementation detail, if it acts up yell at me
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CACHEFILE_DIR}/_deps/catch2-src/contrib")
endif ()

# Benchmark dependencies
if (INFO_CLI_BUILD_BENCHMARKS)
    ## Boost.ProgramOptions
    # Boost's answer to CLI parsing. A Boost-style, heavyweight,
    # can even cook your dinner if you need it library
    # Either depends on other parts of Boost, or just doesn't provide
    # a CMake project as-is, so we need to pull in the whole thing
    # fun times
    GetDependency(
            Boost
            COMPONENTS program_options
            REPOSITORY_URL https://github.com/boostorg/boost.git
            VERSION v1.73.0
    )

    ## cxxopts
    # CLI parser with a similar interface to Boost.ProgramOptions, but is
    #  - header-only
    #  - not part of Boost
    #  - much lighter-weight
    GetDependency(
            cxxopts
            REPOSITORY_URL https://github.com/jarro2783/cxxopts.git
            VERSION v2.2.0
    )

    ## Celero
    # Benchmarking library
    GetDependency(
            Celero
            REPOSITORY_URL https://github.com/DigitalInBlue/Celero.git
            VERSION v2.6.0
    )
endif ()
