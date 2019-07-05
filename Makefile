## help:        Show available targets
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

# Processes used during CMake build
j = 2  

SOURCE_FILES = $(wildcard include/codeowners/*) $(wildcard src/*)
TEST_FILES = $(wildcard tests/*.hpp) $(wildcard tests/*.cpp) 

BUILD_ROOT = build_output
BUILD_TYPE ?= Debug
SANITIZER ?= NONE
BUILD_DIR = $(BUILD_ROOT)/$(BUILD_TYPE)-$(SANITIZER)

BUILD_MAKEFILE = $(BUILD_DIR)/Makefile

SANITIZER_OPTIONS = $(shell [ $(SANITIZER) = "NONE" ] && echo '' || echo "-DSANITIZE_$(SANITIZER)=On")

## ls-owners:   Build ls-owners executable
MAIN_EXECUTABLE = $(BUILD_DIR)/apps/ls-owners
$(MAIN_EXECUTABLE): $(BUILD_MAKEFILE) $(SOURCE_FILES)
	cmake --build $(BUILD_DIR) -j$(j) --target ls-owners

ls-owners: $(MAIN_EXECUTABLE)
	@echo Executable: $(MAIN_EXECUTABLE)

## test:        Run C++ unit test suite
TEST_EXECUTABLE = $(BUILD_DIR)/tests/codeowners_tests
$(TEST_EXECUTABLE): $(BUILD_MAKEFILE) $(SOURCE_FILES) $(TEST_FILES)
	cmake --build $(BUILD_DIR) -j$(j) --target codeowners_tests

.PHONY: test
test: $(TEST_EXECUTABLE) 
	@echo Executable: $(TEST_EXECUTABLE)
	$(TEST_EXECUTABLE)


.PHONY: test_asan test_msan test_ubsan

## test_asan    Build and run tests with Address Sanitizer 
test_asan: SANITIZER = ADDRESS
test_asan: $(TEST_EXECUTABLE)
	ASAN_OPTIONS=verbosity=1,exitcode=1 $(TEST_EXECUTABLE)

## test_msan    Build and run tests with Memory Sanitizer 
test_msan: SANITIZER = MEMORY
test_msan: $(TEST_EXECUTABLE)
	MSAN_OPTIONS=verbosity=1,exitcode=1 $(TEST_EXECUTABLE)

## test_ubsan   Build and run tests with Undefined Behavior Sanitizer 
test_ubsan: SANITIZER = UNDEFINED
test_ubsan: $(TEST_EXECUTABLE)
	UBSAN_OPTIONS=verbosity=1,exitcode=1 $(TEST_EXECUTABLE)

test_tsan: SANITIZER = THREAD
test_tsan: $(TEST_EXECUTABLE)
	TSAN_OPTIONS=verbosity=1,exitcode=1 $(TEST_EXECUTABLE)

sanitized_tests: test_asan test_msan test_ubsan

## cmake:       Generate Makefiles using CMake 
cmake: $(BUILD_MAKEFILE)
$(BUILD_MAKEFILE): CMakeLists.txt tests/CMakeLists.txt
	mkdir -p $(BUILD_DIR)
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) $(SANITIZER_OPTIONS)

## clean:       Remove output associated with this build type and sanitizer
clean:
	rm -rf $(BUILD_DIR)

## clean_all:   Remove output associated with all builds
clean_all:
	rm -rf $(BUILD_ROOT)
