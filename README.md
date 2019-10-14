# codeowners-cpp: library and utility for identifying code owners

This project is a C++ library and command-line application for identifying "code
owners" within a project, based on Github's CODEOWNERS file convention.

[![Build Status](https://travis-ci.com/nmusolino/codeowners-cpp.svg?branch=master)](https://travis-ci.com/nmusolino/codeowners-cpp)

## Background

Github [introduced "code owners"](https://github.blog/2017-07-06-introducing-code-owners/)
in 2017 as a way for a project to designate reviewers of files with a Git
repository.  Github repository owners can require that pull requests be reviewed
by specific users who are identified, using file patterns, in the CODEOWNERS listing.

Here is a sample CODEOWNERS file:

```
# Default ownership can be established using the `*` pattern.
*             @j_random_maintainer

# C++ files are owned by user `cpp-owner`.
# Later rules supercede earlier ones, so files matching these patterns
# will be owned by @cpp-owners, not @j_random_maintainer.
*.hpp         @cpp-owner
*.cpp         @cpp-owner

# All documentation maintained by `dholliday` and `dbrown`.
# Patterns can correspond to multiple owners.
docs/**       @dholliday @dbrown
```

* The code ownership feature is documented at Github's
["About code owners"](https://help.github.com/en/articles/about-code-owners) page.
* The CODEOWNERS file uses filename patterns, similar to `.gitignore` or `.gitattributes`
files, that are matched against files in the repository.
* Code ownership files can be placed at one of three locations, relative to a repository root:
 `CODEOWNERS`, `.docs/CODEOWNERS`, or `.github/CODEOWNERS`.

## Usage

*Coming soon.*

## Building the C++ project

*Coming soon.*

## Contributing to the `codeowners-cpp` project

*Coming soon.*

## Acknowledgements

Codeowners-cpp uses the following libraries, which are included as git submodules:

* [libgit2](https://libgit2.org): "a portable, pure C implementation of the Git core methods."
* [santizers-cmake](https://github.com/arsenm/sanitizers-cmake): "CMake module to enable sanitizers for binary targets."
* [Google Test](https://github.com/google/googletest): Google testing and mocking framework.

The contributors to these external projects are kindly acknowledged.

## License

This project is licensed under the GNU General Public License, version 3.  See the [LICENSE](LICENSE) file for details.

