cmake_minimum_required(VERSION 3.14)

project(codeowners-cpp
	VERSION 0.1.0
	DESCRIPTION "Command-line tool for listing code owners in a Git repository")

set(CMAKE_CXX_STANDARD 17)


## LIBRARIES

# codeowners library
add_library(codeowners
	    include/codeowners/codeowners.hpp
	    src/codeowners.cpp)

target_include_directories(codeowners
			   PUBLIC include)
	       	

## UNIT TEST SUITE
enable_testing()
add_subdirectory(tests)

