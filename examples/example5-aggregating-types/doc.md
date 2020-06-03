# Example 5 - Aggregating types

## Tutorial

The library offers a special interface for the standard collections, when passed
as reference. The library calls these types aggregating types, which should
not be confused with aggregate types from The Standard.

Aggregating types have a special capability when passed as a reference to a variable,
to the cli_parser as a callback: they add to the stored values, and do not override
the current elements in the collection.
As an example if you have a `std::vector<int>` as a reference callback, any ints
encountered will be put at the end of the vector, instead of constructing a new
vector for each int an overwriting the vector consecutively.
 
This behavior is not present when passing them as a lambda that takes a concrete type.

## Example

Let's say we are writing a compiler and want to allow warnings to be enabled
one-by-one.

We will use the example from the repo's main README as a base. The compiler part 
is a mock implementation.

First, we need to store our warnings. Easy, just get a vector.

```c++
std::vector<std::string> warns;
``` 

Then we register a callback for `W` to add each option to our collection of vectors.
Also, a nice help message, just because we can.

```c++
"W"_opt["Warnings to be checked for during compilation"_hlp]->*warns
```

And we are done, now all '-W...' options will be put as a value into our warns 
vector.

The full code for wholesomeness:

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
    std::vector<std::string> warns;

    auto cli = info::cli_parser::make(
           "o|output"_opt["The output executable's name"_hlp]->*exec_name,
           "O"_opt["The optimization's level"]->*optlvl,
           "W"_opt["Warnings to be checked for during compilation"_hlp]->*warns
    )();

    std::vector<std::string_view> args;
    try {
        args = cli(argc, argv);
    } catch(const std::exception& ex) {
        std::cout << "encountered error while parsing input arguments: " << ex.what() << std::endl;
        return -1;
    }

    compiler comp{args};
    comp.compile(exec_name, warns, optlvl);

    return 0;
}
```

You can try out the world's first header-only compiler by building `cli-ex5`.
