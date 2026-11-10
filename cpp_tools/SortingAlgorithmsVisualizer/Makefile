# Thin wrapper around the CMake presets - the real build config lives in
# CMakeLists.txt / CMakePresets.json. Run `make help` to list targets.

TARGET    := SortingAlgorithmsVisualizer
EMSDK_DIR ?= $(HOME)/emsdk   # where emsdk is installed (override: make web EMSDK_DIR=...)
LEAKS     ?= 1               # make asan LEAKS=0 to turn leak checking off entirely

.PHONY: all run release asan tsan test test-asan test-tsan web serve clean help

all: run                                   ## default: debug build + run

# build-<preset>: configure (only re-runs when needed) + build
build-%:
	cmake --preset $*
	cmake --build --preset $*
	@# point it at whichever preset you just built
	ln -sf build/$*/compile_commands.json .

# The web preset needs emsdk activated; this explicit rule overrides build-% for it.
build-web:
	@test -f $(strip $(EMSDK_DIR))/emsdk_env.sh || { echo "emsdk not found at $(strip $(EMSDK_DIR)) - run emsdk-update, or: make web EMSDK_DIR=/path/to/emsdk"; exit 1; }
	bash -c 'source $(strip $(EMSDK_DIR))/emsdk_env.sh >/dev/null 2>&1 && cmake --preset web && cmake --build --preset web'

run: build-debug                           ## debug build + run
	./build/debug/$(TARGET)

release: build-release                     ## optimized build + run
	./build/release/$(TARGET)

asan: build-asan                           ## run under ASan + UBSan
	ASAN_OPTIONS=detect_leaks=$(strip $(LEAKS)) LSAN_OPTIONS=suppressions=$(CURDIR)/lsan.supp:print_suppressions=0 \
		./build/asan/$(TARGET)

tsan: build-tsan                           ## run under ThreadSanitizer
	setarch $$(uname -m) -R ./build/tsan/$(TARGET)

test: build-debug                          ## unit tests
	ctest --preset debug

test-asan: build-asan                      ## unit tests under ASan + UBSan
	LSAN_OPTIONS=suppressions=$(CURDIR)/lsan.supp:print_suppressions=0 ctest --preset asan

test-tsan: build-tsan                      ## unit tests under TSan
	setarch $$(uname -m) -R ctest --preset tsan

web: build-web                             ## WebAssembly build (activates emsdk itself)

serve: build-web                           ## web build + serve at localhost:8000
	@echo "Open http://localhost:8000/$(TARGET).html"
	python3 -m http.server -d build/web 8000

clean:                                     ## delete all build output
	rm -rf build

help:                                      ## list targets
	@grep -E '^[a-z-]+:.*## ' $(MAKEFILE_LIST) | sed 's/:.*## /\t/'
