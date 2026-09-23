PRESET ?= release
BUILD_DIR := build/$(PRESET)
APP   := $(BUILD_DIR)/bin/similar_files_checker
CACHE := $(BUILD_DIR)/CMakeCache.txt

.PHONY: all configure build run test clean

all: build

# Configure only when the build tree is missing or the presets changed
$(CACHE): CMakePresets.json
	cmake --preset $(PRESET)
	@touch $@

configure: $(CACHE)

build: $(CACHE)
	cmake --build --preset $(PRESET)

run: build
	./$(APP) $(ARGS)

test: build
	ctest --preset $(PRESET)

clean:
	rm -rf build