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

include(GetGit)
include(GetSVN)

include(Utils)

# Checks the system for all a dependency and if not found,
# will install them for the project
# Supported repositories are 'Git' and 'SVN' as deduced from the URL
# GetDependency(
#    <DEPENDENCY_TO_SEARCH>
#    REPOSITORY_URL <URL_OF_<GIT|SVN>_REPOSITORY>
#    VERSION <GIT_TAG|SVN_REVISION>
#    [COMPONENTS <LIST_OF_COMPONENTS...>]
#    [FALLBACK <DEPENDENCY_LIBRARY_NAME>
#    [FALLBACK_COMPONENTS <LIST_OF_COMPONENTS_OF_FALLBACK_DEPENDENCY...>]]
# )
function(GetDependency)
    ## Helpers
    macro(EMIT_ERROR MSG)
        message(FATAL_ERROR "GetDependency was not passed the appropriate arguments:\
        ${MSG}")
    endmacro()
    set(REPO_GIT_NAME GIT)
    set(REPO_GIT_REVISION GIT_TAG)
    set(REPO_SVN_NAME SVN)
    set(REPO_SVN_REVISION SVN_REVISION)

    ## ARGUMENT HANDLING BEGIN #################################################
    cmake_parse_arguments(PARSE_ARGV 0 GET_DEP
                          ""
                          "REPOSITORY_URL;VERSION;FALLBACK"
                          "COMPONENTS;FALLBACK_COMPONENTS"
                          )

    ## Check incomplete arguments
    if (GET_DEP_KEYWORDS_MISSING_VALUES)
        EMIT_ERROR("Values for the option(s): '${GEP_DEP_KEYWORDS_MISSING_VALUES}' were not defined.")
    endif ()

    ## Library
    list(LENGTH GET_DEP_UNPARSED_ARGUMENTS GET_DEP_UNPARSED_ARGUMENTS_LEN)
    if (GET_DEP_UNPARSED_ARGUMENTS_LEN EQUAL 0)
        EMIT_ERROR("Dependency name to check/install must be provided")
    elseif (GET_DEP_UNPARSED_ARGUMENTS_LEN GREATER 1)
        EMIT_ERROR("Dependency name to check/install must be specified exactly once")
    endif ()
    list(GET GET_DEP_UNPARSED_ARGUMENTS 0 Library)

    ## Components
    if (DEFINED GET_DEP_COMPONENTS)
        set(Components ${GET_DEP_COMPONENTS})
    else ()
        set(Components)
    endif ()

    ## HasFallback / FallbackLibrary
    if (DEFINED GET_DEP_FALLBACK)
        set(HasFallback TRUE)
        set(FallbackLibrary ${GET_DEP_FALLBACK})
    else ()
        set(HasFallback FALSE)
        set(FallbackLibrary ${GET_DEP_FALLBACK})
    endif ()

    ## FallbackComponents
    if (DEFINED GET_DEP_FALLBACK_COMPONENTS)
        if (NOT HasFallback)
            EMIT_ERROR("FALLBACK_COMPONENTS must only be specified if FALLBACK is specified")
        endif ()
        set(FallbackComponents ${GET_DEP_FALLBACK_COMPONENTS})
    else ()
        set(FallbackComponents)
    endif ()

    ## URL
    if (NOT DEFINED GET_DEP_REPOSITORY_URL)
        EMIT_ERROR("REPOSITORY_URL must be passed")
    endif ()
    set(URL "${GET_DEP_REPOSITORY_URL}")

    ## RepoType/RepoTagType
    GetRepoTypeFromURL("${URL}" DEDUCED_REPO_TYPE)
    set(RepoType "${REPO_${DEDUCED_REPO_TYPE}_NAME}")
    set(RepoTagType "${REPO_${DEDUCED_REPO_TYPE}_REVISION}")

    ## Version
    if (NOT DEFINED GET_DEP_VERSION)
        EMIT_ERROR("VERSION must be passed")
    endif ()
    set(Version "${GET_DEP_VERSION}")

    ##### Variables set
    # # All list vars have a <LIST_NAME>_LEN set about their length
    #
    # Library            : The main library to check
    # Components         : The components to search for with Library [LIST]
    # HasFallback        : Whether fallback's provided
    # FallbackLibrary    : The fallback library to check
    # FallbackComponents : The components to search for with FallbackLib [LIST]
    # RepoType           : Type of the repository - GIT or SVN
    # RepoTagType        : Type of the repository's versioning thing: GIT_TAG/SVN_REVISION
    # URL                : The git/SVN repo URL to install from if not found
    # Version            : The git tag/SVN revision to install if not found
    ## ARGUMENT HANDLING END ###################################################

    ## Main library
    if (Components_LEN EQUAL 0)
        find_package("${Library}" QUIET)
    else ()
        find_package("${Library}" COMPONENTS ${Components} QUIET)
    endif ()

    if ("${${Library}_FOUND}")
        message(STATUS "Loaded dependency from system: '${Library}'")
        set("${Library}_LINK_AS" ${Library})
        return()
    endif ()

    ## Fallback library
    if (HasFallback)
        if (FallbackComponents_LEN EQUAL 0)
            find_package("${FallbackLibrary}" QUIET)
        else ()
            find_package("${FallbackLibrary}" COMPONENTS ${FallbackComponents} QUIET)
        endif ()

        if ("${${FallbackLibrary}_FOUND}")
            message(STATUS "Loaded fallback dependency from system: '${FallbackLibrary}' "
                    "for dependency: '${Library}'")
            set("${Library}_LINK_AS" ${FallbackLibrary})
            return()
        endif ()
    endif ()

    ## Install
    message(STATUS "Could not load dependency from system: '${Library}' - Installing")
    FetchContent_Declare(
            "${Library}"
            "${RepoType}_REPOSITORY" "${URL}"
            "${RepoTagType}" "${Version}"
    )
    FetchContent_MakeAvailable("${Library}")
endfunction()
