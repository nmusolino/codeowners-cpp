## help             Show available targets
.PHONY : help
help: Makefile
	@echo "Makefile targets" 
	@sed -n 's/^##//p' Makefile
	@echo
	@echo User-defined variables:
	@echo "    SANITIZER  = NONE | ADDRESS | MEMORY | THREAD | UNDEFINED"
	@echo "    BUILD_TYPE = Debug | Release | ..."
	@echo
	@echo Sample invocation:
	@echo "    make BUILD_TYPE=debug SANITIZER=UNDEFINED ls-owners test"

# USER-SETTABLE VARIABLES
# Processes used during CMake build
j ?= 2
BUILD_TYPE ?= Debug
SANITIZER ?= NONE

# SOURCE FILE ENUMERATION
SOURCE_FILES = $(wildcard include/codeowners/*) $(wildcard src/*)
TEST_FILES = $(wildcard tests/*.hpp) $(wildcard tests/*.cpp) 

# BUILD OUTPUT LOCATIONS
BUILD_ROOT = build_output
BUILD_DIR = $(BUILD_ROOT)/$(BUILD_TYPE)-$(SANITIZER)
BUILD_MAKEFILE = $(BUILD_DIR)/Makefile
SANITIZER_OPTIONS = $(shell [[ $(SANITIZER) =~ "ADDRESS|MEMORY|THREAD|UNDEFINED" ]] && echo "-DSANITIZE_$(SANITIZER)=On")

MAIN_EXECUTABLE = $(BUILD_DIR)/apps/ls-owners
TEST_EXECUTABLE = $(BUILD_DIR)/tests/codeowners_tests

## ls-owners        Build ls-owners executable
$(MAIN_EXECUTABLE): $(BUILD_MAKEFILE) $(SOURCE_FILES)
	cmake --build $(BUILD_DIR) -j$(j) --target ls-owners

ls-owners: $(MAIN_EXECUTABLE)
	@echo Executable: $(MAIN_EXECUTABLE)

## build            Synonym for 'ls-owners'
build: ls-owners

## all              Build ls-owners app and run tests
all: ls-owners test

# TEST TARGETS
$(TEST_EXECUTABLE): $(BUILD_DIR) $(BUILD_MAKEFILE) $(SOURCE_FILES) $(TEST_FILES)
	cmake --build $(BUILD_DIR) -j$(j) --target codeowners_tests

## test             Run C++ unit test suite
test: $(TEST_EXECUTABLE) 
	@echo Executable: $(TEST_EXECUTABLE)
	$(TEST_EXECUTABLE)

scan-build: SANITIZER = SCAN-BUILD
scan-build: scan-build-command = scan-build -v -k -o $(BUILD_DIR)/scan-build-reports/
scan-build: $(BUILD_DIR)
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	cmake --build $(BUILD_DIR) -j$(j) --target git2
	cmake --build $(BUILD_DIR) -j$(j) --target gtest_main
	scan-build -v -k -o $(BUILD_DIR)/scan-build-reports/ cmake --build $(BUILD_DIR) -j$(j)

.PHONY: test test_asan test_msan test_ubsan

## test_asan        Build and run tests with Address Sanitizer 
test_asan: SANITIZER = ADDRESS
test_asan: $(TEST_EXECUTABLE)
	ASAN_OPTIONS=verbosity=1,exitcode=1 $(TEST_EXECUTABLE)

## test_msan        Build and run tests with Memory Sanitizer 
test_msan: SANITIZER = MEMORY
test_msan: $(TEST_EXECUTABLE)
	MSAN_OPTIONS=verbosity=1,exitcode=1 $(TEST_EXECUTABLE)

## test_ubsan       Build and run tests with Undefined Behavior Sanitizer 
test_ubsan: SANITIZER = UNDEFINED
test_ubsan: $(TEST_EXECUTABLE)
	UBSAN_OPTIONS=verbosity=1,exitcode=1 $(TEST_EXECUTABLE)

test_tsan: SANITIZER = THREAD
test_tsan: $(TEST_EXECUTABLE)
	TSAN_OPTIONS=verbosity=1,exitcode=1 $(TEST_EXECUTABLE)

## test_sanitized   Build and run unit tests with address sanitizer, memory sanitizer, and UB sanitizer
test_sanitized: test_asan test_msan test_ubsan


# INITIAL CMAKE-RELATED TARGETS
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

## cmake            Generate Makefiles using CMake
cmake: $(BUILD_MAKEFILE)
$(BUILD_MAKEFILE): $(BUILD_DIR) CMakeLists.txt tests/CMakeLists.txt
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(SANITIZER_OPTIONS)


# CLEAN TARGETS
## clean            Remove output associated with this build type and sanitizer
clean:
	rm -rf $(BUILD_DIR)

## clean_all        Remove output associated with all builds
clean_all:
	rm -rf $(BUILD_ROOT)

