cmake_minimum_required(VERSION 3.14)

project(codeowners-cpp
        VERSION 0.1.0
        DESCRIPTION "Command-line tool for listing code owners in a Git repository")

set(CMAKE_CXX_STANDARD 17)

set(CODEOWNERS_COMPILE_OPTIONS -Werror -Wall -Wextra)


## External dependencies
add_subdirectory(external/googletest)
add_subdirectory(external/libgit2)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})
find_package(Sanitizers)

# Boost 1.67 is the version available on Travis Mac OS X VMs.
find_package(Boost 1.67 COMPONENTS filesystem program_options REQUIRED)

## Dummy target for compiler options
add_library(codeowners_compile_options src/null.cpp)
target_compile_options(codeowners_compile_options
        INTERFACE -Werror -Wall -Wextra)

## codeowners library
add_library(codeowners
        include/codeowners/codeowners.hpp
        include/codeowners/paths.hpp
        src/codeowners.cpp)
target_include_directories(codeowners
        PUBLIC include
        PRIVATE external/libgit2/include
        )
target_link_libraries(codeowners
        PUBLIC Boost_filesystem
        PRIVATE codeowners_compile_options git2
        )
add_sanitizers(codeowners)

## Executables
add_subdirectory(apps)

## Unit test suite
enable_testing()
add_subdirectory(tests)

