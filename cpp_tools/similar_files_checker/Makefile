# toggle between debug and release
PRESET ?= debug

# Optional sanitizer: empty (none), asan, or tsan.
#   make run SAN=asan
#   make test SAN=tsan PRESET=release
SAN ?=

ifneq ($(filter-out asan tsan,$(SAN)),)
$(error SAN must be empty, asan or tsan (got '$(SAN)'))
endif

# Each sanitizer gets its own preset and build tree (e.g. build/debug-asan),
# so switching between them never forces a full rebuild.
CONFIG    := $(PRESET)$(if $(SAN),-$(SAN))
BUILD_DIR := build/$(CONFIG)
APP   := $(BUILD_DIR)/bin/similar_files_checker
CACHE := $(BUILD_DIR)/CMakeCache.txt

# TSan can abort at startup on newer kernels with high ASLR entropy
# ("unexpected memory mapping"); disabling address randomization avoids it.
RUN_PREFIX := $(if $(filter tsan,$(SAN)),setarch $(shell uname -m) -R)

# Changing either presets file triggers a reconfigure.
# $(wildcard ...) is empty when CMakeUserPresets.json doesn't exist.
PRESET_FILES := CMakePresets.json $(wildcard CMakeUserPresets.json)

.PHONY: all configure build run clean{% if enable_tests %} test{% endif %}


all: build

# Configure only when the build tree is missing or the presets changed.
# Assumes each configure preset uses binaryDir = ${sourceDir}/build/${presetName}.
$(CACHE): $(PRESET_FILES)
	cmake --preset $(CONFIG)
	@touch $@

configure: $(CACHE)

build: $(CACHE)
	cmake --build --preset $(CONFIG)

# Pass program arguments with: make run ARGS="--flag value"
run: build
	$(RUN_PREFIX) ./$(APP) $(ARGS)

test: build
	$(RUN_PREFIX) ctest --preset $(CONFIG)

clean:
	rm -rf build