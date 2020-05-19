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
