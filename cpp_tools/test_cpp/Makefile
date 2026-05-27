BUILD_DIR := build
TARGET := app
TARGET_TESTS := tests
TESTS := $(BUILD_DIR)/bin/$(TARGET_TESTS)
APP := $(BUILD_DIR)/bin/$(TARGET)

# .PHONY tells make that a target is not a real file, so it will always run the associated commands regardless of whether a file with that name exists.
.PHONY: configure build test run clean

configure:
	cmake -S . -B $(BUILD_DIR)

build: configure
	cmake --build $(BUILD_DIR)

run: build
# 	cd $(BUILD_DIR)
	./$(APP)

test: build
	./$(TESTS)

clean:
	rm -rf $(BUILD_DIR)