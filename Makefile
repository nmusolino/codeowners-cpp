## help             Show available targets
.PHONY : help
help: Makefile
	@echo "Makefile targets" 
	@sed -n 's/^##//p' Makefile
	@echo
	@echo User-defined variables:
	@echo "    BUILD_TYPE = Debug | Release | RelWithDebInfo | ..."
	@echo
	@echo Sample invocation:
	@echo "    make BUILD_TYPE=debug ls-owners test test_asan"

SHELL = /bin/bash

# USER-SETTABLE VARIABLES
# Processes used during CMake build
j ?= 2
BUILD_TYPE ?= Debug

# SOURCE FILE ENUMERATION
CMAKE_FILES = CMakeLists.txt tests/CMakeLists.txt apps/CMakeLists.txt
# This is a bit more broad than it needs to be for each target.
SOURCE_FILES = $(wildcard include/codeowners/*) $(wildcard src/*) $(wildcard apps/*)
TEST_FILES = $(wildcard tests/*.hpp) $(wildcard tests/*.cpp) 

# BUILD OUTPUT LOCATIONS
BUILD_ROOT = build_output

$(BUILD_ROOT)/$(BUILD_TYPE)-%/Makefile : $(CMAKE_FILES)
	cmake -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(SANITIZER_CONFIG_OPTION) -S . -B $(dir $@)
	@echo "Created:  $@"

$(BUILD_ROOT)/$(BUILD_TYPE)-%/tests/codeowners_tests : $(BUILD_ROOT)/$(BUILD_TYPE)-%/Makefile $(SOURCE_FILES) $(TEST_FILES)
	cmake --build $(dir $<) -j$(j) --target codeowners_tests		
	@echo "Built:  $@"

$(BUILD_ROOT)/$(BUILD_TYPE)-%/apps/ls-owners : $(BUILD_ROOT)/$(BUILD_TYPE)-%/Makefile $(SOURCE_FILES)
	cmake --build $(dir $<) -j$(j) --target ls-owners
	@echo "Built:  $@"

## ls-owners        Build ls-owners command-line utility
ls-owners: $(BUILD_ROOT)/$(BUILD_TYPE)-nosan/apps/ls-owners
	@echo "Built:  $<"


# TESTS
## test             Run C++ unit test suite
TEST_EXECUTABLE = $(BUILD_ROOT)/$(BUILD_TYPE)-nosan/tests/codeowners_tests
test: $(TEST_EXECUTABLE) 
	$(TEST_EXECUTABLE)

all: test ls-owners

ASAN_TEST_EXECUTABLE = $(BUILD_ROOT)/$(BUILD_TYPE)-asan/tests/codeowners_tests
MSAN_TEST_EXECUTABLE = $(BUILD_ROOT)/$(BUILD_TYPE)-msan/tests/codeowners_tests
UBSAN_TEST_EXECUTABLE = $(BUILD_ROOT)/$(BUILD_TYPE)-ubsan/tests/codeowners_tests

test_asan_executable: $(ASAN_TEST_EXECUTABLE)
test_asan_executable: SANITIZER_CONFIG_OPTION = -DSANITIZE_ADDRESS=On

test_msan_executable: $(MSAN_TEST_EXECUTABLE)
test_msan_executable: SANITIZER_CONFIG_OPTION = -DSANITIZE_MEMORY=On

test_ubsan_executable: $(UBSAN_TEST_EXECUTABLE)
test_ubsan_executable: SANITIZER_CONFIG_OPTION = -DSANITIZE_UNDEFINED=On

## test_asan        Run test suite with address sanitizer
## test_msan        Run test suite with memory sanitizer
## test_ubsan       Run test suite with undefined behavior sanitizer
test_asan: test_asan_executable
	ASAN_OPTIONS=verbosity=1,exitcode=1 $(ASAN_TEST_EXECUTABLE)
test_msan: test_msan_executable
	MSAN_OPTIONS=verbosity=1,exitcode=1 $(MSAN_TEST_EXECUTABLE)
test_ubsan: test_ubsan_executable
	UBSAN_OPTIONS=verbosity=1,exitcode=1 $(UBSAN_TEST_EXECUTABLE)

.PHONY: test test_asan test_ubsan test_msan test_sanitizers
## test_sanitizers  Run test suite with all sanitizers (lengthy)
test_sanitizers: test test_asan test_ubsan test_msan

# UTILITY TARGETS
## clean            Remove all build output
clean:
	rm -rf $(BUILD_ROOT)

.git/hooks/pre-commit:  hooks/pre-commit
	cp hooks/pre-commit .git/hooks/pre-commit

## install_hooks    Install git hooks into this repo's git directory
install_hooks: .git/hooks/pre-commit
