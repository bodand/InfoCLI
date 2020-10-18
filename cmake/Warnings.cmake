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

include(CheckCXXCompilerFlag)

## Compiler warnings
function(CheckWarningFlag OptionName CacheName)
    if (OptionName MATCHES [[^/]]) # MSVC-style args are passed as-is
        set(WarningPrefix "")
    else ()
        set(WarningPrefix "-W")
    endif ()
    check_cxx_compiler_flag("${WarningPrefix}${OptionName}" "HasWarning_${CacheName}")
    set("HAS_WARNING_${CacheName}" ${HasWarning_${CacheName}} PARENT_SCOPE)
endfunction()

set(_INFO_CLI_POSSIBLE_WARNINGS
    # default enables
    extra pedantic sign-compare error=uninitialized unused cast-qual cast-align
    abstract-vbase-init array-bounds-pointer-arithmetic assign-enum consumed
    conditional-uninitialized deprecated-implementations header-hygiene error=move
    error=documentation-deprecated-sync error=non-virtual-dtor error=infinite-recursion
    keyword-macro loop-analysis newline-eof over-aligned redundant-parens
    reserved-id-macro sign-conversion signed-enum-bitfield thread-safety
    undefined-internal-type undefined-reinterpret-cast unused-const-variable
    unneeded-internal-declaration unreachable-code-aggressive unused-variable
    unused-exception-parameter unused-parameter unused-template error=lifetime
    error=sometimes-uninitialized tautological-overlap-compare suggest-final-types
    nullability-completeness unreachable-code-loop-increment redundant-decls
    suggest-attribute=pure suggest-attribute=const suggest-attribute=cold
    suggest-final-methods duplicated-branches placement-new=2 error=trampolines
    logical-op reorder
    /w14062 /w14165 /w14191 /w14242 /we4263 /w14265 /w14287 /w14296 /we4350 /we4355
    /w14355 /w14471 /we4545 /w14546 /w14547 /w14548 /w14549 /w14557 /we4596 /w14605
    /w14668 /w14768 /w14822 /we4837 /we4928 /we4946 /we4986 /w15032 /w15039 /wd4010
    3
    /diagnostics:caret
    # default disables
    no-unknown-pragmas no-unused-macros no-nullability-extension
    no-c++20-extensions no-comment
    # extra warnings
    ${INFO_EXTRA_WARNINGS}
    ${INFO_CLI_EXTRA_WARNINGS}
    )

add_library(Warnings INTERFACE)
add_library(InfoCLI::Warnings ALIAS Warnings)

target_compile_options(Warnings INTERFACE
                       $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall>
                       )

foreach (warn IN LISTS _INFO_CLI_POSSIBLE_WARNINGS)
    string(MAKE_C_IDENTIFIER "${warn}" cwarn)
    CheckWarningFlag("${warn}" "${cwarn}")
    if (HAS_WARNING_${cwarn})
        if (warn MATCHES [[^/]]) # MSVC-style args are passed as-is
            set(WarningPrefix "")
        else ()
            set(WarningPrefix "-W")
        endif ()
        target_compile_options(Warnings INTERFACE "${WarningPrefix}${warn}")
    endif ()
endforeach ()

## Install targets
install(TARGETS Warnings
        EXPORT InfoCLI_Targets
        )
