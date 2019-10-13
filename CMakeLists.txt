cmake_minimum_required(VERSION 3.14)

project(codeowners-cpp
        VERSION 0.1.0
        DESCRIPTION "Command-line tool for listing code owners in a Git repository")

# Enable MACOS_RPATH policy
cmake_policy(SET CMP0042 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(STRICT_COMPILE_OPTIONS -Werror -Wall -Wextra)

## External dependencies
add_subdirectory(external/googletest)
set(BUILD_CLAR OFF CACHE BOOL "")  # Disable test suite in libgit2
add_subdirectory(external/libgit2)
add_subdirectory(external/range-v3)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})
find_package(Sanitizers)

# Boost 1.67 is the version available on Travis Mac OS X VMs.
find_package(Boost 1.60 COMPONENTS filesystem program_options REQUIRED)

## codeowners library
add_library(codeowners
        include/codeowners/codeowners.hpp
        include/codeowners/errors.hpp
        include/codeowners/filesystem.hpp
        include/codeowners/parser.hpp
        include/codeowners/path_filters.hpp
        include/codeowners/repository.hpp
        include/codeowners/ruleset.hpp
        include/codeowners/type_utils.hpp
        include/codeowners/strong_typedef.hpp
        src/attribute_set.hpp
        src/attribute_set.cpp
        src/codeowners.cpp
        src/errors.cpp
        src/git_resources.hpp
        src/git_resources.cpp
        src/parser.cpp
        src/pattern_map.hpp
        src/repository.cpp
        src/ruleset.cpp
        )
target_include_directories(codeowners
        PUBLIC include
        PRIVATE external/libgit2/include
        )
target_link_libraries(codeowners
        PUBLIC Boost_filesystem range-v3
        PRIVATE git2
        )
target_compile_options(codeowners PRIVATE ${STRICT_COMPILE_OPTIONS})
add_sanitizers(codeowners)

## Executables
add_subdirectory(apps)

## Unit test suite
enable_testing()
add_subdirectory(tests)

