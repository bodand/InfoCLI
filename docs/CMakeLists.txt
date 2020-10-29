## InfoCLI project
# Copyright (c) 2020 bodand
# Licensed under the BSD 3-Clause license
#
# The CMake script that manages the building of the documentation and the
# examples the tutorial contains.

## Get Doxygen
find_package(Doxygen REQUIRED)

## Get files in documentations
get_target_property(_INFO_CLI_INCLUDE_FILES info::cli INCLUDE_FILES)
get_target_property(_INFO_CLI_SOURCE_EXT info::cli SOURCE_EXTENSION)
get_target_property(_INFO_CLI_HEADER_EXT info::cli HEADER_EXTENSION)

## Set Doxygen settings
#&!off
set(DOXYGEN_OUTPUT_DIRECTORY      "${CMAKE_CURRENT_BINARY_DIR}/docs")
set(DOXYGEN_REPEAT_BRIEF          NO)
set(DOXYGEN_PYTHON_DOCSTRING      NO)
set(DOXYGEN_BUILTIN_STL_SUPPORT   YES)
set(DOXYGEN_NUM_PROC_THREADS      0)
set(DOXYGEN_ALPHABETICAL_INDEX    NO)
set(DOXYGEN_EXTRACT_ALL           YES)
set(DOXYGEN_EXTRACT_LOCAL_CLASSES NO)
set(DOXYGEN_PREDEFINED            "INFO_CLI_DOXYGEN_RUN")
set(DOXYGEN_FILE_PATTERNS         "*.${_INFO_CLI_SOURCE_EXT}"
                                  "*.${_INFO_CLI_HEADER_EXT}"
                                  "*.dox")
set(DOXYGEN_STRIP_FROM_PATH       "${CMAKE_SOURCE_DIR}")
set(DOXYGEN_SOURCE_BROWSER        YES)
set(DOXYGEN_GENERATE_HTML         YES)
set(DOXYGEN_HTML_HEADER           "${CMAKE_CURRENT_SOURCE_DIR}/theme/header.html")
set(DOXYGEN_HTML_FOOTER           "${CMAKE_CURRENT_SOURCE_DIR}/theme/footer.html")
set(DOXYGEN_HTML_EXTRA_STYLESHEET "${CMAKE_CURRENT_SOURCE_DIR}/theme/custom.css"
                                  "${CMAKE_CURRENT_SOURCE_DIR}/theme/dark.css")
set(DOXYGEN_HTML_TIMESTAMP        YES)
set(DOXYGEN_GENERATE_MAN          YES)
set(DOXYGEN_MAN_EXTENSION         ".3info")
set(DOXYGEN_MAN_LINKS             YES)
set(DOXYGEN_DOT_MULTI_TARGETS     NO)
#&!on

## Tutorial Targets
foreach (x RANGE 4)
    DefineNewTutorialTarget(${x})
    list(APPEND _INFO_CLI_TUTORIAL_FILES "src/Tutorial_${x}.dox")
endforeach ()

## Generate doxyfile
doxygen_add_docs(cli-docs
                 ${_INFO_CLI_INCLUDE_FILES}
                 src/Installation.dox
                 src/Manual.dox
                 ${_INFO_CLI_TUTORIAL_FILES}
                 COMMENT "Generating Doxygen documentation"
                 USE_STAMP_FILE
                 )
