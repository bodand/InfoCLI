## BSD 3-Clause License
#
# Copyright (c) 2020, 2023, bodand
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

cmake_minimum_required(VERSION 3.16)
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
enable_language(CXX)

### Policies ###
cmake_policy(SET CMP0048 NEW) # project sets VERSION vars
cmake_policy(SET CMP0091 NEW) # MSVC does not set the runtime library in CMAKE_<LANG>_FLAGS
cmake_policy(SET CMP0092 NEW) # MSVC does not set the default /W3 in CMAKE_<LANG>_FLAGS
cmake_policy(SET CMP0009 NEW) # Do not follow symlinks in GLOB_RECURSE

### Custom Properties ###
define_property(TARGET
                PROPERTY SOURCE_EXTENSION
                BRIEF_DOCS "The file extension of source files."
                FULL_DOCS [[The file extension used by the target for source
                            files. eg. cpp or cxx]]
                )

define_property(TARGET
                PROPERTY HEADER_EXTENSION
                BRIEF_DOCS "The file extension of header files."
                FULL_DOCS [[The file extension used by the target for header
                            files. eg. hpp or hxx]]
                )

define_property(TARGET
                PROPERTY INCLUDE_FILES
                BRIEF_DOCS "The list of include files for the target"
                FULL_DOCS [[The list of the absolute paths of the files that are
                            used in the target as included files. Basically the
                            list of files used found in the include directories
                            whoso extensions match the target's HEADER_EXTENSION
                            property.]]
                )

### Modules ###
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(Utils)

### InfoCLI Project ###
project(InfoCLI
        VERSION 2.2.0
        LANGUAGES CXX
        )

### Options ###
option(INFO_CLI_BUILD_DOCS
       "Generate the full InfoCLI HTML documentation [Off]"
       Off
       )

option(INFO_CLI_BUILD_TESTS
       "Build the unit test suite for InfoCLI [On]"
       On
       )

option(INFO_CLI_BUILD_BENCHMARKS
       "Build the benchmarks for InfoCLI's option parsing. [Off]"
       Off
       )

option(INFO_CLI_BUILD_STATIC
       "Build a shared library for InfoCLI instead of a static one. [Off except MSVC]"
       "${MSVC}"
       )
NameOption("${INFO_CLI_BUILD_STATIC}" "STATIC;SHARED" INFO_CLI_BUILD_TYPE)
message(STATUS "[${PROJECT_NAME}] Building ${INFO_CLI_BUILD_TYPE} library")

### Use C++17 ###
set(CMAKE_CXX_STANDARD 17)
add_library(cli_cxx_std INTERFACE)
add_library(InfoCLI::cxx_std ALIAS cli_cxx_std)

target_compile_features(cli_cxx_std INTERFACE
                        "cxx_std_17"
                        )

### Dependencies ###
find_package(InfoUtils CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

### Warnings ###
include(Warnings)

## clang-cl support ##
if ("x${CMAKE_CXX_COMPILER_FRONTEND_VARIANT}" STREQUAL "xMSVC")
    message(STATUS "Injecting /EHsc into clang-cl command-line")
    add_compile_options(/EHsc)
endif ()

### info::cli ###
add_library(cli "${INFO_CLI_BUILD_TYPE}"
            src/cli_parser.cxx
            src/option.cxx
            src/impl/option_builder.cxx
            src/impl/helpful_option_builder.cxx
            src/types/type_data.cxx
            src/types/type_parser.cxx
            src/exc/no_such_option.cxx
            src/exc/callback_error.cxx
            src/exc/bad_option_value.cxx)
add_library(info::cli ALIAS cli)

set(_CLI_SRC_EXT "cxx")
set(_CLI_HDR_EXT "hxx")
file(GLOB_RECURSE _CLI_INCLUDE_FILES
     LIST_DIRECTORIES false
     CONFIGURE_DEPENDS
     "${CMAKE_SOURCE_DIR}/include/*.${_CLI_HDR_EXT}"
     )

set_target_properties(cli PROPERTIES
                      OUTPUT_NAME "info_cli$<$<CONFIG:Debug>:d>"
                      SOURCE_EXTENSION "${_CLI_SRC_EXT}"
                      HEADER_EXTENSION "${_CLI_HDR_EXT}"
                      INCLUDE_FILES "${_CLI_INCLUDE_FILES}"
                      )


target_link_libraries(cli PUBLIC
                      InfoCLI::cxx_std
                      InfoCLI::Warnings
                      fmt::fmt
                      info::utils
                      )

target_include_directories(cli PUBLIC
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:include>
                           )

target_compile_options(cli PUBLIC
                       $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>
                       )

target_compile_definitions(cli PUBLIC
                           -DINFO_CLI_BUILD_STATIC=$<BOOL:${INFO_CLI_BUILD_STATIC}>
                           -DINFO_CLI_EXPORTS=$<BOOL:$<BUILD_INTERFACE:1>>
                           )

### Debug ###
if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_target_properties(cli PROPERTIES
                          CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG -pthread -Og")
endif ()

## Docs ##
if (INFO_CLI_BUILD_DOCS)
    add_subdirectory(docs)
endif ()

# Benchmarks ##
if (INFO_CLI_BUILD_BENCHMARKS)
    add_subdirectory(benchmark)
endif ()

## Tests ##
if (INFO_CLI_BUILD_TESTS)
    enable_testing()
    include(CTest)

    add_subdirectory(test)
endif ()

## Install
install(TARGETS cli cli_cxx_std
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}/include/info"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        FILES_MATCHING PATTERN "*.hxx"
        )
write_basic_package_version_file(
        ${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE info::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
        )

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
        "${PROJECT_NAME}Config.cmake"
        @ONLY)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
        )
