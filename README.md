# InfoCLI

The new generation of the mess that was InfoParse. InfoCLI doesn't have completely
random and useless capabilities, like deleting the pointer returned by the
callback function, or rerunning the callback depending on its return value.
Why did I even have those, I don't know.

Also, InfoCLI does everything it can during compile-time, expect, of course, the
parsing.

The API is different, as now we have different design goals, but the main design 
decision is still the same: have it look like Perl's `Getopt::Long` as much as possible.  

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
