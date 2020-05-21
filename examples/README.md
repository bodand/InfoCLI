# Examples

This directory contains all example that present the way to work with InfoCLI. 
Please read them in order, and please read all of them before jumping into using the library,
along with the option matcher style used by InfoCLI defined in `docs/MatchingOptions.md`.

While the library strives to be the easiest to use, I think you should still read these
to understand the way it is supposed to be used.

## Structure

Each example directory contains a CMake project which showcases the usage of the library
with more and more complex features. 
All projects are simple, and do not follow the directory structure used by Info*
projects, because there would be more directories than files, so the source file 
for each example is just thrown there as `main.cpp`.

Other than the CMake project files, the directories contain a `doc.md` file,
which contains two parts: a tutorial part, containing the required knowledge
about the library to understand that example; and a part which explains what the example
does.
