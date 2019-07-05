# TEST EXECUTABLE
add_executable(codeowners_tests
    codeowners.t.cpp
)
target_link_libraries(codeowners_tests
	codeowners
	gtest_main
)
add_sanitizers(codeowners_tests)

add_test(NAME codeowners_tests
	 COMMAND codeowners_tests)
