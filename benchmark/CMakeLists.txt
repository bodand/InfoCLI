#!&Copyright

cmake_minimum_required(VERSION 3.14)

project(InfoCLI_Benchmarks CXX)

include(BenchmarkUtils)

## Create benchmark datasets
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/data")
set(CLI_BENCHMARK_SETS 10 20 50 100 200 283 500)
foreach (set IN LISTS CLI_BENCHMARK_SETS)
    if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/data/${set}.input.txt")
        continue()
    endif ()

    foreach (n RANGE 1 ${set})
        InputData(${n} Input_LINE)
        BoostData(${n} Boost_LINE)
        CXXOptsData(${n} CXXOpts_LINE)
        InfoData(${n} Info_LINE)
        GnuData(${n} GNU_LINE GNU_HELP GNU_CHECK)

        # Input Data to parse against
        file(APPEND "${CMAKE_CURRENT_BINARY_DIR}/data/${set}.input.txt" "${Input_LINE}")
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

## Create targets
foreach (BENCHMARK_NUMBER IN LISTS CLI_BENCHMARK_SETS)
    message(STATUS "[InfoCLI>benchmark] Found set with size ${BENCHMARK_NUMBER}")

    configure_file(
            main.cpp.in
            "main.${BENCHMARK_NUMBER}.cpp"
    )

    add_executable("cli-bench-${BENCHMARK_NUMBER}"
                   "${CMAKE_CURRENT_BINARY_DIR}/main.${BENCHMARK_NUMBER}.cpp")

    target_link_libraries("cli-bench-${BENCHMARK_NUMBER}"
                          PUBLIC
                          Boost::boost Boost::program_options
                          info::cli
                          cxxopts
                          benchmark benchmark_main # benchmark::benchmark does not exist
                          )

    add_custom_command(TARGET "cli-bench-${BENCHMARK_NUMBER}" POST_BUILD
                       COMMENT "Copying shared objects for 'cli-bench-${BENCHMARK_NUMBER}'"
                       COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:info::cli> "${CMAKE_CURRENT_BINARY_DIR}"
                       COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:benchmark> "${CMAKE_CURRENT_BINARY_DIR}"
                       )

    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # GCC's LTO optimizes the benchmarks away
        set_target_properties("cli-bench-${BENCHMARK_NUMBER}"
                              PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif ()
endforeach ()


