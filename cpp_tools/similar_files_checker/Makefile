BUILD_DIR := build
TARGET := similar_files_checker
TARGET_TESTS := tests

TESTS := $(BUILD_DIR)/bin/$(TARGET_TESTS)
APP := $(BUILD_DIR)/bin/$(TARGET)

# .PHONY tells make that a target is not a real file,
# so it will always run the associated commands.
.PHONY: all configure build test run clean

all: build

configure:
	cmake -S . -B $(BUILD_DIR) \
        -DCMAKE_C_COMPILER=gcc-14 \
        -DCMAKE_CXX_COMPILER=g++-14 \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

build: configure
	cmake --build $(BUILD_DIR)

run: build
	./$(APP) $(ARGS)

test: build
	./$(TESTS)

clean:
	rm -rf $(BUILD_DIR)




