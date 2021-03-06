function(InputData_NonGNU N oLine)
    set(${oLine} "\"--option-${N}\", \"666\", " PARENT_SCOPE)
endfunction()

function(InputData_GNU N oLine)
    set(${oLine} "\"--option-${N}=666\", " PARENT_SCOPE)
endfunction()

function(BoostData N oLine)
    set(${oLine} "(\"option-${N}\", po::value<int>(&a<${N}>), \"some help -- ${N}\")" PARENT_SCOPE)
endfunction()

function(CXXOptsData N oLine)
    set(${oLine} "(\"option-${N}\", \"some help -- ${N}\", cxo::value<int>(a<${N}>))" PARENT_SCOPE)
endfunction()

function(InfoData N oLine)
    set(${oLine} "\"option-${N}\"_opt >= \"some help -- ${N}\" >>= a<${N}>," PARENT_SCOPE)
endfunction()

function(GnuData N oLine oHelp oCheck)
    set(${oLine} "{\"option-${N}\", required_argument, 0, 0}," PARENT_SCOPE)
    set(${oHelp} "\"some help -- ${N}\"," PARENT_SCOPE)
    set(${oCheck} "if (strcmp(long_opts[opt_idx].name, \"option-${N}\") == 0) {a<${N}> = atoi(optarg);}" PARENT_SCOPE)
endfunction()
