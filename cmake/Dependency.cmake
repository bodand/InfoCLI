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
include(GetURL)

# Checks the system for all a dependency and if not found,
# will install them for the project
function(GetDependency
         Names    # Names to search for
         RepoType # Either: Git, SVN, or URL
         URL      # The URL to the source repo/file
         Version  # The git tag/svn revision to use, or the tarball's hash
         )
    # Search the system
    foreach (name IN LISTS Names)
        find_package(${name} QUIET)
        if (${name}_FOUND)
            message(STATUS "Requested dependency '${name}' found")
            set(GD_LAST_FOUND ${name} PARENT_SCOPE)
            return()
        endif ()
    endforeach ()

    # Install dependency
    list(GET Names 0 DependencyName)
    message(STATUS "Installing dependency '${DependencyName}'")

    if (RepoType STREQUAL "Git")
        PullGitDependency(${DependencyName} ${URL} ${Version})
    elseif (RepoType STREQUAL "SVN")
        PullSVNDependency(${DependencyName} ${URL} ${Version})
    elseif (RepoType STREQUAL "URL")
        PullFileDependency(${DependencyName} ${URL} ${Version})
    else ()
        message(FATAL_ERROR "GetDependency got unknown dependency type: ${RepoType}
The following are supported: Git, SVN, URL")
    endif ()
endfunction()
