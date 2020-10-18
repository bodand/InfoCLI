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

cmake_minimum_required(VERSION 3.14)

project(InfoCLI_Benchmarks CXX)

include(BenchmarkUtils)
include(CheckIPOSupported)

check_ipo_supported(RESULT LTO_SUPPORTED
                    LANGUAGES CXX
                    )

## Create benchmark datasets
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/data")
set(CLI_BENCHMARK_SETS 100 200 283 500)
foreach (set IN LISTS CLI_BENCHMARK_SETS)
    if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/data/${set}.input.non-gnu.txt")
        continue()
    endif ()

    foreach (n RANGE 1 ${set})
        InputData_NonGNU(${n} NonGNU_Input_LINE)
        InputData_GNU(${n} GNU_Input_LINE)
        BoostData(${n} Boost_LINE)
        CXXOptsData(${n} CXXOpts_LINE)
        InfoData(${n} Info_LINE)
        GnuData(${n} GNU_LINE GNU_HELP GNU_CHECK)

        # Input Data to parse against
        file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/data/${set}.input.non-gnu.txt" "${NonGNU_Input_LINE}")
        file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/data/${set}.input.gnu.txt" "${GNU_Input_LINE}")
        # Data for Boost.ProgramOptions
        file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/data/${set}.boost.txt" "${Boost_LINE}")
        # Data for cxxopts
        file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/data/${set}.cxxopts.txt" "${CXXOpts_LINE}")
        # Data for InfoCLI
        file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/data/${set}.info.txt" "${Info_LINE}")
        # Data for GNU getopt_long
        file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/data/${set}.gnu.opt.txt" "${GNU_LINE}")
        file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/data/${set}.gnu.help.txt" "${GNU_HELP}")
        file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/data/${set}.gnu.check.txt" "${GNU_CHECK}")
    endforeach ()
endforeach ()

## Check for getopt(_long) ##
include(CheckIncludeFileCXX)
include(CheckCXXSymbolExists)

check_include_file_cxx("getopt.h" INFO_CLI_HAS_HEADER_GETOPT_H)
if (INFO_CLI_HAS_HEADER_GETOPT_H)
    check_cxx_symbol_exists(getopt "getopt.h" INFO_CLI_HAS_GETOPT)
    check_cxx_symbol_exists(getopt_long "getopt.h" INFO_CLI_HAS_GETOPT_LONG)
else ()
    set(INFO_CLI_HAS_GETOPT 0 CACHE INTERNAL "Whether we have getopt or not in getopt.h")
    set(INFO_CLI_HAS_GETOPT_LONG 0 CACHE INTERNAL "Whether we have getopt_long or not in getopt.h")
    mark_as_advanced(INFO_CLI_HAS_GETOPT)
    mark_as_advanced(INFO_CLI_HAS_GETOPT_LONG)
endif ()

configure_file(include/getopt-compat.hxx.in
               include/getopt-compat.hxx)

## Create targets
foreach (BENCHMARK_NUMBER IN LISTS CLI_BENCHMARK_SETS)
    message(STATUS "[InfoCLI>benchmark] Found set with size ${BENCHMARK_NUMBER}")

    configure_file(src/non-gnu.cxx.in
                   "non-gnu.${BENCHMARK_NUMBER}.cxx"
                   )

    configure_file(src/gnu.cxx.in
                   "gnu.${BENCHMARK_NUMBER}.cxx"
                   )

    add_executable("cli-bench-non-gnu-${BENCHMARK_NUMBER}"
                   "${CMAKE_CURRENT_BINARY_DIR}/non-gnu.${BENCHMARK_NUMBER}.cxx")

    add_executable("cli-bench-gnu-${BENCHMARK_NUMBER}"
                   "${CMAKE_CURRENT_BINARY_DIR}/gnu.${BENCHMARK_NUMBER}.cxx")

    target_link_libraries("cli-bench-non-gnu-${BENCHMARK_NUMBER}" PRIVATE
                          Boost::boost Boost::program_options
                          info::cli
                          cxxopts$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:::cxxopts>
                          Catch2::Catch2
                          )

    target_link_libraries("cli-bench-gnu-${BENCHMARK_NUMBER}" PRIVATE
                          Boost::boost Boost::program_options
                          info::cli
                          cxxopts$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:::cxxopts>
                          Catch2::Catch2
                          )

    target_include_directories("cli-bench-non-gnu-${BENCHMARK_NUMBER}" PRIVATE
                               "${CMAKE_CURRENT_SOURCE_DIR}/include"
                               "${CMAKE_CURRENT_BINARY_DIR}/include"
                               )

    target_include_directories("cli-bench-gnu-${BENCHMARK_NUMBER}" PRIVATE
                               "${CMAKE_CURRENT_SOURCE_DIR}/include"
                               "${CMAKE_CURRENT_BINARY_DIR}/include"
                               )

    add_custom_command(TARGET "cli-bench-non-gnu-${BENCHMARK_NUMBER}" POST_BUILD
                       COMMENT "Copying shared objects for 'cli-bench-non-gnu-${BENCHMARK_NUMBER}'"
                       COMMAND ${CMAKE_COMMAND} -E
                       copy $<TARGET_FILE:info::cli> "$<TARGET_FILE_DIR:cli-bench-non-gnu-${BENCHMARK_NUMBER}>"
                       )

    add_custom_command(TARGET "cli-bench-gnu-${BENCHMARK_NUMBER}" POST_BUILD
                       COMMENT "Copying shared objects for 'cli-bench-gnu-${BENCHMARK_NUMBER}'"
                       COMMAND ${CMAKE_COMMAND} -E
                       copy $<TARGET_FILE:info::cli> "$<TARGET_FILE_DIR:cli-bench-gnu-${BENCHMARK_NUMBER}>"
                       )

    if (LTO_SUPPORTED
        AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug"
        AND NOT (MINGW OR CYGWIN))
        message(STATUS "[InfoCLI>benchmark] Enabling LTO for \"cli-bench-non-gnu-${BENCHMARK_NUMBER}\"")
        set_target_properties("cli-bench-non-gnu-${BENCHMARK_NUMBER}"
                              PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
        message(STATUS "[InfoCLI>benchmark] Enabling LTO for \"cli-bench-gnu-${BENCHMARK_NUMBER}\"")
        set_target_properties("cli-bench-gnu-${BENCHMARK_NUMBER}"
                              PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif ()
endforeach ()


