# codeowners-cpp: library and utility for identifying code owners

This project is a C++ library and command-line application for identifying "code
owners" within a project, based on Github's CODEOWNERS file convention.

## Background

Github [introduced "code owners"](https://github.blog/2017-07-06-introducing-code-owners/)
in 2017 as a way for a project to designate reviewers of files with a Git
repository.  According to Github's
[reference page](https://help.github.com/en/articles/about-code-owners),
a CODEOWNERS file can be placed at one of three locations, and uses filename
patterns, similar to a `.gitignore` or `.gitattributes` file.

Here is a sample CODEOWNERS file:

```
*             @global-owner1 @global-owner2
*.js          @js-owner
*.go          @go-owner
/build/logs/  @doctocat
apps/         @octocat
docs/*        @doc-user
/docs/        @doc-user
```

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

