# Example 2 - Auto-help

## Tutorial

This is a short part. The library is capable of automatically generate a `h|help`
option to print usage information to the user. However, this does not know what
messages should it tell the user, so it requires the library's user to write the help
message strings.

If the library's user decides not to write any help messages for the options, the
library will not generate any special option, as you can see with the previous example.
If you were to try using `--help` with `cli-ex1` it'd error out and tell you that that option
does not exist. 

However, if you provide a help message to at least one of your options,
all options with a help message will be printed. If an option does not define
a help string, it will be omitted from the message.

Providing a help string is one operator away from you, using this syntax:
`"<opts...>"_opt["<help message you'd like to print to user>"_hlp]->*<callback>`

With that done, we can now understand the example.

## Example

This is just one change from our last example. We set the two options, -i and -o, 
to have the help messages `The file to copy from` and `The file to copy to` respectively.

```c++
// set-up parser
auto cli = info::cli::cli_parser::make(
       "i|input-file"_opt["The file to copy from"_hlp]->*inp,
       "o|output-file"_opt["The file to copy to"_hlp]->*outp
)();
```

With that our example is completed, as the auto-help will generate everything 
that needs to be generated to provide the `-h` and `--help` options. 
Do note that providing an option which matches on `h|help` will override the default
generated help message.
While the library does not warn for this yet, as I'm still trying to figure out how
to generate a compile-time warning for such behavior, it is planned to happen.

Just for completeness the whole source without the copyright notice is as such:

```c++
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
    auto cli = info::cli::cli_parser::make(
           "i|input-file"_opt["The file to copy from"_hlp]->*inp,
           "o|output-file"_opt["The file to copy to"_hlp]->*outp
    )();

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
 
This can be built as the target `cli-ex2`.
