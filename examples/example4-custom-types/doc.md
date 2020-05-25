# Example 4 - Custom types

## Tutorial

You might want to have a custom type input from the command line. While you could
implement this with a lambda-callback like this:
```c++
[&](auto str) {
  my_var = my_type::from_str(str);  
}
```
Where you either implement a separate from_str or just implement the whole function inside
the lambda.

This, however, is pretty meaningless work. Why write lambdas, if you can just rely
on the library to call the functions/run the code you want?

The only thing to do is specialize the parser object. How? Read on.

The most important here is: `info::cli::type_parser<T>`. By default, the unspecialized
version calls `operator>>` on your type. This, however, might not be desirable:
it requires the type to be default-initialisable, and is, in general, pretty slow.
And, of course, requires your type to overload `operator>>`.

To fix all these problems the specialization needs to implement an `operator()` which
takes a string, and returns an `info::expected<T, std::string_view>`. Then the library
calls the most-specialized version, which is most likely the one you provided.

This allows you to execute arbitrary code when creating the instances of your type.

For more information about `type_parser`, and specialization in the library in general, 
see the `docs/SpecializationEndpoints.md` file.

## Example

Preface: Sorry, I can't think of a way to make this meaningful for our little copier, so
just bear with the example.

Let's say you want an option which, if defined, can never be set to the string
`not-my-type`, for, uh, legacy reasons to be compatible with the old tool your
system's been using as of now.

This option takes a value of the `my_type` type, so to seamlessly incorporate `my_type`
into the library, without polluting it with `operator>>`, is to specialize `type_parser<my_type>`.

This is easy, first you include `info/cli.hpp`, then you can define the specialization:
 
```c++
// project
#include "my_type.hpp"

template<>
struct info::cli::type_parser<my_type> {
    info::expected<my_type, std::string_view>
    operator()(std::string_view str) {
        if (str == "not-my-type")
            return INFO_UNEXPECTED{std::string_view{"Not my type passed"}};
        return {};
    }
};
```

This'll allow the library to make sense of a callback that requires `my_type`, be
it a variable, or a functor callback.

That's it really, you don't need to do anything to make your type sensible for the library.
Now you can just specify the - undocumented - feature of allowing the call to receive the option 
`--type`.

This, as you should be used to now, only requires a change in the `cli_parser` construction.
However, we'd like to use `my_type` as a variable, so we also declare `my_type type` just below 
the in/output strings:

```c++
std::string inp;
std::string outp;
my_type type;

// set-up parser
auto cli = info::cli::cli_parser::make(
       "i|input-file"_opt["The file to copy from"_hlp]->*inp,
       "o|output-file"_opt["The file to copy to"_hlp]->*outp,

       "type"_opt->*type,

       "v|version"_opt["Prints the version of the utility"_hlp]->*[](bool) {
         std::cout << '3' << std::endl;
         std::exit(1);
       }
)();
```

Now if we pass `not-my-type` to the `--type` parameter, the library will throw the exception
containing the error message `"Not my type passed"`, as specified.

We are now done, and just for completeness, this is the full code:

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

// project
#include "my_type.hpp"

template<>
struct info::cli::type_parser<my_type> {
    info::expected<my_type, std::string_view>
    operator()(std::string_view str) {
        if (str == "not-my-type")
            return INFO_UNEXPECTED{std::string_view{"Not my type passed"}};
        return {};
    }
};

int main(int argc, char** argv) {
    std::string inp;
    std::string outp;
    my_type type;

    // set-up parser
    auto cli = info::cli::cli_parser::make(
           "i|input-file"_opt["The file to copy from"_hlp]->*inp,
           "o|output-file"_opt["The file to copy to"_hlp]->*outp,

           "type"_opt->*type,

           "v|version"_opt["Prints the version of the utility"_hlp]->*[](bool) {
             std::cout << '3' << std::endl;
             std::exit(1);
           }
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

You can try out this example after building `cli-ex4`.
