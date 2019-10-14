# TEST EXECUTABLE
add_executable(codeowners_tests
	attribute_set.t.cpp
	codeowners.t.cpp
	filesystem.t.cpp
	git_resources.t.cpp
	pattern_map.t.cpp
        path_sources.t.cpp
	repository.t.cpp
	types.t.cpp
        type_utils.t.cpp strong_typedef.t.cpp parser.t.cpp ruleset.t.cpp)

## Ensure that library-private headers can be included from test files:
##     #include <src/header.hpp>
target_include_directories(codeowners_tests
		PRIVATE ..
)
target_compile_options(codeowners_tests PRIVATE ${STRICT_COMPILE_OPTIONS})
target_link_libraries(codeowners_tests
	codeowners
	gtest_main
)
add_sanitizers(codeowners_tests)

add_test(NAME codeowners_tests
	 COMMAND codeowners_tests)
