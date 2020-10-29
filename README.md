# InfoCLI v2

InfoCLI v2 is the rethought version of InfoCLI. Callback remade the DSL used to declare
options, which allows it to do away with compile-time strings completely: this 
obviously means terrific improvements to compilation times, and the biggest
problem of InfoCLI v1.x is fixed.

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

## Usage

The most basic example is as follows, where a basic compiler is modelled.
Callback takes `-o` or `--output` to specify how to call the output, and `-O` to
specify optimization level. 
The actual compiler parts are omitted, of course.

```c++
// stdlib
#include <string>
#include <exception>
#include <iostream>

// info::cli
#include <info/cli.hxx>
using namespace info::cli::udl;

// project
#include <compiler/compiler.hpp>

int main(int argc, char** argv) {
    std::string exec_name = "a.out";
    int optlvl = 0;

    info::cli::cli_parser cli{
        'o' / "output"_opt >= "The output executable's name" >>= exec_name,
        'O'_opt            >= "The optimization level"       >>= optlvl
    };
    
    std::vector<std::string_view> args;
    try {
        args = cli(argc, argv);
    } catch(const std::exception& ex) {
        std::cerr << "encountered error while parsing input arguments: " << ex.what() << std::endl;
        return -1;
    }
    
    compiler comp{args};
    comp.compile(exec_name, optlvl);
}
```

For the complete documentation and user guide the `docs/` directory contains
multiple examples and a using `INFO_CLI_BUILD_DOCS` creates a complete doxygen
documentation for the project... After it is done, of course.

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

After configuring the `cli-bench-${option type}-${numer of options parsed}` targets 
can be used to run the benchmarks. For the available targets, see the output of CMake.

## Licenses

The InfoCLI project is available under the BSD 3-Clause license. For more 
information see the provided `LICENSE` file.

InfoCLI uses the following libraries as dependencies under the listed licenses:
 - `InfoUtils`: BSD 3-Clause (`LICENSE`)
 - `{fmt}`    : The fmtlib license (`LICENSE.fmtlib`)
 - `Catch2`   : Boost Software License v1.0 (`LICENSE.bsl`)

The documentation uses Doxygen. For the generated HTML documentation, a dark
theme is used from https://github.com/MaJerle/doxygen-dark-theme. It is available
under the MIT License: see `LICENSE.doxygen-dark-theme`. The actual theme
is modified to make the `note` paragraphs look like they do now.
