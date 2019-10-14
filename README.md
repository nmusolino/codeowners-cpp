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
* Code ownership files can be placed at one of three locations, relative to
  the repository root: `CODEOWNERS`, `.docs/CODEOWNERS`, or `.github/CODEOWNERS`.

## Usage

This repository includes a command-line utility `ls-owners`, which lists files in the
repository and their corresponding owners.

#### Listing file owners

When invoked on its own, the utility will list all files in the current directory,
and (recursively) in all subdirectories.
```
$ ls-owners
.clang-format:    @nmusolino
.github/ISSUE_TEMPLATE.md:    @nmusolino
.github/PULL_REQUEST_TEMPLATE.md:    @nmusolino
.gitignore:    @nmusolino
.gitmodules:    @nmusolino
[...]
```

The utility can be invoked with specific files or directories.

```
$ ls-owners include/
include/codeowners/codeowners.hpp:    @nmusolino
include/codeowners/errors.hpp:    @nmusolino
include/codeowners/filesystem.hpp:    @nmusolino
include/codeowners/git_resources_fwd.hpp:    @nmusolino
include/codeowners/parser.hpp:    @nmusolino
[...]
```

#### Coming soon:  specifying a CODEOWNERS file in a non-standard location
A codeowners file can be specified on the command line using the `--owners-file` option:
```
$ ls-owners --owners-file owners.txt src/    # Not yet implemented
```
When a file is specified by the user, the `ls-owners` utility will not attempt to locate
a CODEOWNERS file in the standard locations.


#### Coming soon:  filters for files

The `ls-owners` utility will be able to list files based on their status within
the git repository.

```
$ ls-owners --index-only            # Not yet implemented.
$ ls-owners --include-untracked     # Not yet implemented.
$ ls-owners --include-ignored       # Implies `--include-untracked`.  Not yet implemented.
```

#### Coming soon:  filters for owners

The `ls-owners` utility can be used to list all files with or without owners:
```
$ ls-owners --unowned src/          # Not yet implemented
$ ls-owners --owned src/            # Not yet implemented
```

List all files for which user `@fred` is an owner:
```
$ ls-owners --owner=@fred           # Not yet implemented
```

## Building the C++ project

The `codeowners-cpp` project uses the CMake build system.  The CMake system is itself
controlled by a standard Makefile (which is somewhat unconventional).

To build the `ls-owners` utility:
```
$ make ls-owners
```

To build and execute the test suite:
```
$ make test
```

Invoking `make all` is equivalent to the `test` and `ls-owners` targets:
```
$ make all
```

## Contributing to the `codeowners-cpp` project

Contributions to the `codeowners-cpp` are welcome from all users.

Feature requests and bug reports can be created using "New issue" button on
the project's [Issues](https://github.com/nmusolino/codeowners-cpp/issues) page.

Pull requests are welcome and can be created using the standard Github
pull request workflow.

## Acknowledgements

Codeowners-cpp uses the following libraries, which are included as git submodules:

* [libgit2](https://libgit2.org):
  "a portable, pure C implementation of the Git core methods."
* [santizers-cmake](https://github.com/arsenm/sanitizers-cmake):
  "CMake module to enable sanitizers for binary targets."
* [Google Test](https://github.com/google/googletest):
  Google testing and mocking framework.

The contributors to these external projects are kindly acknowledged.

## License

This project is licensed under the GNU General Public License, version 3.
See the [LICENSE](LICENSE) file for details.

