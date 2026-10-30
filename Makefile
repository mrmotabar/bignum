CXX ?= clang++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O3 -Iinclude

BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin

TARGET_CLI = $(BIN_DIR)/bignum
TARGET_TEST = $(BIN_DIR)/test-bignum
TARGET_BENCH = $(BIN_DIR)/benchmark

.PHONY: all clean test benchmark directories

all: directories $(TARGET_CLI) $(TARGET_TEST) $(TARGET_BENCH)

directories:
	@mkdir -p $(BIN_DIR)

$(TARGET_CLI): src/main.cpp include/bignum/bignum.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(TARGET_TEST): tests/test-bignum.cpp include/bignum/bignum.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(TARGET_BENCH): tests/benchmark.cpp include/bignum/bignum.hpp
	$(CXX) $(CXXFLAGS) $< -o $@

test: directories $(TARGET_TEST)
	@echo "Running Bignum test suite..."
	@$(TARGET_TEST)

benchmark: directories $(TARGET_BENCH)
	@echo "Running Bignum benchmark suite..."
	@$(TARGET_BENCH)

run: directories $(TARGET_CLI)
	@$(TARGET_CLI)

clean:
	@rm -rf $(BUILD_DIR)
