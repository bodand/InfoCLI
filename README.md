# InfoCLI v2

InfoCLI v2 is the rethought version of InfoCLI. It 

## Compatibility

The code's only been tested on Windows at the moment, so it's advised to check
on your OS by building and running the tests. If an error comes up, please throw
me an issue, so I at least know there are problems and don't have to live blissfully
ignorant of them.

### MSVC

While in theory MSVC can fully build InfoCLI, building a DLL *is not* actively tested
because I'm kind of lost on how the Microsoft ABI works, and how MSVC deals with 
DLLs with those imports and exports, mostly because there are C++ stdlib components
that may cross DLL boundaries.
Constructing static libraries (`cli.lib`) is completely fine, and *is* actively 
tested.

### MingGW-W64 GCC

MinGW-W64 GCC works well, and building DLLs is completely fine, the not here
is that using by self-built LLVM LLD (from trunk) 12.0.0 causes linking errors
during the creation of the DLL. The easiest workaround is to use the MinGW-W64 
ld that comes with GCC.
I do not know if this problem is present when using GCC on an actual unix-like
system with lld.

## Usage

The most basic example is as follows, where a basic compiler is modelled.
It takes `-o` or `--output` to specify how to call the output, and `-O` to
specify optimization level. 
The actual compiler parts are omitted, of course.

```c++
// stdlib
#include <string>
#include <exception>
#include <iostream>

// info::cli
#include <info/cli.hpp>
using namespace info::cli::udl;

// project
#include <compiler/compiler.hpp>

int main(int argc, char** argv) {
    std::string exec_name;
    int optlvl;    

    info::cli::cli_parser cli{
        'o' / "output"_opt >= "The output executable's name" >>= exec_name,
        'O'_opt >= "The optimization's level" >>= optlvl      
    };
    
    std::vector<std::string_view> args;
    try {
        args = cli(argc, argv);
    } catch(const std::exception& ex) {
        std::cout << "encountered error while parsing input arguments: " << ex.what() << std::endl;
        return -1;
    }
    
    compiler comp{args};
    comp.compile(exec_name, optlvl);
    
    return 0;
}
```

For the complete documentation and user guide the `docs/` directory contains
multiple examples and a using `INFO_CLI_BUILD_DOCS` creates a complete doxygen
documentation for the project.

## Benchmarks 

When configured with `INFO_CLI_BUILD_BENCHMARKS` the library builds the `benchmark/`
directory/subproject, however it requires additional dependencies: 
 - `Boost` for the `Boost.ProgramOptions` module, which cannot be installed stand-alone,
   thus the whole charade is required. This requires the user to install it:
   InfoCLI does not attempt to auto-install. 
 - `cxxopts` the other commonly used CLI parsing library. This is header only,
   and is CMake based, therefore, if not found, InfoCLI will install it locally
   for the project.
   
Benchmarking is done with the Catch2 test library's benchmark capabilities, so
no other libraries are required.

After configuring the `cli-bench-${numer of options parsed}` targets can be used 
to run the benchmarks. For the available targets, see the output of CMake.
