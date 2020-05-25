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
