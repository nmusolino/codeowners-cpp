# TEST EXECUTABLE
add_executable(codeowners_tests
        codeowners.t.cpp filesystem.t.cpp git_resources.t.cpp types.t.cpp repository.t.cpp)

target_compile_options(codeowners_tests PRIVATE ${STRICT_COMPILE_OPTIONS})
target_link_libraries(codeowners_tests
	codeowners
	gtest_main
)
add_sanitizers(codeowners_tests)

add_test(NAME codeowners_tests
	 COMMAND codeowners_tests)
