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

## NameOption(OptionValue Options StringValue)
##
## Assigns a string value from the list of two elements passed as Options
## depending of the truthyness of the value passed as OptionValue
## and returns in through StringValue
function(NameOption ibOptionValue ilOptions ossStringValue)
    if (ibOptionValue)
        list(GET ilOptions 0 ssVal)
    else ()
        list(GET ilOptions 1 ssVal)
    endif ()
    set(${ossStringValue} ${ssVal} PARENT_SCOPE)
endfunction()

## Count(List Value oReturnVal)
##
## Counts how many occurrences of `Value` are in
## `List` and returns it through `oReturnVal`
function(Count List Value oReturnVal)
    set(_SUM 0)
    foreach (elem IN LISTS List)
        if ((Value STREQUAL elem)
             || (Value EQUAL elem))
            math(EXPR _SUM "${_SUM} + 1")
        endif ()
    endforeach ()
    set("${oReturnVal}" ${_SUM} PARENT_SCOPE)
endfunction()

## GetRepoTypeFromURL(RepoURL oRepoType)
##
## Deduces the VCS from the repository's clone URL
## Currently supported are Git and SVN
## Note that this is a crappy algorithm:
##  checks if the url ends in .git, in which case it is a Git repo
##  in any other case it is just SVN. Have fun
function(GetRepoTypeFromURL RepoURL oRepoType)
    string(LENGTH "${RepoURL}" RepoURL_LEN)
    math(EXPR RepoURL_GIT_BEGIN "${RepoURL_LEN} - 4")
    string(SUBSTRING "${RepoURL}" ${RepoURL_GIT_BEGIN} 4 RepoURL_LAST4)
    if (RepoURL_LAST4 STREQUAL ".git")
        set("${oRepoType}" GIT PARENT_SCOPE)
    else ()
        set("${oRepoType}" SVN PARENT_SCOPE)
    endif ()
endfunction()
