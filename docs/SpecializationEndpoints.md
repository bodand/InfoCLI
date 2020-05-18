# Specialization Endpoints

The library allows some templates to be specialized in order to provide custom
functionality in the library, for supporting custom types and the error reporting.

## specialize.hpp

Under `info/cli/specialize.hpp`, all specializable templates are included without
anything getting instantiated, so if you wish to specialize anything, include this
first, define your specializations, then include `info/cli.hpp`.
The headers' structure other than these two headers is regarded as an implementation
detail, thus is volatile to depend on, as it can change even with a patch version
bump.

## Endpoints

The followings define the requirements for each type of specialization, and their
purpose along with the default unspecialized behavior.
 
### Error handling

The library defines a template called `info::cli::error_reporter<clas...>` that
gets instantiated by the library to with an empty parameter pack, thus
the user-defined error_reporter is to be specialized for an empty parameter pack.

The type shall define an `operator()` which is not supposed to return. If it returns
behavior is undefined. 
The operator, when called, is passed a(n) `std::string_view` instance.

By default, all it does is throws an instance of `info::cli::parse_exception` containing
the error message passed to it.

### Type parsing

The required parser to parse the string representation into an instance of type `T`
is implemented by the `into::cli::type_parser<class T>` template.

The type, which is a specialization of `type_parser` for type `T` 
shall define an `operator()` which shall not throw, and shall return 
an `info::expected<T, std::string_view>`. \[Note: To use `info::expected` just return the
result as-is in the happy case, or return `INFO_UNEXPECTED{"conversion to T failed"}` otherwise.]

By default, it relies on `std::istringstream` and `operator>>` being overloaded
for the type. This also requires the type to be default constructable.

Not all types are specializable at the moment, however. Allowing the overriding of the library's default 
values is work in progress.
The library provides optimized conversion into all arithmetic types and `std::string`/`~_view`.

#### Rationale for noexcept and info::expected
When the types need to be parsed from a string to get passed to the handler function,
or shoved into the reference, they get parsed which may fail. There we don't throw
an exception directly, but using `info::expected<T, E>`, where `E` is a std::string_view
containing the reason parsing failed, then use the possibly specialized error handler
to rely on the used specialization.

### Option handling

Some types may allow some special handling to happen by the option parsing
part of the library. Such special cases are stored inside the `info::cli::type_data<T>` template
instantiation.

Currently, these static members are expected to be defined for each `type_data` specialization:

 - `allow_nothing` (`bool`): Whether the type's option is not used in the form of `--option=value` the
 following token of input does not get passed into the conversion function, unlike by default.
 - `default_value` (`std::string_view`): The value to pass to the conversion function when nothing 
 is parsed from the input. This only makes sense if `allow_nothing` is set to `true`.
 - `length` (`int`): The length of the expected value. Negative value means the input is of unspecified length.
  0 is undefined.
  This is used when POSIX style short parameters are grouped and one takes an argument: 
  if that arg is of length 1, the following happens:
  `-abcd` -> `-a b -c -d`, if `-a` takes one char.
  This behavior only occurs if `expected_type` is not set to `numeric`, as for numeric types,
  the end of the option value is always distinguishable. 
 - `expected_type` (`info::cli::parse_type`): Enum value about the type to expect. Values can be either:
 `aplha`, `alphanumeric`, `numeric`. Used when `--optinonvalue` occurs, and the expected type is
 `numeric`: the value is assumed to start from the first digit till the end of the token.
 Otherwise `--optionvalue` is parsed as one long option name which will likely cause a failure.
 Other uses are described under `length`.

The conversion function refers to the `type_parser` specializable template discussed above.

By default, these values are set:
 
 - `allow_nothing`: `false`
 - `default_value`: `""`
 - `length`: `-1`
 - `expected_type`: `alphanumeric`

Not all types are specializable at the moment, however. Allowing the overriding of the library's default
values is work in progress.
The library provides specializations for the following types, with their respective values that 
deviate from the default.

 - `bool`: 
   - `allow_nothing`: `true`
   - `default_value`: `1`

 - `char`(other char-like types are not implemented by default)
   - `length`: `1`

Also, other than specializations all types that pass `std::is_arithmetic_type` have
`expected_type` set to `numeric`, except `bool`, `char`, and the `charN_t` types including
`wchar_t`.
