# Example 3 - Callback functions

## Tutorial

I'm sure you have been trying to ask me: "Can we only have variables as callbacks? 
That doesn't seem that flexible." Here I answer: "Of course not, that'd be boring.",
so callbacks can be any callable, including generic lambdas and functors, although those
have a bit of a caveat.

So, what's the syntax? We are not adding even more to our rusty friend, however, as you 
might have noticed in the last example's tutorial the callbacks were simply referred to as
`<callback>` instead of `<callback variable>` which was used in example 1.
You might have already drawn the conclusion: just use the functor you want to be called
instead of a variable. That's it.
For example if I want to print out every input I can do the following:
```c++
info::cli::cli_parser cli{
    "p|print"_opt->*[](auto inp) {
        std::cout << "given: " << inp;
    }
};
```
 
The requirements of the parameters of the callback: 
 - it needs to be callable with 1 argument
 - if the callback is generic: it will be given a `std::string_view` to do whatever it wants
 - if the callback defines what type it needs: it will be formatted as such and passed as the requested object
 
Now the next question: "How does formatting work?"
Bool is false if the argument is `0`, `""`, `"false"`, or `"off"`. Any other value for a bool
type option is true.
Currently, the library specializes arithmetic types and uses the appropriate stdlib function
to format them the most performant way possible, eg. `<charconv>`. 
As of now, for all other types the library uses the stream interface to format arguments from
a string, thus if you have a type which overloads `operator>>` for `std::istream&` you are good
to go. More about custom types in the next example.

## Example

We will now add a `v|version` option which will print `3\n` to the user then exit the program.

Again, we only need to modify the `cli_parser` construction, so let's just do that:

```c++
// set-up parser
info::cli::cli_parser cli{
       "i|input-file"_opt["The file to copy from"_hlp]->*inp,
       "o|output-file"_opt["The file to copy to"_hlp]->*outp,

       "v|version"_opt["Prints the version of the utility"_hlp]->*[](bool) {
         std::cout << '3' << std::endl;
         std::exit(1);
       }
};
```

And again, we are done. The library does not support no-argument callbacks,
so we use `bool`, which will tell us whether the argument was called with true or false
value. `--version=0` would give false for example, but we ignore the value, and just
print and exit. If we wanted to we could've used this lambda instead which will only
print and return if the given value is true:
```c++
[](bool val) {
  if (val) {
      std::cout << '3' << std::endl;
      std::exit(1);
  }
}
```

For the sake of completeness, again, here is our full code:

```c++
// stdlib
#include <iostream>
#include <string>
#include <cstdlib>
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
           "i|input-file"_opt["The file to copy from"_hlp]->*inp,
           "o|output-file"_opt["The file to copy to"_hlp]->*outp,

           "v|version"_opt["Prints the version of the utility"_hlp]->*[](bool) {
             std::cout << '3' << std::endl;
             std::exit(1);
           }
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
        return 255;
    }
    if (outp.empty()) {
        std::cerr << "The output file must be set.";
        return 254;
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

If you build this as `cli-ex3` you can try out your new and fancy callback.
