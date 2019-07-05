cmake_minimum_required(VERSION 3.14)

project(codeowners-cpp
        VERSION 0.1.0
        DESCRIPTION "Command-line tool for listing code owners in a Git repository")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE on)


## External dependencies
add_subdirectory(external/googletest)
add_subdirectory(external/libgit2)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/sanitizers-cmake/cmake" ${CMAKE_MODULE_PATH})
find_package(Sanitizers)

# Boost 1.67 is the version available on Travis Mac OS X VMs.
find_package(Boost 1.67 COMPONENTS filesystem program_options REQUIRED)

## codeowners library
add_library(codeowners
        include/codeowners/codeowners.hpp
        src/codeowners.cpp include/codeowners/paths.hpp)

target_compile_options(codeowners PRIVATE -Werror -Wall -Wextra)

message( "LIBGIT2_INCLUDES: ${LIBGIT2_INCLUDES}" )

target_include_directories(codeowners
        PUBLIC include
        PRIVATE external/libgit2/include
        )
target_link_libraries(codeowners
        PUBLIC Boost_filesystem
        )
target_link_libraries(codeowners PRIVATE git2)
add_sanitizers(codeowners)

## Executables
add_subdirectory(apps)

## Unit test suite
enable_testing()
add_subdirectory(tests)

