# Example 1 - Basic usage

## Tutorial 

This example shows the basic usage of the library with simple callback values,
where you just use your existing variables and want the library to fill those
vars with the values it parses from the input.

The code is really easy to understand, thankfully to the DSL provided by the library.

First, or course, is to include the required headers. InfoCLI provides one
header which should be used when dealing with the library. All other headers 
are declared to be a part of the implementation and are not supposed to be depended on
and thus these may change without prior warning.
The stable header is `info/cli.hpp`.
To use the library one just has to include `info/cli.hpp`.

After one has included the InfoCLI header, you can start using the library, after just
one small step first. Because the library's DSL relies on UDL-s, they need
to be pulled into the user's namespace. If you, for understandable reasons, do not want
to `using namespace info::cli`, the library allows the usage of `using namespace info::cli::literals`,
which only bring the two literals the DSL requires, and noting more.

The only object you need to directly instantiate here is only one object: `info::cli::cli_parser`.
It has a template constructor to create the required options it needs to parse, and is used thusly:
```c++
#include <info/cli.hpp>
using namespace info::cli::literals;

int main(int argc, char** argv) {
    info::cli::cli_parser cli{
        /* options */
    };
}
```
Do note that an empty options list like that will trigger a static_assert and fail compilation.

The options to be passed to the `cli_parser` are where the DSL of InfoCLI comes into play:
using this syntax you can define the options you need to match:
`"<All options you want to match for this one callback separated by pipes>"_opt->*<variable to callback into>`.
The option names are transformed at compile-time into a list (tuple) of all strings you need to match
with a prepended dash depending on the length of the option: short POSIX-style options are prepended
with one dash (eg. "d" -> "-d"), while long options are prepended with two dashes (eg. "opt" -> "--opt").

## Example

This example contains a file copier utility. The interface is a bit uncomfortable 
to use, but it showcases how the library can be used, so that's good enough for us here.
 
So say you want to have a program that takes two possible options: one for the input file,
and one for the output file. Each option should have a short and a long option which
will call it.

First, let's take our starter structure as presented earlier in the turorial part.  

```c++
#include <info/cli.hpp>
using namespace info::cli::literals;

int main(int argc, char** argv) {
    info::cli::cli_parser cli{
        /* options */
    };
}
```

First, we need to define our options: 
for the input file we will allow `-i` and `--input-file`, while for the output
we will allow `-o` and `--output-file`. 
Also, we want each of these options to fill a string value for us. For this we
need two string variables, `std::string`, of course: `inp`, and `outp`.

After we know what we want the library to do for use, we can type it down:

```c++
// stdlib
#include <string>

// InfoCLI
#include <info/cli.hpp>
using namespace info::cli::literals;

int main(int argc, char** argv) {
    std::string inp;
    std::string outp;

    info::cli::cli_parser cli{
        "i|input-file"_opt->*inp,
        "o|output-file"_opt->*outp
    };
} 
```

Now, the library will nicely construct the cli_parse we want it to construct.
Now the only thing we need is to call the constructed parser to actually do the parsing:

```c++
// stdlib
#include <iostream>
#include <string>

// InfoCLI
#include <info/cli.hpp>
using namespace info::cli::literals;

int main(int argc, char** argv) {
    std::string inp;
    std::string outp;

    info::cli::cli_parser cli{
        "i|input-file"_opt->*inp,
        "o|output-file"_opt->*outp
    };
    
    std::vector<std::string_view> rem;
    try {
        rem = cli(argc, argv);
    } catch (std::exception& ex) {
        std::cerr << "Unable to parse provided arguments: " << ex.what();
        return -1;
    }

    if (inp.empty()) {
        std::cerr << "The input file must be set.";
        return 1;
    }
    if (outp.empty()) {
        std::cerr << "The output file must be set.";
        return 2;
    }
} 
```

By default, the call to the parser can throw, so we go and wrap the parser invocation
in a try-catch, and tell the user that something broke and exit with an error code of `-1`.

The `rem` vector is used to store all arguments passed to our executable which were not
part of the options to parse. If we do not store these value the compiler will warn us,
so we just store them here, even though in this simplistic example we don't care about them.
(Yes, we could have just cast the thing to void, but this shows what type the parser returns.)

Now the parts related to the argument parsing is complete, now we just need to implement
the file copying operation, for which we will just rely on `std::filesystem`, because 
we don't really care as that is not the point of the example.

With that, we have the full code in our `main.cpp` (excluding the license notice):

```c++
// stdlib
#include <iostream>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;

// InfoCLI
#include <info/cli.hpp>
using namespace info::cli::literals;

int main(int argc, char** argv) {
    std::string inp;
    std::string outp;

    // set-up parser
    info::cli::cli_parser cli{
        "i|input-file"_opt->*inp,
        "o|output-file"_opt->*outp
    };
    
    // perform parsing
    std::vector<std::string_view> rem;
    try {
        rem = cli(argc, argv);
    } catch (std::exception& ex) {
        std::cerr << "Unable to parse provided arguments: " << ex.what();
        return -1;
    }
 
    // check required values to have been set
    if (inp.empty()) {
        std::cerr << "The input file must be set.";
        return 1;
    }
    if (outp.empty()) {
        std::cerr << "The output file must be set.";
        return 2;
    }

    // perform copy
    try {
        fs::copy(inp, outp);
    } catch (std::exception& ex) {
        std::cerr << "Error occurred while copying: " << ex.what();
        return -2;
    }
} 
```  

If you build it, using `cmake --build <build-path> --target cli-ex1` you can try using the 
executable. Do note that, by default, it depends on the shared object `icli.dll`/`icli.so`
in the path: to build a static library use `-DINFO_CLI_BUILD_SHARED=Off` with the cmake invocation.
