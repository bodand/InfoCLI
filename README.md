# InfoCLI

The new generation of the mess that was InfoParse. InfoCLI doesn't have completely
random and useless capabilities, like deleting the pointer returned by the
callback function, or rerunning the callback depending on its return value.
Why did I even have those, I don't know.

Also, InfoCLI does everything it can during compile-time, expect, of course, the
parsing.

The API is different, as now we have different design goals, but the main design 
decision is still the same: have it look like Perl's `Getopt::Long` as much as possible.  

## DISCLAIMER
If you have a lot of options to check, this library is not the best choice you can make.
While I worked very hard to not make it so, the compile-times are horrendous.
Up to 100 options is doable, as if you were compiling something using Boost.Spirit, 
200 is risque, and more is only advised if you can make absolutely sure, 
that one file that contains the cli_parser creation is only compiled *once*,
and only in really rare cases. 
Even then, use something else.
With 500 options I have around 1 hour long compilations for 1 file. You can try it
in the benchmarks if you don't believe me: pass `INFO_CLI_BUILD_BENCHMARKS` to cmake 
then build the target `cli-bench-500`.

Also, note that the compilers **will** take up RAM. Clang seems to use more on my
machine, but that may vary, as I'm running Windows & the MSYS2 MinGW install of both compilers.

## MSVC

MSVC does not support this GNU extension, nor anything equal:
```c++
template <class CharT, CharT...>
struct option_str {};

template <class CharT, CharT... cs>
constexpr option_str<char, cs...> operator""_opt() {
  return {};
}

"asd"_opt;
```
In turn, we respect its decision by not supporting MSVC.

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
using namespace info::cli::literals;

// project
#include <compiler/compiler.hpp>

int main(int argc, char** argv) {
    std::string exec_name;
    int optlvl;    

    auto cli = info::cli_parser::make(
        "o|output"_opt["The output executable's name"_hlp]->*exec_name,
        "O"_opt["The optimization's level"]->*optlvl      
    )();
    
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

For more usage examples and tutorial see the examples/ subdirectory.
